
#include <memory>

#include "DelayHeadController.h"
#include "gui/colors.h"

using namespace juce;


juce::String valToPercent(double v)
{
	return String(v * 100.0) + "%";
}

juce::String valToSignedIntStr(double v)
{
	String prefix{""};
	if (v > 0.0)
	{
		prefix = "+";
	}

	return prefix + String(roundToInt(v));
}


DelayHeadController::DelayHeadController(const juce::String& label)
	: semitonesKnob(valToSignedIntStr), feedbackKnob(valToPercent)
{
	constexpr int semitonesTextboxHeight = 15;

	indexLabel.setText(label, dontSendNotification);

	delayGainSlider.setSliderStyle(Slider::SliderStyle::LinearVertical);
	delayGainSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	// set up the semitones knob to have an editable textbox
	semitonesKnob.getSlider().setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	semitonesKnob.getSlider().setTextBoxStyle(Slider::TextBoxRight, false, 30, semitonesTextboxHeight);
	semitonesKnob.getLabel().setText("Pitch", dontSendNotification);

	feedbackKnob.getSlider().setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	feedbackKnob.getSlider().setTextBoxStyle(Slider::TextBoxRight, true, 30, semitonesTextboxHeight);
	feedbackKnob.getLabel().setText("Fdbk", dontSendNotification);

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
	semitoneAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, semitonesKnob.getSlider());
}


void
DelayHeadController::resetFeedbackAttachment(AudioProcessorValueTreeState &valueTreeState, String &param)
{
	feedbackAttachment = std::make_unique<SliderAttachment>(valueTreeState, param, feedbackKnob.getSlider());
}


void DelayHeadController::resized()
{
	auto area = getLocalBounds().toFloat();

	const auto labelArea = area.removeFromTop(indexLabel.getFont().getHeight() + 4);
	indexLabel.setBounds(labelArea.toNearestInt());

	const auto gainArea = area.removeFromLeft(area.getWidth() / 3.0f);
	delayGainSlider.setBounds(gainArea.toNearestInt());

	const auto potHeight = area.getHeight() / 2.0f;

	const auto feedbackArea = area.removeFromTop(potHeight);
	feedbackKnob.setBounds(feedbackArea.toNearestInt());

	semitonesKnob.setBounds(area.toNearestInt());
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
	g.fillRoundedRectangle(bounds, 5);
}
