
#include <memory>

#include "DelayHeadController.h"

using namespace juce;

DelayHeadController::DelayHeadController()
{
	delayGainSlider.setSliderStyle(Slider::SliderStyle::Rotary);
	delayGainSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	// set up the semitones knob to have an editable textbox
	semitonesKnob.setSliderStyle(Slider::SliderStyle::Rotary);
	semitonesKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 100, semitonesTextboxHeight);

	feedbackKnob.setSliderStyle(Slider::SliderStyle::Rotary);
	feedbackKnob.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	addAndMakeVisible(&delayGainSlider);
	addAndMakeVisible(&semitonesKnob);
	addAndMakeVisible(&feedbackKnob);
}


void DelayHeadController::resetDelayGainAttachment(
		AudioProcessorValueTreeState &valueTreeState,
		String &param
)
{
	delayGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, delayGainSlider);
}


void
DelayHeadController::resetSemitoneAttachment(AudioProcessorValueTreeState &valueTreeState, String &param)
{
	semitoneAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, semitonesKnob);
}


void
DelayHeadController::resetFeedbackAttachment(AudioProcessorValueTreeState &valueTreeState, String &param)
{
	feedbackAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, feedbackKnob);
}


void DelayHeadController::resized()
{
	Rectangle<int> area = getLocalBounds();

	int height = area.getHeight();
	int width = area.getWidth();

	auto potHeight = height / 3;

	auto gainArea = area.removeFromTop(potHeight);
	delayGainSlider.setBounds(gainArea);

	auto feedbackArea = area.removeFromTop(potHeight);
	feedbackKnob.setBounds(feedbackArea);

	semitonesKnob.setBounds(area);
}


void DelayHeadController::paint(Graphics &g)
{
	Component::paint(g);

	g.setColour(getLookAndFeel().findColour(juce::Label::backgroundColourId));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 1);
}
