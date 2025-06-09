#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "../src/pitch/MultiPhaseVocoder.h"

TEST_CASE("PhaseVocoder initialization", "[phasevocoder]") {
    // Create a MultiPhaseVocoder with 2 vocoders
    PV::MultiPhaseVocoder vocoder(2);

    // Basic test to ensure it initializes without crashing
    REQUIRE_NOTHROW(vocoder.setPitchShiftSemitones(0, 0.0f));
    REQUIRE_NOTHROW(vocoder.setPitchShiftSemitones(1, 0.0f));
}

TEST_CASE("PhaseVocoder pitch shifting", "[phasevocoder]") {
    // Create a MultiPhaseVocoder with 1 vocoder
    PV::MultiPhaseVocoder vocoder(1);

    // Set pitch shift to no change (0 semitones)
    vocoder.setPitchShiftSemitones(0, 0.0f);

    // Push a simple sine wave sample
    float inputSample = 1.0f;
    REQUIRE_NOTHROW(vocoder.pushSample(inputSample));

    // Get the next sample (this won't match the input yet as we need to fill the buffer)
    // This is just to test that the method doesn't crash
    REQUIRE_NOTHROW(vocoder.nextSample(0));
}
