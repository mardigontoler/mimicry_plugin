
#pragma once

namespace mimicry_util
{

    /*!
    Returns the approximate number of samples per note value of the given beat divider at the given sample rate.
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
