
#include <memory>

#include "DelayHeadController.h"


DelayHeadController::DelayHeadController()
{
  delayGainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
  delayGainSlider.setTextBoxStyle(juce::Slider::NoTextBox, true, 0, 0);


  // set up the semitones knob to have an editable textbox
  semitonesKnob.setSliderStyle(juce::Slider::SliderStyle::Rotary);
  semitonesKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, semitonesTextboxHeight);

  addAndMakeVisible(&delayGainSlider);
  addAndMakeVisible(&semitonesKnob);

}


void DelayHeadController::resetDelayGainAttachment(
  juce::AudioProcessorValueTreeState&valueTreeState,
  juce::String& param
  )
{
  delayGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, delayGainSlider);
}
