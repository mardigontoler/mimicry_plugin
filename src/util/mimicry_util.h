
#pragma once

namespace mimicry_util
{

    /**
    Approximate number of samples per note value of the given beat divider at the given sample rate.
    For example, at fs=44.1khz and 120bpm, an eight note (beatDivider = 2)
    would last for 
    */
    inline size_t
    getSamplesPerSubdivision(const double beatsPerMinute, const double sampleRate, const float beatDivider)
    {
        const double beatsPerSecond = beatsPerMinute / 60.0f;
        const double samplesPerBeat = sampleRate / beatsPerSecond;
        const double samplesPerInterval = samplesPerBeat * beatDivider;
        return static_cast<size_t>(samplesPerInterval);
    }

}
