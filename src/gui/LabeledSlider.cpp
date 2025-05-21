
#include "LabeledSlider.h"

#include <utility>

LabeledSlider::LabeledSlider(std::function<juce::String (double)> strValFunc)
	: knob(std::move(strValFunc))
{
	// Setup label
	label.setText("", juce::dontSendNotification);
	label.setJustificationType(juce::Justification::centred);
	addAndMakeVisible(label);

	// Setup knob
	knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
	knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
	knob.setDoubleClickReturnValue(true, 0.0);
	addAndMakeVisible(knob);
}

void LabeledSlider::resized()
{
	juce::GlyphArrangement glyphs;
	glyphs.addLineOfText(label.getFont(), label.getText(), 0.0f, 0.0f);
	auto labelHeight = label.getFont().getHeight() + 4;

	auto bounds = getLocalBounds().toFloat();
	label.setBounds(bounds.removeFromTop(labelHeight).toNearestInt());
	knob.setBounds(bounds.toNearestInt());
}
