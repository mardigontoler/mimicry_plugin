
#include "PhaseVocoder.h"


static constexpr float tau = 2 * juce::MathConstants<float>::pi;


PhaseVocoder::PhaseVocoder()
        :
        forwardFFT(PV::fftOrder),
        inverseFFT(PV::fftOrder),
        window(PV::FFT_SIZE, juce::dsp::WindowingFunction<float>::WindowingMethod::hann)
{
    for(int k = 0; k < PV::FFT_SIZE; k++)
    {
        omegas[k] = (tau * static_cast<float>(k)) / PV::FFT_SIZE;
    }
}



void PhaseVocoder::pushSample(float sample) noexcept {
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
        fifosWritten -= analysisHopSize;

        if(!juce::approximatelyEqual(factor, 1.0f)) { // pitch shift

            for(int i = 0; i < PV::FFT_SIZE; i++){
                tmp[i] = fifo[(fifoRead + i) % PV::FFT_SIZE];
            }
            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);

            // copy fifo complex values
            for (int j = 0; j < PV::FFT_SIZE; j++) {
                timeFftData[j] = {tmp[j], 0};
            }

            forwardFFT.perform(timeFftData.data(), freqFftData.data(), false);

            phaseCorrect(); // mutates freqFftData to

            inverseFFT.perform(freqFftData.data(), timeFftData.data(), true);


            for (int i = 0; i < PV::FFT_SIZE; i++) {
                float nextOutput = timeFftData[i].real();
                tmp[i] = nextOutput;
            }


            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);

            for (int i = 0; i < PV::FFT_SIZE; i++) {
                // scale amplitude down by number of overlapping windows, and write to output buffer, directly where
                // it's being read from
                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size()] +=
                        tmp[i] / static_cast<float>(PV::analysisOverlapFactor);
            }
        }
        else { // factor is 1, so don't pitch shift
            // pass the new fifo samples straight to the output buffer
            for (int i = 0; i < analysisHopSize; i++) {
                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size() ] = fifo[(fifoRead + i) % PV::FFT_SIZE];
            }
        }

        outputReady = true;

        fifoRead = juce::negativeAwareModulo((fifoRead) + analysisHopSize , PV::FFT_SIZE);

    }
}


float PhaseVocoder::nextSample() {
    if (outputReady)
    {
        // move the output read head by the ratio
        // Return linearly interpolated values.
        const int leftIndex = floor(outputIndex);
        const double sample1 = outputData[leftIndex];
        const auto frac = outputIndex - static_cast<double>(leftIndex);
        outputIndex += factor;
        while(static_cast<size_t>(outputIndex) >= outputData.size())
        {
            outputIndex -= static_cast<double>(outputData.size());
        }
        const int rightIndex = (leftIndex + 1) % static_cast<int>(outputData.size());
        const double sample2 = outputData[rightIndex];

        // erase an output buffer sample once the smoothed output read head moves past it completely
        size_t clearIndex = lastLeftIndex;
        while(clearIndex != leftIndex)
        {
            outputData[clearIndex] = 0;
            clearIndex = (clearIndex + 1) % outputData.size();
        }
        lastLeftIndex = leftIndex;

        return static_cast<float>(sample1 + frac * (sample2 - sample1));

    }
    return 0;
}



void PhaseVocoder::phaseCorrect() {

    for(int i = 0 ; i < PV::FFT_SIZE; i++) {

        const float inputPhase = std::arg(freqFftData[i]);
        const float omega = omegas[i];
        float deltaInputPhase = inputPhase - oldInputPhases[i] - (static_cast<float>(analysisHopSize) * omega);
        deltaInputPhase = deltaInputPhase - tau * round(deltaInputPhase / tau);
        oldInputPhases[i] = inputPhase;
        float instantaneousFrequency = omega + (deltaInputPhase / static_cast<float>(analysisHopSize));

        float outputPhase = oldOutputPhases[i] +(static_cast<float>(synthesisHopSize) * instantaneousFrequency);
        outputPhase -= tau * round(outputPhase / tau);

        oldOutputPhases[i] = outputPhase;

        // complex multiplication to rotate the fft values so that they match the target output phases
        freqFftData[i] *= std::polar(1.0f, outputPhase - inputPhase);
    }
}

void PhaseVocoder::setPitchShiftSemitones(const float numSemitones) {
    factor = static_cast<float>(pow(2,  numSemitones / 12.0f));
    synthesisHopSize = static_cast<int>(factor * static_cast<float>(analysisHopSize));
}

PhaseVocoder::Analyzer::Analyzer()
{
}


void PhaseVocoder::Analyzer::pushSample(float sample) noexcept
{
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
        fifosWritten -= analysisHopSize;

        if(!juce::approximatelyEqual(factor, 1.0f)) { // pitch shift

            for(int i = 0; i < PV::FFT_SIZE; i++){
                tmp[i] = fifo[(fifoRead + i) % PV::FFT_SIZE];
            }
            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);

            // copy fifo complex values
            for (int j = 0; j < PV::FFT_SIZE; j++) {
                timeFftData[j] = {tmp[j], 0};
            }

            forwardFFT.perform(timeFftData.data(), freqFftData.data(), false);

            phaseCorrect(); // mutates freqFftData to

            inverseFFT.perform(freqFftData.data(), timeFftData.data(), true);


            for (int i = 0; i < PV::FFT_SIZE; i++) {
                float nextOutput = timeFftData[i].real();
                tmp[i] = nextOutput;
            }


            window.multiplyWithWindowingTable(tmp.data(), PV::FFT_SIZE);

            for (int i = 0; i < PV::FFT_SIZE; i++) {
                // scale amplitude down by number of overlapping windows, and write to output buffer, directly where
                // it's being read from
                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size()] +=
                        tmp[i] / static_cast<float>(PV::analysisOverlapFactor);
            }
        }
        else { // factor is 1, so don't pitch shift
            // pass the new fifo samples straight to the output buffer
            for (int i = 0; i < analysisHopSize; i++) {
                outputData[(static_cast<int>(floor(outputIndex)) + i) % outputData.size() ] = fifo[(fifoRead + i) % PV::FFT_SIZE];
            }
        }

        outputReady = true;

        fifoRead = juce::negativeAwareModulo((fifoRead) + analysisHopSize , PV::FFT_SIZE);

    }
}

float PhaseVocoder::Analyzer::nextSample()
{
}


int PhaseVocoder::getDelay() const {
    return analysisHopSize;
}
