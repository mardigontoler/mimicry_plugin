
#pragma once

#include "MultiPhaseVocoder.h"


// Platform-specific definitions used for declaring an interface, independent of
// the SIMD instruction set.
#include "hwy/base.h"  // HWY_RESTRICT

namespace pitch_functions {

	struct PhaseCorrectArgs{
		PhaseCorrectArgs(float *oldInputPhases, float *oldOutputPhases, std::complex<float> *freqFftData, float * const omegas,
						 int synthesisHopSize, const PV::PvConstants &pvConstants) : oldInputPhases(oldInputPhases),
																					 oldOutputPhases(oldOutputPhases),
																					 freqFftData(freqFftData),
																					 omegas(omegas),
																					 synthesisHopSize(synthesisHopSize),
																					 pvConstants(pvConstants)
		{}

		float* oldInputPhases;
		float* oldOutputPhases;
		std::complex<float>* freqFftData;
		float* omegas;
		int synthesisHopSize;
		PV::PvConstants pvConstants;
	};

	HWY_DLLEXPORT void PhaseCorrectSIMD(PhaseCorrectArgs* HWY_RESTRICT args);


}  // namespace pitch
