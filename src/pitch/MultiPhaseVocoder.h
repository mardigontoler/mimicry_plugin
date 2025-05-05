
#pragma once

#include "juce_core/juce_core.h"
#include "juce_dsp/juce_dsp.h"

#include "hwy/aligned_allocator.h"


namespace PV
{
	struct PvConstants {
		static constexpr unsigned int fftOrder = 11u;
		static constexpr int FFT_SIZE = 1u << fftOrder;
		static constexpr int analysisOverlapFactor = 8;
		static constexpr size_t analysisHopSize = FFT_SIZE / analysisOverlapFactor;

	};

	typedef std::complex<float> Complex_t;


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

		std::vector<float, hwy::AlignedAllocator<float>> fifo;
		std::vector<float, hwy::AlignedAllocator<float>> timeDomainRealTmp;
		std::vector<Complex_t, hwy::AlignedAllocator<Complex_t>>  timeDomainTmp;
		std::vector<Complex_t, hwy::AlignedAllocator<Complex_t>>  freqDomainTmp;

		struct OutputSection {

			OutputSection();

			float factor = 1.0f;
			int synthesisHopSize = static_cast<int>(factor * static_cast<float>(PvConstants::analysisHopSize));

			std::vector<Complex_t, hwy::AlignedAllocator<Complex_t>> freqFftData; // buffer for freq domain data
			std::vector<Complex_t, hwy::AlignedAllocator<Complex_t>> inverseFftOutput; // buffer for time domain data (complex)

			std::vector<float, hwy::AlignedAllocator<float>> inverseFftRealOutput; // buffer for time domain data (complex)

			std::vector<float, hwy::AlignedAllocator<float>> oldInputPhases;
			std::vector<float, hwy::AlignedAllocator<float>> oldOutputPhases;
			std::vector<float, hwy::AlignedAllocator<float>> outputData;

			double outputIndex = 0;
			size_t lastLeftIndex = 0;

		};

		std::vector<OutputSection> mOutputSections;

		// These stay constant for a particular FFT_SIZE. Represents how much to propgate the phase of each frequency bin
		hwy::AlignedFreeUniquePtr<float[]> omegas;
		hwy::AlignedFreeUniquePtr<float[]> analysisHopSizeScaledOmegas;

		bool outputReady = false;

		size_t fifoIndex = 0;
		size_t fifosWritten = 0;
		size_t fifoRead = 0;

		size_t mNumVocoders = 1;

		void phaseCorrect(OutputSection &section);

		void phaseCorrectSIMD(OutputSection &section);
	};
}
