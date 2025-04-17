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
    DelayHeadController() {
        delayGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        delayGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);


        // set up the semitones knob to have an editable textbox
        semitonesKnob.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        semitonesKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, semitonesTextboxHeight);

        addAndMakeVisible(&delayGainSlider);
        addAndMakeVisible(&semitonesKnob);

    }



    void resetDelayGainAttachment(juce::AudioProcessorValueTreeState& valueTreeState, juce::String& param){
        delayGainAttachment.reset(new SliderAttachment(valueTreeState, param, delayGainSlider));
    }

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
