
#include "TempoDisplay.h"


TempoDisplay::TempoDisplay(juce::AudioProcessorValueTreeState* vts)
	: mValueTreeState(vts)
{
	tempoLabel.setText("120", juce::NotificationType::dontSendNotification);
	glowLabel.setText("120", juce::NotificationType::dontSendNotification);
	tempoLabel.setJustificationType(juce::Justification::right);
	glowLabel.setJustificationType(juce::Justification::right);
	tempoLabel.setMinimumHorizontalScale(0.0f);
	glowLabel.setMinimumHorizontalScale(0.0f);

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


void TempoDisplay::SetText(juce::String text)
{
	using namespace juce;

	mText = text;
	tempoLabel.setText(mText, dontSendNotification);
	glowLabel.setText(mText, dontSendNotification);
	repaint();
}


void TempoDisplay::sliderValueChanged(juce::Slider* slider)
{
	using namespace juce;

	if ( ! mSyncActive)
	{
		const auto tempo = static_cast<int>(slider->getValue());
		const auto str = String(tempo);
		SetText(str);
	}
}


void TempoDisplay::TempoSyncChanged(const bool syncActive)
{
	using namespace juce;
	mSyncActive = syncActive;
	if (mSyncActive)
	{
		const String str{"snc"};
		SetText(str);
	}
	else
	{
		const auto tempoSyncParam = mValueTreeState->getParameter("bpm");
		const auto newValue = tempoSyncParam->getNormalisableRange().convertFrom0to1(tempoSyncParam->getValue());
		const auto truncValue = static_cast<int>(newValue);
		const String str{truncValue};
		SetText(str);
	}
	repaint();
}


void TempoDisplay::parameterChanged(const juce::String& parameterID, const float newValue)
{
	using namespace juce;

	if (parameterID == "tempoSync")
	{
		const bool active{newValue > 0.5f};
		TempoSyncChanged(active);
	}
	if (parameterID == "bpm")
	{
		if (! mSyncActive)
		{
			const auto truncValue = static_cast<int>(newValue);
			const String str{truncValue};
			SetText(str);
		}
	}
}
