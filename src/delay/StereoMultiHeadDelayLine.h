/*

#pragma once

#include "MultiHeadDelayLine.h"



template <class T>
struct stereo_samples {
    T left;
    T right;
};


template <class T>
class StereoMultiHeadDelayLine {

private:

    std::vector<float> leftGains;
    std::vector<float> rightGains;

    size_t numHeads;


public:


    StereoMultiHeadDelayLine(size_t numHeads)
    :
        leftDelayLine(numHeads),
        rightDelayLine(numHeads),
        leftGains(numHeads, 1.0f),
        rightGains(numHeads, 1.0f),
        numHeads(numHeads)
    {

    }

    ~StereoMultiHeadDelayLine() {

    }

    MultiHeadDelayLine<T> leftDelayLine;
    MultiHeadDelayLine<T> rightDelayLine;

    void resize(size_t maxSamples) {
        leftDelayLine.resize(maxSamples);
        rightDelayLine.resize(maxSamples);
    }

    void clear() {
        leftDelayLine.clear();
        rightDelayLine.clear();
    }

    //void pushNextStereoSamples(stereo_samples samples) {
    //    pushSampleToChannel(0, samples.left);
    //    pushSampleToChannel(1, samples.right);
    //}

    void pushSampleToChannel(int channel, T sample) {
        jassert(channel == 0 || channel == 1);
        if (channel == 0) {
            leftDelayLine.pushNextSample(sample);
        }
        else if (channel == 1) {
            rightDelayLine.pushNextSample(sample);
        }
    }

    //stereo_samples getNextStereoDelayedSamples() {
    //    stereo_samples samples;
    //    samples.left = getNextSampleFromChannel(0);
    //    samples.right = getNextSampleFromChannel(1);
    //    return samples;
    //}

    T getNextSample(size_t headIndex, int channel) {
        jassert(channel == 0 || channel == 1);
        jassert(headIndex < numHeads);
        if (channel == 0)
            return leftGains[headIndex] * leftDelayLine.getNextDelayedSample(headIndex);
        else if (channel == 1) {
            return rightGains[headIndex] * rightDelayLine.getNextDelayedSample(headIndex);
        }
    }



    void setNumDelaySamples(size_t headIndex, size_t numSamples, double sampleRate) {
        jassert(headIndex < numHeads);
        leftDelayLine.setNumDelaySamples(headIndex, numSamples, sampleRate);
        rightDelayLine.setNumDelaySamples(headIndex, numSamples, sampleRate);
    }

    void setGain(size_t i, int channel, float gain) {
        jassert(i < numHeads);
        jassert(channel == 0 || channel == 1);
        if (channel == 0)leftGains[i] = gain;
        else if (channel == 1)rightGains[i] = gain;
    }


    size_t getNumHeads() { return numHeads; }

};
*/
