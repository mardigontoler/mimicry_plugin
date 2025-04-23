
#include "MultiPhaseVocoder.h"


using namespace PV;

static constexpr float tau = 2 * juce::MathConstants<float>::pi;


MultiPhaseVocoder::MultiPhaseVocoder(const size_t numVocoders)
        :
		forwardFFT(PV::fftOrder),
        inverseFFT(PV::fftOrder),
        window(PV::FFT_SIZE, juce::dsp::WindowingFunction<float>::WindowingMethod::hann),
        mNumVocoders(numVocoders)
{
	for (size_t ix = 0; ix < numVocoders; ix++)
	{
		mOutputSections.emplace_back(OutputSection{});
	}

    for(size_t k = 0; k < PV::FFT_SIZE; k++)
    {
        omegas[k] = (tau * static_cast<float>(k)) / PV::FFT_SIZE;
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
    fifoIndex = (fifoIndex + 1) % PV::FFT_SIZE;
    fifosWritten++;
    if (fifosWritten == PV::FFT_SIZE)
    {
		jassert(analysisHopSize <= fifosWritten);
        fifosWritten -= analysisHopSize;

		for (size_t i = 0; i < PV::FFT_SIZE; i++) {
			timeDomainRealTmp[i] = fifo[(fifoRead + i) % PV::FFT_SIZE];
		}
		window.multiplyWithWindowingTable(timeDomainRealTmp.data(), PV::FFT_SIZE);

		// copy fifo complex values
		for (size_t j = 0; j < PV::FFT_SIZE; j++) {
			timeDomainTmp[j] = {timeDomainRealTmp[j], 0};
		}

		forwardFFT.perform(timeDomainTmp.data(), freqDomainTmp.data(), false);

		for (auto& section : mOutputSections)
		{
			// copy the frequency domain data into each pitch shifter output section
			section.freqFftData = freqDomainTmp;
		}

		// TODO take section as parameter
		phaseCorrect(); // mutates freqFftData to

		for (auto& section : mOutputSections)
		{
			if(!juce::approximatelyEqual(section.factor, 1.0f)) { // pitch shift

				const auto &sz = PV::FFT_SIZE;
				inverseFFT.perform(section.freqFftData.data(), section.inverseFftOutput.data(), true);
				for (size_t ix = 0; ix < sz; ix++) {
					section.inverseFftRealOutput[ix] = section.inverseFftOutput[ix].real();
				}

				window.multiplyWithWindowingTable(section.inverseFftRealOutput.data(), PV::FFT_SIZE);

				for (size_t i = 0; i < PV::FFT_SIZE; i++) {
					// scale amplitude down by number of overlapping windows, and write to output buffer, directly where
					// it's being read from
					section.outputData[(static_cast<size_t>(floor(outputIndex)) + i) % section.outputData.size()] +=
							section.inverseFftRealOutput[i] / static_cast<float>(PV::analysisOverlapFactor);
				}
			}
			else { // factor is 1, so don't pitch shift
				// pass the new fifo samples straight to the output buffer
				for (size_t i = 0; i < analysisHopSize; i++) {
					section.outputData[(static_cast<size_t>(floor(outputIndex)) + i) % section.outputData.size() ] = fifo[(fifoRead + i) % PV::FFT_SIZE];
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
    if ( ! outputReady) {
		return 0;
	}

	if (vocoderIx >= mOutputSections.size())
	{
		jassertfalse;
		return 0;
	}

	auto& section = mOutputSections[vocoderIx];

	// move the output read head by the ratio
	// Return linearly interpolated values.
	const size_t leftIndex = floor(outputIndex);
	const double sample1 = section.outputData[leftIndex];
	const auto frac = outputIndex - static_cast<double>(leftIndex);
	outputIndex += section.factor;
	while(static_cast<size_t>(outputIndex) >= section.outputData.size())
	{
		outputIndex -= static_cast<double>(section.outputData.size());
	}
	const size_t rightIndex = (leftIndex + 1) % section.outputData.size();
	const double sample2 = section.outputData[rightIndex];

	// erase an output buffer sample once the smoothed output read head moves past it completely
	size_t clearIndex = lastLeftIndex;
	while(clearIndex != leftIndex)
	{
		section.outputData[clearIndex] = 0;
		clearIndex = (clearIndex + 1) % section.outputData.size();
	}
	lastLeftIndex = leftIndex;

	return static_cast<float>(sample1 + frac * (sample2 - sample1));
}



void MultiPhaseVocoder::phaseCorrect() {

	for (auto & section : mOutputSections)
	{
		auto& oldInputPhases = section.oldInputPhases;
		auto& oldOutputPhases = section.oldOutputPhases;
		auto& freqFftData = section.freqFftData;

		for(size_t i = 0 ; i < PV::FFT_SIZE; i++) {

			const float inputPhase = std::arg(freqFftData[i]);
			const float omega = omegas[i];

			float deltaInputPhase = inputPhase - oldInputPhases[i] - (static_cast<float>(analysisHopSize) * omega);
			deltaInputPhase = deltaInputPhase - tau * std::round(deltaInputPhase / tau);
			oldInputPhases[i] = inputPhase;
			float instantaneousFrequency = omega + (deltaInputPhase / static_cast<float>(analysisHopSize));

			float outputPhase = oldOutputPhases[i] +(static_cast<float>(section.synthesisHopSize) * instantaneousFrequency);
			outputPhase -= tau * std::round(outputPhase / tau);

			oldOutputPhases[i] = outputPhase;

			// complex multiplication to rotate the fft values so that they match the target output phases
			freqFftData[i] *= std::polar(1.0f, outputPhase - inputPhase);
		}
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

//PhaseVocoder::Analyzer::Analyzer()
//{
//}


//void PhaseVocoder::Analyzer::pushSample(float sample) noexcept
//{
//	// add the new sample to the fifo.
//    // once we have enough samples to do the first fft,
//    // do it, grabbing samples from the fifo starting from a fifoStart index
//    // that moves by the hop size every time the fft is taken. (it should wrap around)
//
//    // output is written to the output buffer
//
//    // we can bypass the actual processing when the factor is 1 (no pitch shift)
//    // but we should still be adding samples so that the queue stays full
//
//    fifo[fifoIndex] = sample;
//    fifoIndex = (fifoIndex + 1) % PV::FFT_SIZE;
//    fifosWritten++;
//    if (fifosWritten == PV::FFT_SIZE)
//    {
//        fifosWritten -= analysisHopSize;
//
//        if(!juce::approximatelyEqual(factor, 1.0f)) { // pitch shift
//
//            for(int i = 0; i < PV::FFT_SIZE; i++){
//                tmp[i] = fifo[(fifoRead + i) % PV::FFT_SIZE];
//            }
//            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);
//
//            // copy fifo complex values
//            for (int j = 0; j < PV::FFT_SIZE; j++) {
//                timeFftData[j] = {tmp[j], 0};
//            }
//
//            forwardFFT.perform(timeFftData.data(), freqFftData.data(), false);
//
//            phaseCorrect(); // mutates freqFftData to
//
//            inverseFFT.perform(freqFftData.data(), timeFftData.data(), true);
//
//
//            for (int i = 0; i < PV::FFT_SIZE; i++) {
//                float nextOutput = timeFftData[i].real();
//                tmp[i] = nextOutput;
//            }
//
//
//            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);
//
//            for (int i = 0; i < PV::FFT_SIZE; i++) {
//                // scale amplitude down by number of overlapping windows, and write to output buffer, directly where
//                // it's being read from
//                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size()] +=
//                        tmp[i] / static_cast<float>(PV::analysisOverlapFactor);
//            }
//        }
//        else { // factor is 1, so don't pitch shift
//            // pass the new fifo samples straight to the output buffer
//            for (int i = 0; i < analysisHopSize; i++) {
//                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size() ] = fifo[(fifoRead + i) % PV::FFT_SIZE];
//            }
//        }
//
//        outputReady = true;
//
//        fifoRead = juce::negativeAwareModulo((fifoRead) + analysisHopSize , PV::FFT_SIZE);
//
//    }
//}

//float PhaseVocoder::Analyzer::nextSample()
//{
//}


[[maybe_unused]] size_t MultiPhaseVocoder::getDelay() const {
    return analysisHopSize;
}
