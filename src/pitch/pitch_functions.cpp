
#include "pitch_functions.h"

#include <cstdio>
#include <cstdlib>  // abort

#include <cmath>  // std::abs
#include <memory>
#include <numeric>  // std::iota, std::inner_product

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma clang diagnostic push
#pragma ide diagnostic ignored "modernize-concat-nested-namespaces"

// First undef to prevent error when re-included.
#undef HWY_TARGET_INCLUDE
// For dynamic dispatch, specify the name of the current file (unfortunately
// __FILE__ is not reliable) so that foreach_target.h can re-include it.
#define HWY_TARGET_INCLUDE "pitch_functions.cpp"
// Generates code for each enabled target by re-including this source file.


#include "hwy/foreach_target.h"  // IWYU pragma: keep

// Must come after foreach_target.h to avoid redefinition errors.
#include "hwy/aligned_allocator.h"
#include "hwy/highway.h"
#include "hwy/nanobenchmark.h"



HWY_BEFORE_NAMESPACE();

namespace pitch_functions // NOLINT(*-concat-nested-namespaces)
{
namespace HWY_NAMESPACE
{

HWY_ATTR_NO_MSAN void HwyPhaseCorrect(PhaseCorrectArgs *  HWY_RESTRICT args)
{
	namespace hn = hwy::HWY_NAMESPACE;

	constexpr float tau = 2 * juce::MathConstants<float>::pi;

	auto& fftSize = PV::PvConstants::FFT_SIZE;
	auto& analysisHopSize = PV::PvConstants::analysisHopSize;
	const auto synthesisHopSizeF = static_cast<float>(args->synthesisHopSize);

	// Define SIMD types
	const hn::ScalableTag<float> d;  // largest possible vector

//	const hn::ScalableTag<float> d;
//	const size_t N = Lanes(d);

	for(size_t i = 0 ; i < fftSize; i++) {

		const float inputPhase = std::arg(args->freqFftData[i]); // std::atan2(std::imag(z), std::real(z))
		const float omega = args->omegas[i];

		float deltaInputPhase = inputPhase - args->oldInputPhases[i] - (static_cast<float>(analysisHopSize) * omega);
		deltaInputPhase = deltaInputPhase - tau * std::round(deltaInputPhase / tau);
		args->oldInputPhases[i] = inputPhase;
		const float instantaneousFrequency = omega + (deltaInputPhase / static_cast<float>(analysisHopSize));

		float outputPhase = args->oldOutputPhases[i] + (static_cast<float>(args->synthesisHopSize) * instantaneousFrequency);
		outputPhase -= tau * std::round(outputPhase / tau);

		args->oldOutputPhases[i] = outputPhase;

		// complex multiplication to rotate the fft values so that they match the target output phases
		args->freqFftData[i] *= std::polar(1.0f, outputPhase - inputPhase);
	}

}


// NOLINTNEXTLINE(google-readability-namespace-comments)
}  // namespace HWY_NAMESPACE
}  // namespace


HWY_AFTER_NAMESPACE();

// The table of pointers to the various implementations in HWY_NAMESPACE must
// be compiled only once (foreach_target #includes this file multiple times).
// HWY_ONCE is true for only one of these 'compilation passes'.
#if HWY_ONCE

namespace pitch_functions
{
	// This macro declares a static array used for dynamic dispatch; it resides in
	// the same outer namespace that contains FloorLog2.
	HWY_EXPORT(HwyPhaseCorrect);


	HWY_DLLEXPORT void PhaseCorrectSIMD(PhaseCorrectArgs *  HWY_RESTRICT args)
	{
		const auto ptr = HWY_DYNAMIC_POINTER(HwyPhaseCorrect);
		return ptr(args);
	}
}

#endif // HWY_ONCE
