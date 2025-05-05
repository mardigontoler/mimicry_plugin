

#include "pitch_functions.h"


using namespace PV;
static constexpr float tau = 2 * juce::MathConstants<float>::pi;


MultiPhaseVocoder::MultiPhaseVocoder(const size_t numVocoders)
		:
		forwardFFT(PvConstants::fftOrder),
        inverseFFT(PvConstants::fftOrder),
        window(PvConstants::FFT_SIZE, juce::dsp::WindowingFunction<float>::WindowingMethod::hann),
        mNumVocoders(numVocoders)
{
	for (size_t ix = 0; ix < numVocoders; ix++)
	{
		mOutputSections.emplace_back(OutputSection{});
	}

    for(size_t k = 0; k < PvConstants::FFT_SIZE; k++)
    {
    	const auto omega = (tau * static_cast<float>(k)) / PvConstants::FFT_SIZE;
        omegas[k] = omega;
    	analysisHopSizeScaledOmegas[k] = PvConstants::analysisHopSize * omega;
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

	auto& FFT_SIZE = PvConstants::FFT_SIZE;
	auto& analysisHopSize = PvConstants::analysisHopSize;
	auto& analysisOverlapFactor = PvConstants::analysisOverlapFactor;

    fifo[fifoIndex] = sample;
    fifoIndex = (fifoIndex + 1) % PvConstants::FFT_SIZE;
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
					section.outputData[(static_cast<size_t>(floor(section.outputIndex)) + i) % section.outputData.size() ] = fifo[(fifoRead + i) % FFT_SIZE];
				}
			}

		}

        outputReady = true;

        fifoRead = static_cast<size_t>(
				juce::negativeAwareModulo(static_cast<int>((fifoRead) + analysisHopSize) , FFT_SIZE)
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
void MultiPhaseVocoder::phaseCorrect(OutputSection& section)
{
	using namespace juce::dsp;

	auto& FFT_SIZE = PvConstants::FFT_SIZE;
	auto& analysisHopSize = PvConstants::analysisHopSize;

	auto& oldInputPhases = section.oldInputPhases;
	auto& oldOutputPhases = section.oldOutputPhases;
	auto& freqFftData = section.freqFftData;

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



void MultiPhaseVocoder::phaseCorrectSIMD(OutputSection& section)
{
	using namespace juce::dsp;

	auto& oldInputPhases = section.oldInputPhases;
	auto& oldOutputPhases = section.oldOutputPhases;
	auto& freqFftData = section.freqFftData;

	PvConstants constants;

	pitch_functions::PhaseCorrectArgs args{
		oldInputPhases.data(),
		oldOutputPhases.data(),
		freqFftData.data(),
		omegas.data(),
		section.synthesisHopSize,
		constants
	};

	pitch_functions::PhaseCorrectSIMD(&args);
}


void MultiPhaseVocoder::setPitchShiftSemitones(size_t vocoderIx, const float numSemitones)
{
	auto& analysisHopSize = PvConstants::analysisHopSize;

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
	auto& analysisHopSize = PvConstants::analysisHopSize;
    return analysisHopSize;
}
