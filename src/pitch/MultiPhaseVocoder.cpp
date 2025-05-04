
#include "MultiPhaseVocoder.h"


using namespace PV;

static constexpr float tau = 2 * juce::MathConstants<float>::pi;


MultiPhaseVocoder::MultiPhaseVocoder(const size_t numVocoders)
        :
		forwardFFT(fftOrder),
        inverseFFT(fftOrder),
        window(FFT_SIZE, juce::dsp::WindowingFunction<float>::WindowingMethod::hann),
        mNumVocoders(numVocoders)
{
	for (size_t ix = 0; ix < numVocoders; ix++)
	{
		mOutputSections.emplace_back(OutputSection{});
	}

    for(size_t k = 0; k < FFT_SIZE; k++)
    {
    	const auto omega = (tau * static_cast<float>(k)) / FFT_SIZE;
        omegas[k] = omega;
    	analysisHopSizeScaledOmegas[k] = analysisHopSize * omega;
    }
}



void MultiPhaseVocoder::pushSample(float sample) noexcept {
    // add the new sample to the fifo.
    // once we have enough samples to do the first fft,
    // do it, grabbing samples from the fifo starting from a fifoStart index
    // that moves by the hop size every time the fft is taken. (it should wrap around)

    // output is written to the output buffer

    // we can bypass the actual processing when the factor is 1 (no pitch shift)
    // but we should still be adding samples so that the queue stays full

    fifo[fifoIndex] = sample;
    fifoIndex = (fifoIndex + 1) % FFT_SIZE;
    fifosWritten++;
    if (fifosWritten == FFT_SIZE)
    {
    	// we now have enough samples to do processing.

		jassert(analysisHopSize <= fifosWritten);
        fifosWritten -= analysisHopSize;

		for (size_t i = 0; i < FFT_SIZE; i++) {
			timeDomainRealTmp[i] = fifo[(fifoRead + i) % FFT_SIZE];
		}
		window.multiplyWithWindowingTable(timeDomainRealTmp.data(), FFT_SIZE);

		// copy fifo complex values
		for (size_t j = 0; j < FFT_SIZE; j++) {
			timeDomainTmp[j] = {timeDomainRealTmp[j], 0};
		}

		forwardFFT.perform(timeDomainTmp.data(), freqDomainTmp.data(), false);

		for (auto& section : mOutputSections)
		{
			// copy the frequency domain data into each pitch shifter output section
			section.freqFftData = freqDomainTmp;
		}

		for (auto& section : mOutputSections)
		{
			phaseCorrect(section); // mutates freqFftData to

			if(!juce::approximatelyEqual(section.factor, 1.0f)) { // pitch shift

				const auto &sz = FFT_SIZE;
				inverseFFT.perform(section.freqFftData.data(), section.inverseFftOutput.data(), true);
				for (size_t ix = 0; ix < sz; ix++) {
					section.inverseFftRealOutput[ix] = section.inverseFftOutput[ix].real();
				}

				window.multiplyWithWindowingTable(section.inverseFftRealOutput.data(), FFT_SIZE);

				for (size_t i = 0; i < FFT_SIZE; i++) {
					// scale amplitude down by number of overlapping windows, and write to output buffer, directly where
					// it's being read from
					section.outputData[(static_cast<size_t>(floor(section.outputIndex)) + i) % section.outputData.size()] +=
							section.inverseFftRealOutput[i] / static_cast<float>(analysisOverlapFactor);
				}
			}
			else { // factor is 1, so don't pitch shift
				// pass the new fifo samples straight to the output buffer
				for (size_t i = 0; i < analysisHopSize; i++) {
					section.outputData[(static_cast<size_t>(floor(section.outputIndex)) + i) % section.outputData.size() ] = fifo[(fifoRead + i) % PV::FFT_SIZE];
				}
			}

		}

        outputReady = true;

        fifoRead = static_cast<size_t>(
				juce::negativeAwareModulo(static_cast<int>((fifoRead) + analysisHopSize) , PV::FFT_SIZE)
				);

    }
}


float MultiPhaseVocoder::nextSample(size_t vocoderIx)
{

	if (vocoderIx >= mOutputSections.size())
	{
		jassertfalse;
		return 0;
	}

	auto& section = mOutputSections[vocoderIx];

	// move the output read head by the ratio
	// Return linearly interpolated values.
	const size_t leftIndex = floor(section.outputIndex);
	const double sample1 = section.outputData[leftIndex];
	const auto frac = section.outputIndex - static_cast<double>(leftIndex);
	section.outputIndex += section.factor;
	while(static_cast<size_t>(section.outputIndex) >= section.outputData.size())
	{
		section.outputIndex -= static_cast<double>(section.outputData.size());
	}
	const size_t rightIndex = (leftIndex + 1) % section.outputData.size();
	const double sample2 = section.outputData[rightIndex];

	// erase an output buffer sample once the smoothed output read head moves past it completely
	size_t clearIndex = section.lastLeftIndex;
	while(clearIndex != leftIndex)
	{
		section.outputData[clearIndex] = 0;
		clearIndex = (clearIndex + 1) % section.outputData.size();
	}
	section.lastLeftIndex = leftIndex;

	return static_cast<float>(sample1 + frac * (sample2 - sample1));
}


