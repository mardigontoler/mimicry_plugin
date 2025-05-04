
#pragma once

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

namespace PV
{
    static constexpr unsigned int fftOrder = 11u;
    static constexpr int FFT_SIZE = 1u << fftOrder;
    static constexpr int analysisOverlapFactor = 8;
	static constexpr size_t analysisHopSize = PV::FFT_SIZE / PV::analysisOverlapFactor;

	typedef juce::dsp::Complex<float> Complex_t;
}

class MultiPhaseVocoder {


public:

    explicit MultiPhaseVocoder(size_t numVocoders);

    ~MultiPhaseVocoder() = default;

    void pushSample(float sample) noexcept;

    float nextSample(size_t vocoderIx);

	[[maybe_unused]] [[maybe_unused]] [[nodiscard]] size_t getDelay() const;

    void setPitchShiftSemitones(size_t vocoderIx, float numSemitones);

protected:

    static constexpr float maxFactor = 2.0f;
//    static constexpr float minFactor = 0.5f;

    juce::dsp::FFT forwardFFT;
    juce::dsp::FFT inverseFFT;
    juce::dsp::WindowingFunction<float> window;

    std::array<float, PV::FFT_SIZE> fifo{};
	std::array<float, PV::FFT_SIZE> timeDomainRealTmp{};
    std::array<PV::Complex_t, PV::FFT_SIZE> timeDomainTmp{};
    std::array<PV::Complex_t, PV::FFT_SIZE> freqDomainTmp{};

	struct OutputSection {

		float factor = 1.0f;
		int synthesisHopSize = static_cast<int>(factor * static_cast<float>(PV::analysisHopSize));

		std::array<PV::Complex_t, PV::FFT_SIZE> freqFftData{}; // buffer for freq domain data
		std::array<PV::Complex_t, PV::FFT_SIZE> inverseFftOutput{}; // buffer for time domain data (complex)
		std::array<float, PV::FFT_SIZE> inverseFftRealOutput{}; // buffer for time domain data (complex)

		std::array<float, PV::FFT_SIZE> oldInputPhases{};
		std::array<float, PV::FFT_SIZE> oldOutputPhases{};
		std::array<float, (4 * PV::FFT_SIZE * static_cast<int>(maxFactor))> outputData{};

		double outputIndex = 0;
		size_t lastLeftIndex = 0;

	};

	std::vector<OutputSection> mOutputSections;

    // These stay constant for a particular FFT_SIZE. Represents how much to propgate the phase of each frequency bin
    std::array<float, PV::FFT_SIZE> omegas{};
	std::array<float, PV::FFT_SIZE> analysisHopSizeScaledOmegas{};

    bool outputReady = false;

    size_t fifoIndex = 0;
    size_t fifosWritten = 0;
    size_t fifoRead = 0;

	size_t mNumVocoders = 1;

	__declspec(noinline)void phaseCorrect(OutputSection& section) const;
    __declspec(noinline)void phaseCorrectSIMD(OutputSection& section) const;
};
