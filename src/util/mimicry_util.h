
#pragma once

namespace mimicry_util {

    /**
    Approximate number of samples per note value of the given beat divider at the given sample rate.
    For example, at fs=44.1khz and 120bpm, an eight note (beatDivider = 2)
    would last for 
    */
    long getSamplesPerSubdivision(double beatsPerMinute, double sampleRate, float beatDivider) {
        double beatsPerSecond = beatsPerMinute / 60.0f;
        double samplesPerBeat = sampleRate / beatsPerSecond;
        double samplesPerInterval = samplesPerBeat * beatDivider;
        return (long)samplesPerInterval;
    }



}