// the original phase adjustment function, used before the SIMD was added. Leaving in for reference,
// and to test the new version against
void MultiPhaseVocoder::phaseCorrect(OutputSection& section) const
{
	using namespace juce::dsp;

	auto& oldInputPhases = section.oldInputPhases;
	auto& oldOutputPhases = section.oldOutputPhases;
	auto& freqFftData = section.freqFftData;

	jassert(JUCE_USE_SIMD);

	constexpr size_t realFloatSimdRegSz = SIMDRegister<float>::SIMDNumElements;
	constexpr size_t complexFloatSimdRegSz = SIMDRegister<std::complex<float>>::SIMDNumElements;

	jassert(FFT_SIZE % complexFloatSimdRegSz == 0);

	for(size_t i = 0 ; i < FFT_SIZE; i++) {

		const float inputPhase = std::arg(freqFftData[i]); // std::atan2(std::imag(z), std::real(z))
		const float omega = omegas[i];

		float deltaInputPhase = inputPhase - oldInputPhases[i] - (static_cast<float>(analysisHopSize) * omega);
		deltaInputPhase = deltaInputPhase - tau * std::round(deltaInputPhase / tau);
		oldInputPhases[i] = inputPhase;
		const float instantaneousFrequency = omega + (deltaInputPhase / static_cast<float>(analysisHopSize));

		float outputPhase = oldOutputPhases[i] + (static_cast<float>(section.synthesisHopSize) * instantaneousFrequency);
		outputPhase -= tau * std::round(outputPhase / tau);

		oldOutputPhases[i] = outputPhase;

		// complex multiplication to rotate the fft values so that they match the target output phases
		freqFftData[i] *= std::polar(1.0f, outputPhase - inputPhase);
	}
}



void MultiPhaseVocoder::phaseCorrectSIMD(OutputSection& section) const
{
	using namespace juce::dsp;

	auto& oldInputPhases = section.oldInputPhases;
	auto& oldOutputPhases = section.oldOutputPhases;
	auto& freqFftData = section.freqFftData;

	jassert(JUCE_USE_SIMD);

	constexpr size_t realFloatSimdRegSz = SIMDRegister<float>::SIMDNumElements;
	constexpr size_t complexFloatSimdRegSz = SIMDRegister<std::complex<float>>::SIMDNumElements;

	jassert(FFT_SIZE % complexFloatSimdRegSz == 0);

	// get complex arugment (phase angle) of FFT frequency bins

	float inputPhase[FFT_SIZE]{};

	// separate frequency bins into real and imaginary scalar arrays
	for (size_t ix = 0; ix < FFT_SIZE; ++ix)
	{
		inputPhase[ix] = std::arg(freqFftData[ix]); // std::atan2(std::imag(z), std::real(z))
	}

	const auto tauReg = SIMDRegister<float>(tau);

	for (size_t offset = 0; offset < FFT_SIZE; offset += realFloatSimdRegSz)
	{
		auto inputPhaseReg = SIMDRegister<float>::fromRawArray(inputPhase + offset);
		auto omegasReg = SIMDRegister<float>::fromRawArray(omegas.data() + offset);
		auto analysisHopScaledOmegas = SIMDRegister<float>::fromRawArray(analysisHopSizeScaledOmegas.data() + offset);

		auto oldInputPhaseReg = SIMDRegister<float>::fromRawArray(oldInputPhases.data() + offset);
		auto deltaInputPhaseReg = inputPhaseReg - oldInputPhaseReg;
		deltaInputPhaseReg -= analysisHopScaledOmegas;

		inputPhaseReg.copyToRawArray(oldInputPhases.data() + offset);


	}


}


void MultiPhaseVocoder::setPitchShiftSemitones(size_t vocoderIx, const float numSemitones)
{
	if (vocoderIx >= mOutputSections.size())
	{
		jassertfalse;
		return;
	}
	auto& section = mOutputSections[vocoderIx];
    section.factor = static_cast<float>(pow(2,  numSemitones / 12.0f));
    section.synthesisHopSize = static_cast<int>(section.factor * static_cast<float>(analysisHopSize));
}


[[maybe_unused]] size_t MultiPhaseVocoder::getDelay() const {
    return analysisHopSize;
}
