
#include "TempoDisplay.h"



TempoDisplay::TempoDisplay()
{
	tempoLabel.setText("120", juce::NotificationType::dontSendNotification);
	glowLabel.setText("120", juce::NotificationType::dontSendNotification);
	tempoLabel.setJustificationType(juce::Justification::right);
	glowLabel.setJustificationType(juce::Justification::right);
	tempoLabel.setMinimumHorizontalScale(1.0f);
	glowLabel.setMinimumHorizontalScale(1.0f);

	addAndMakeVisible(tempoLabel);
//	addAndMakeVisible(glowLabel, 1);
}

void TempoDisplay::paint(juce::Graphics&g)
{
	g.setColour(getLookAndFeel().findColour(juce::Label::backgroundColourId));
	g.fillRoundedRectangle(getLocalBounds().toFloat(), 5);
}

void TempoDisplay::resized()
{
	using namespace juce;
	//        tempoLabel.setBounds(getLocalBounds());
	const Rectangle<int> localBounds = getLocalBounds();
	const Rectangle<int> textBoxBounds(
		localBounds.getX() - 5,
		localBounds.getY() + 5,
		localBounds.getWidth(),
		localBounds.getHeight()
	);
	Rectangle<int> glowTextBoxBounds = textBoxBounds;
	glowTextBoxBounds.translate(-2, -2);

	const Colour mainColour = tempoLabel.findColour(Label::textColourId, false);
	Colour glowLabelColour = mainColour.withAlpha(0.4f);
//	glowLabel.setColour(Label::textColourId, glowLabelColour);

	tempoLabel.setBounds(textBoxBounds);
//	glowLabel.setBounds(glowTextBoxBounds);

}


void TempoDisplay::sliderValueChanged(juce::Slider* slider)
{
	using namespace juce;
	const auto tempo = static_cast<int>(slider->getValue());
	const auto str = String(tempo);
	tempoLabel.setText(str, dontSendNotification);
	glowLabel.setText(str, dontSendNotification);
}
