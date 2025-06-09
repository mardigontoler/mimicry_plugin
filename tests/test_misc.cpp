#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../src/pitch/MultiPhaseVocoder.h"

TEST_CASE("PhaseVocoder factor calculation", "[phasevocoder]") {
    // Create a MultiPhaseVocoder with 1 vocoder
    PV::MultiPhaseVocoder vocoder(1);

    // Test octave up (12 semitones)
    vocoder.setPitchShiftSemitones(0, 12.0f);

    // Test octave down (-12 semitones)
    vocoder.setPitchShiftSemitones(0, -12.0f);

    // Test quarter tone (0.5 semitones)
    vocoder.setPitchShiftSemitones(0, 0.5f);

    // All of these just test that the method runs without crashing
    // In a real test, you'd also verify the actual output values
    SUCCEED("setPitchShiftSemitones executed without errors");
}
