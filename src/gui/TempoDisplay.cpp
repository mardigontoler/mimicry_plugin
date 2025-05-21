
#include "TempoDisplay.h"

#include <utility>

std::vector<juce::String> listenedParameters{"tempoSync", "bpm"};


TempoDisplay::TempoDisplay(juce::AudioProcessorValueTreeState& vts)
	: mValueTreeState(vts)
{
	setLookAndFeel(&digitalLAF);
	tempoLabel.setText("120", juce::NotificationType::dontSendNotification);
	bgTextLabel.setText("000", juce::NotificationType::dontSendNotification);
	tempoLabel.setJustificationType(juce::Justification::right);
	bgTextLabel.setJustificationType(juce::Justification::right);
	tempoLabel.setMinimumHorizontalScale(0.0f);
	bgTextLabel.setMinimumHorizontalScale(0.0f);

	addAndMakeVisible(tempoLabel);
	addAndMakeVisible(bgTextLabel, 0);

	for (const auto& param : listenedParameters)
	{
		mValueTreeState.addParameterListener(param, this);
	}
}

TempoDisplay::~TempoDisplay()
{
	for (const auto& param : listenedParameters)
	{
		mValueTreeState.removeParameterListener(param, this);
	}
	setLookAndFeel(nullptr);
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

	const Colour mainColour = juce::Colours::grey;
	Colour glowLabelColour = mainColour.withAlpha(0.4f);
	bgTextLabel.setColour(Label::textColourId, glowLabelColour);

	tempoLabel.setBounds(textBoxBounds);
	bgTextLabel.setBounds(textBoxBounds);

}


void TempoDisplay::SetText(juce::String text)
{
	using namespace juce;

	mText = std::move(text);
//	bgTextLabel.setText(mText, dontSendNotification);
	tempoLabel.setText(mText, dontSendNotification);
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
		const auto tempoSyncParam = mValueTreeState.getParameter("bpm");
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
