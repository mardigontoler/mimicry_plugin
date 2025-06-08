
#pragma once

#include "MultiPhaseVocoder.h"


// Platform-specific definitions used for declaring an interface, independent of
// the SIMD instruction set.
#include "hwy/base.h"  // HWY_RESTRICT

namespace pitch_functions
{

    struct PhaseCorrectArgs
    {

        PhaseCorrectArgs(PV::MultiPhaseVocoder::OutputSection* outputsection, float* omegas,
                         const PV::PvConstants& pvConstants)
                : mOutputSection(outputsection),
                  mOmegas(omegas),
                  mPvConstants(pvConstants)
        {}

        PV::MultiPhaseVocoder::OutputSection* mOutputSection;
        float* mOmegas;
        PV::PvConstants mPvConstants;
    };

    HWY_DLLEXPORT void PhaseCorrectSIMD(const PhaseCorrectArgs* HWY_RESTRICT args);

}
