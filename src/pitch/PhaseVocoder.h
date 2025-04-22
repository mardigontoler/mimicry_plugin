
#pragma once

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

namespace PV
{
    static constexpr unsigned int fftOrder = 11u;
    static constexpr int FFT_SIZE = 1u << fftOrder;
    static constexpr int analysisOverlapFactor = 8;
}

class PhaseVocoder {


public:

    PhaseVocoder();

    ~PhaseVocoder() = default;

    void pushSample(float sample) noexcept;

    float nextSample();

    [[nodiscard]] int getDelay() const;

    void setPitchShiftSemitones(float numSemitones);


    // The decoupled analysis step
    class Analyzer
    {
    public:
        Analyzer();

        void pushSample(float sample) noexcept;
        float nextSample();

    private:
        std::array<float, PV::FFT_SIZE> fifo{};

        int analysisHopSize = PV::FFT_SIZE / PV::analysisOverlapFactor;
        float factor = 1.0f;
        std::array<float, PV::FFT_SIZE> tmp{}; // misc temporary real data

        int fifoIndex = 0;
        int fifosWritten = 0;
        int fifoRead = 0;
        int lastLeftIndex = 0;
        double outputIndex = 0;
    };

protected:

    int analysisHopSize = PV::FFT_SIZE / PV::analysisOverlapFactor;
    float factor = 1.0f;
    int synthesisHopSize = static_cast<int>(factor * static_cast<float>(analysisHopSize));

    static constexpr float maxFactor = 2.0f;
    static constexpr float minFactor = 0.5f;


    juce::dsp::FFT forwardFFT;
    juce::dsp::FFT inverseFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, PV::FFT_SIZE> fifo{};
    std::array<float, PV::FFT_SIZE> tmp{}; // misc temporary real data
    std::array<juce::dsp::Complex<float>, PV::FFT_SIZE> timeFftData; // buffer for time domain data (complex)
    std::array<juce::dsp::Complex<float>, PV::FFT_SIZE> freqFftData; // buffer for freq domain data

    std::array<float, PV::FFT_SIZE> oldInputPhases{};
    std::array<float, PV::FFT_SIZE> oldOutputPhases{};
    std::array<float, (4 * PV::FFT_SIZE * static_cast<int>(maxFactor))> outputData{};

    // These stays constant for a particular value of N. Represents how much to propgate the phase of each frequency bin
    std::array<float, PV::FFT_SIZE> omegas{};

    bool outputReady = false;

    int fifoIndex = 0;
    int fifosWritten = 0;
    int fifoRead = 0;
    int lastLeftIndex = 0;
    double outputIndex = 0;

    void phaseCorrect();
};
