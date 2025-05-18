
#include "LabeledSlider.h"

LabeledSlider::LabeledSlider()
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

	label.attachToComponent(&knob, true);
}

void LabeledSlider::resized()
{
	auto bounds = getLocalBounds();
	knob.setBounds(bounds);
}
