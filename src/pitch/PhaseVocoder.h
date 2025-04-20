
#pragma once

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

class PhaseVocoder {


public:

    PhaseVocoder();

    ~PhaseVocoder() = default;

    void pushSample(float sample) noexcept;

    float nextSample();

    [[nodiscard]] int getDelay() const;

    void setPitchShiftSemitones(float numSemitones);

private:
    static constexpr unsigned int fftOrder = 11u;
    static constexpr int N = 1u << fftOrder; // fft size
    static constexpr float tau = 2 * juce::MathConstants<float>::pi;
    static constexpr int analysisOverlapFactor = 8;
    int analysisHopSize = N / analysisOverlapFactor;
    float factor = 1.0f;
    int synthesisHopSize = static_cast<int>(factor * static_cast<float>(analysisHopSize));

    static constexpr float maxFactor = 2.0f;
    static constexpr float minFactor = 0.5f;


    juce::dsp::FFT forwardFFT;
    juce::dsp::FFT inverseFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, N> fifo{};
    std::array<float, N> tmp{}; // misc temporary real data
    std::array<juce::dsp::Complex<float>, N> timeFftData; // buffer for time domain data (complex)
    std::array<juce::dsp::Complex<float>, N> freqFftData; // buffer for freq domain data

    std::array<float, N> oldInputPhases{};
    std::array<float, N> oldOutputPhases{};
    std::array<float, (4*N*static_cast<int>(maxFactor))> outputData{};

    // These stays constant for a particular value of N. Represents how much to propgate the phase of each frequency bin
    std::array<float, N> omegas{};

    bool outputReady = false;

    int fifoIndex = 0;
    int fifosWritten = 0;
    int fifoRead = 0;
    int lastLeftIndex = 0;
    double outputIndex = 0;

    void phaseCorrect();
};
