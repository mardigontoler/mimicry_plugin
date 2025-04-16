/*
  ==============================================================================
    This component is a slider with a knob below it, meant to control the
    gain of a delay line output as well as the amount of pitch shift on
    the delay head.
  ==============================================================================
*/

#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

using namespace juce;
typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;


class DelayHeadController : public Component {
public:
    DelayHeadController() {
        delayGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
        delayGainSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);


        // set up the semitones knob to have an editable textbox
        semitonesKnob.setSliderStyle(Slider::SliderStyle::Rotary);
        semitonesKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 100, semitonesTextboxHeight);

        addAndMakeVisible(&delayGainSlider);
        addAndMakeVisible(&semitonesKnob);

    }



    void resetDelayGainAttachment(AudioProcessorValueTreeState& valueTreeState, String& param){
        delayGainAttachment.reset(new SliderAttachment(valueTreeState, param, delayGainSlider));
    }

    void resetSemitoneAttachment(AudioProcessorValueTreeState& valueTreeState, String& param){
        semitoneAttachment.reset(new SliderAttachment(valueTreeState, param, semitonesKnob));
    }

    void resized() override {
        Rectangle<int> area = getLocalBounds();

        int height = area.getHeight();

        // delay slider on the top
        delayGainSlider.setBounds(area.removeFromTop(height - semitonesKnobSpace).reduced(0, 3));

        // semitones knob below it
        semitonesKnob.setBounds(area);
    }


private:

    int semitonesKnobSpace = 65;
    int semitonesTextboxHeight = 15;
    Slider delayGainSlider;
    Slider semitonesKnob;


    std::unique_ptr<SliderAttachment> delayGainAttachment;
    std::unique_ptr<SliderAttachment> semitoneAttachment;


};