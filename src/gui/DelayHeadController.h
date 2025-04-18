/*
  ==============================================================================
    This component is a slider with a knob below it, meant to control the
    gain of a delay line output as well as the amount of pitch shift on
    the delay head.
  ==============================================================================
*/

#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;


class DelayHeadController : public juce::Component {
public:
    DelayHeadController();

    void resetDelayGainAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param);

    void resetSemitoneAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param){
        semitoneAttachment.reset(new SliderAttachment(valueTreeState, param, semitonesKnob));
    }

    void resized() override {
        juce::Rectangle<int> area = getLocalBounds();

        int height = area.getHeight();

        // delay slider on the top
        delayGainSlider.setBounds(area.removeFromTop(height - semitonesKnobSpace).reduced(0, 3));

        // semitones knob below it
        semitonesKnob.setBounds(area);
    }


private:

    int semitonesKnobSpace = 65;
    int semitonesTextboxHeight = 15;
    juce::Slider delayGainSlider;
    juce::Slider semitonesKnob;

    std::unique_ptr<SliderAttachment> delayGainAttachment;
    std::unique_ptr<SliderAttachment> semitoneAttachment;


};
