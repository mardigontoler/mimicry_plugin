
#pragma once

#include "MultiPhaseVocoder.h"


// Platform-specific definitions used for declaring an interface, independent of
// the SIMD instruction set.
#include "hwy/base.h"  // HWY_RESTRICT

namespace pitch_functions {

	struct PhaseCorrectArgs{

		PhaseCorrectArgs(PV::MultiPhaseVocoder::OutputSection *outputsection, float *omegas,
						 const PV::PvConstants &pvConstants)
						 : outputSection(outputsection),
						 omegas(omegas),
						 pvConstants(pvConstants)
		{}

		PV::MultiPhaseVocoder::OutputSection* outputSection;
		float* omegas;
		PV::PvConstants pvConstants;
	};

	HWY_DLLEXPORT void PhaseCorrectSIMD(PhaseCorrectArgs* HWY_RESTRICT args);


}  // namespace pitch
