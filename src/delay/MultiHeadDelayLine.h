#pragma once

#include "juce_core/juce_core.h"


template<class T>
class MultiHeadDelayLine
{

public:

    explicit MultiHeadDelayLine(const size_t numDelays)
            :
            gains(numDelays, 0),
            targetReadHeads(numDelays, 0),
            smoothedReadHeads(numDelays, 0),
            deltaSmoothReads(numDelays, 1),
            currentDelays(numDelays, 0),
            remainingStepsAtAlteredRate(numDelays, 0),

            initialDelaysSet(numDelays, false)
    {
    }

    ~MultiHeadDelayLine() = default;


    void resize(const size_t _size)
    {
        size = _size;
        delayLineData.resize(size);
    }


    T getNextDelayedSample(const size_t headIndex)
    {
        if (currentDelays[headIndex] == 0)
        {
            return delayLineData[juce::negativeAwareModulo(writeHead - 1, delayLineData.size())] * gains[headIndex];
        }
        smoothedReadHeads[headIndex] += deltaSmoothReads[headIndex];
        while (smoothedReadHeads[headIndex] >= size)
        {
            smoothedReadHeads[headIndex] -= size;
        }
        while (smoothedReadHeads[headIndex] < 0)
        {
            smoothedReadHeads[headIndex] += size;
        }
        targetReadHeads[headIndex] = (targetReadHeads[headIndex] + 1) % size;

        if (deltaSmoothReads[headIndex] != 1.0f)
        {
            remainingStepsAtAlteredRate[headIndex] -= 1;
            if (remainingStepsAtAlteredRate[headIndex] <= 1)
            {
                deltaSmoothReads[headIndex] = 1.0f;
                smoothedReadHeads[headIndex] = targetReadHeads[headIndex];
            }
        }


        return delayLineData[smoothedReadHeads[headIndex]] * gains[headIndex];
    }


    void pushNextSample(T sample)
    {
        delayLineData[writeHead] = sample;
        // increment write head, wrapping around if we went past the end
        writeHead = (writeHead + 1) % size;
    }


    // set the delay size (in samples) for read head i
    void setNumDelaySamples(const size_t i, const size_t numSamples, const size_t sampleRate)
    {
        const float delaySizeChange = currentDelays[i] - numSamples;
        currentDelays[i] = numSamples;

        setTargetDelay(i, numSamples);

        if (numSamples == 0)
        {

        }

        // auto tol = juce::Tolerance<float>{};

        if (initialDelaysSet[i] && juce::approximatelyEqual(delaySizeChange, 0.0f))
        {
            // Only set a delta if the delay had already been set before.
            // Set the delta for this head so that it will be at the target within .5 seconds.
            //float distanceToNewTarget = smoothedReadHeads[i] - targetReadHeads[i];
            const auto numSamplesToGetToTarget = static_cast<size_t> ( static_cast<float>(sampleRate) * 0.5f );
            remainingStepsAtAlteredRate[i] = numSamplesToGetToTarget;

            // calculate where the target will be by the time we want to meet it
            const float projectedTargetReadHead = (numSamplesToGetToTarget + targetReadHeads[i]) % size;

            // how fast should we move the smoothed read head so that it will be at the projected target once we're done moving it?
            // the current read head position might technically be past the projected target, so we "unwrap" it onto a noncircular axis first
            float currentReadHead = smoothedReadHeads[i];
            while (currentReadHead > projectedTargetReadHead)
            {
                currentReadHead -= size;
            }
            // then set the delta
            deltaSmoothReads[i] =
                    (projectedTargetReadHead - smoothedReadHeads[i]) / static_cast<float>(numSamplesToGetToTarget);


        }
        initialDelaysSet[i] = true;
    }


    void clear()
    {
        std::fill(delayLineData.begin(), delayLineData.end(), 0);
    }

    [[nodiscard]] size_t getNumHeads() const
    { return targetReadHeads.size(); }

    void setGain(const size_t i, const float gain)
    {
        gains[i] = gain;
    }


private:

    void setTargetDelay(const size_t i, const size_t numSamples)
    {
        // set the target read head behind the write head
        if (numSamples > writeHead)
        {
            targetReadHeads[i] = size - (numSamples - writeHead);
        } else
        {
            targetReadHeads[i] = writeHead - numSamples;
        }
    }


    // data will be used as a circular buffer.
    std::vector<T> delayLineData;

    // index that the next write operation will occur
    size_t writeHead = 0;

    std::vector<float> gains;

    std::vector<size_t> targetReadHeads;
    size_t size = 0;
    std::vector<float> smoothedReadHeads;
    std::vector<float> deltaSmoothReads;
    std::vector<size_t> currentDelays; // the currently set delay in samples for each read head
    std::vector<size_t> remainingStepsAtAlteredRate; // remember how much longer we need to advance by delta != 1

    std::vector<bool> initialDelaysSet;

};
