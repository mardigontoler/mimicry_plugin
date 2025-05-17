
#include <memory>

#include "DelayHeadController.h"
#include "gui/colors.h"

using namespace juce;



DelayHeadController::DelayHeadController(juce::String label)
{
	constexpr int semitonesTextboxHeight = 15;

	indexLabel.setText(label, dontSendNotification);

	delayGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	delayGainSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	// set up the semitones knob to have an editable textbox
	semitonesKnob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	semitonesKnob.setTextBoxStyle(Slider::TextBoxRight, false, 100, semitonesTextboxHeight);

	feedbackKnob.setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);;
	feedbackKnob.setTextBoxStyle(Slider::TextBoxRight, true, 0, 0);

	addAndMakeVisible(&indexLabel);
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

	const auto labelArea = area.removeFromTop(indexLabel.getFont().getHeight() + 4);
	indexLabel.setBounds(labelArea);

	const auto gainArea = area.removeFromLeft(area.getWidth() / 3);
	delayGainSlider.setBounds(gainArea);

	const auto potHeight = area.getHeight() / 2;

	const auto feedbackArea = area.removeFromTop(potHeight);
	feedbackKnob.setBounds(feedbackArea);

	semitonesKnob.setBounds(area);
}


void DelayHeadController::paint(Graphics &g)
{
	Component::paint(g);

	auto bounds = getLocalBounds().toFloat();

	// Base gradient
	juce::ColourGradient baseGradient(
			juce::Colour(mimicry::Colors::getDelayControllerBgGradient1()),
			bounds.getTopLeft(),
			juce::Colour(mimicry::Colors::getDelayControllerBgGradient2()),
			bounds.getBottomRight(),
			false
	);

	baseGradient.addColour(0.25f, mimicry::Colors::getBgGradientCol2());
	baseGradient.addColour(0.75f, mimicry::Colors::getBgGradientCol2());

	g.setGradientFill(baseGradient);
	g.fillRect(bounds);

}
