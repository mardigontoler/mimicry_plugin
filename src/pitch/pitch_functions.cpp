
#include "pitch_functions.h"

#include "hwy/base.h"

// First undef to prevent error when re-included.
#undef HWY_TARGET_INCLUDE
// For dynamic dispatch, specify the name of the current file (unfortunately
// __FILE__ is not reliable) so that foreach_target.h can re-include it.
#define HWY_TARGET_INCLUDE "pitch_functions.cpp" // NOLINT(cppcoreguidelines-macro-usage)

// Generates code for each enabled target by re-including this source file.

#include "hwy/foreach_target.h"  // IWYU pragma: keep

// Must come after foreach_target.h to avoid redefinition errors.
#include "hwy/aligned_allocator.h"
#include "hwy/highway.h"

#include "hwy/contrib/math/math-inl.h"


HWY_BEFORE_NAMESPACE();  // NOLINT(cppcoreguidelines-macro-usage)

namespace pitch_functions // NOLINT(*-concat-nested-namespaces)
{
namespace HWY_NAMESPACE
{
	constexpr float kTwoPI = 2.0f * 3.14159265358979323846f;
	constexpr float kPI = 3.14159265358979323846f;
	constexpr float kOneOverTwoPI = 1.0f / kTwoPI;


	template <class D, class V>
	HWY_ATTR V NormalizeAngle(D d, V x) // NOLINT
	{
		const auto v_two_pi = Set(d, kTwoPI);
		const auto v_pi = Set(d, kPI);
		const auto v_one_over_two_pi = Set(d, kOneOverTwoPI);

		// x = x - 2π * round(x / 2π)
		auto quotient = Mul(x, v_one_over_two_pi);
		quotient = Round(quotient);
		x = Sub(x, Mul(quotient, v_two_pi));

		// Ensure result is in [-π, π]
		auto too_high = Gt(x, v_pi);
		auto too_low = Lt(x, Neg(v_pi));
		x = IfThenElse(too_high, Sub(x, v_two_pi), x);
		x = IfThenElse(too_low, Add(x, v_two_pi), x);

		return x;
	}


	template <class D, class V>
	HWY_MAYBE_UNUSED V CosApprox(D d, V x)
	{
		// taylor series approx for cosine.
		// 1 - x^2/2! + x^4/4! - x^6/6! ...

		x = NormalizeAngle(d, x);

		const auto x2 = Mul(x, x);
		const auto x4 = Mul(x2, x2);
		const auto x6 = Mul(x4, x2);
		const auto x8 = Mul(x6, x2);
		const auto x10 = Mul(x8, x2);

		auto v_1 = Set(d, 1.0f);
		auto v_2 = Set(d, 2.0f);
		auto v_24 = Set(d, 24.0f);
		auto v_720 = Set(d, 720.0f);
		auto v_40320 = Set(d, 40320.0f);
		auto v_3628800 = Set(d, 3628800);

		auto result = Sub(v_1, Div(x2, v_2));
		result = Add(result, Div(x4, v_24));
		result = Sub(result, Div(x6, v_720));
		result = Add(result, Div(x8, v_40320));
		result = Sub(result, Div(x10, v_3628800));

		return result;
	}

	template <class D, class V>
	HWY_MAYBE_UNUSED V SinApprox(D d, V x)
	{
		// Taylor series approx for sine.
		// x - x^3/3! + x^5/5! - x^7/7! ...

		x = NormalizeAngle(d, x);

		const auto x2 = Mul(x, x);
		const auto x3 = Mul(x2, x);
		const auto x5 = Mul(x3, x2);
		const auto x7 = Mul(x5, x2);
		const auto x9 = Mul(x7, x2);
		const auto x11 = Mul(x9, x2);

		auto v_6 = Set(d, 6.0f);
		auto v_120 = Set(d, 120.0f);
		auto v_5040 = Set(d, 5040.0f);
		auto v_362880 = Set(d, 362880.0f);
		auto v_39916800 = Set(d, 39916800.0f);

		auto result = Sub(x, Div(x3, v_6));
		result = Add(result, Div(x5, v_120));
		result = Sub(result, Div(x7, v_5040));
		result = Add(result, Div(x9, v_362880));
		result = Sub(result, Div(x11, v_39916800));

		return result;
	}

