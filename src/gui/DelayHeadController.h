/*
  ==============================================================================
    This component is a slider with a knob below it, meant to control the
    gain of a delay line output as well as the amount of pitch shift on
    the delay head.
  ==============================================================================
*/

#pragma once

#include <memory>

#include "juce_audio_processors/juce_audio_processors.h"

#include "LabeledSlider.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;


class DelayHeadController : public juce::Component
{
public:
    explicit DelayHeadController(const juce::String& label);

    void resetDelayGainAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param);

    void resetSemitoneAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param);

    void resetFeedbackAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param);

    void resized() override;

    void paint(juce::Graphics& g) override;


private:

    juce::Slider delayGainSlider;
    LabeledSlider semitonesKnob;
    LabeledSlider feedbackKnob;
    juce::Label indexLabel;

    std::unique_ptr<SliderAttachment> delayGainAttachment;
    std::unique_ptr<SliderAttachment> semitoneAttachment;
    std::unique_ptr<SliderAttachment> feedbackAttachment;


};
