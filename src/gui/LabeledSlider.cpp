
#include "LabeledSlider.h"

LabeledSlider::LabeledSlider(const juce::String &labelText)
{
	// Setup label
	label.setText(labelText, juce::dontSendNotification);
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
	auto bounds = getLocalBounds();
	label.setBounds(bounds.removeFromTop(20));
	knob.setBounds(bounds);
}