	HWY_ATTR_NO_MSAN void HwyPhaseCorrect(PhaseCorrectArgs *  HWY_RESTRICT args)
	{

		constexpr float tau = 2 * juce::MathConstants<float>::pi;

		auto& fftSize = PV::PvConstants::FFT_SIZE;

		auto& section = args->mOutputSection;

		constexpr auto analysisHopSizeF = static_cast<float>(PV::PvConstants::analysisHopSize);
		const auto synthesisHopSizeF = static_cast<float>(section->synthesisHopSize);

		// Define SIMD types
		const hwy::HWY_NAMESPACE::ScalableTag<float> d;  // largest possible vector
		const size_t N = Lanes(d);

		// split complex fft bins into real and imaginary parts for processing
		for (size_t i = 0; i < fftSize; ++i)
		{
			auto cpxFreqSample = section->freqFftData[i];

			// highway has an Atan2 function that can vectorize this, but it seems to be innacurrate enough to sound very bad.
			// sticking with std::arg for now ...
			section->freqFftArgs[i] = std::arg(cpxFreqSample);

			section->freqFftReal[i] = cpxFreqSample.real();
			section->freqFftImag[i] = cpxFreqSample.imag();
		}

		// Constants in vectors
		const auto v_tau = Set(d, tau);
		const auto v_analysis_hop = Set(d, analysisHopSizeF);
		const auto v_synthesis_hop = Set(d, synthesisHopSizeF);

		for(size_t i = 0 ; i < fftSize; i += N) {

			auto v_input_phase = Load(d, section->freqFftArgs.data() + i);
			auto v_omega = Load(d, args->mOmegas + i);
			auto v_old_input_phase = Load(d, section->oldInputPhases.data() + i);

			auto v_hop_omega = Mul(v_analysis_hop, v_omega);
			auto v_delta = Sub(v_input_phase, v_old_input_phase);
			v_delta = Sub(v_delta, v_hop_omega);

			auto v_div = Div(v_delta, v_tau);
			auto v_round = Round(v_div);
			v_delta = Sub(v_delta, Mul(v_tau, v_round));

			// store input phase into old input phase
			Store(v_input_phase, d, section->oldInputPhases.data() + i);

			auto v_inst_freq = Add(v_omega, Div(v_delta, v_analysis_hop));

			auto v_old_output_phase = Load(d, section->oldOutputPhases.data() + i);
			auto v_output_phase = Add(v_old_output_phase, Mul(v_synthesis_hop, v_inst_freq));

			// normalize
			v_div = Div(v_output_phase, v_tau);
			v_round = Round(v_div);
			v_output_phase = Sub(v_output_phase, Mul(v_tau, v_round));

			Store(v_output_phase, d, section->oldOutputPhases.data() + i);

			// Now perform complex multiplication to rotate the dft values so they match the desired output phases.
			// Equivalent to args->freqFftData[i] *= std::polar(1.0f, outputPhase - inputPhase);
			// Each bin is

			auto v_phase_diff = Sub(v_output_phase, v_input_phase);
			// std::polar(1.0f, phase_diff) is e^(i*phase_diff) = cos(phase_diff) + i * sin(phase_diff)
			auto v_real = Load(d, section->freqFftReal.data() + i);
			auto v_imag = Load(d, section->freqFftImag.data() + i);
			auto v_cos_diff = CosApprox(d, v_phase_diff);  // CosApprox(d, v_phase_diff);
			auto v_sin_diff = SinApprox(d, v_phase_diff);  // SinApprox(d, v_phase_diff);

			// complex multiplication: (a + bi)*(c + di) = ac - bd + i(ad + bc),
			// so (v_real + i v_imag) * (v_cos_diff + i v_sin_diff)
			// = (v_real * v_cos_diff) - ( v_imag * v_sin_diff ) + i ( (v_real * v_sin_diff) + (v_imag * v_cos_diff) )
			auto v_rotated_real = Sub(Mul(v_real, v_cos_diff), Mul(v_imag, v_sin_diff));
			auto v_rotated_imag = Add(Mul(v_real, v_sin_diff), Mul(v_imag, v_cos_diff));

			Store(v_rotated_real, d, section->freqFftReal.data() + i);
			Store(v_rotated_imag, d, section->freqFftImag.data() + i);
		}

		// recombine real and imaginary parts of rotated bins back into complex numbers for final output
		for (size_t i = 0; i < fftSize; ++i)
		{
			section->freqFftData[i] = {section->freqFftReal[i], section->freqFftImag[i]};
		}
	}


	// NOLINTNEXTLINE(google-readability-namespace-comments)
	}  // namespace HWY_NAMESPACE
}  // namespace


HWY_AFTER_NAMESPACE();  // NOLINT(cppcoreguidelines-macro-usage)


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


