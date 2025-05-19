

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/colors.h"

#include <memory>

using namespace juce;
using namespace mimicry;


//==============================================================================
MimicAudioProcessorEditor::MimicAudioProcessorEditor (MimicAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts), tempoControls(p, vts)

{

    titleLabel.setText("mimicry", NotificationType::dontSendNotification);
    titleLabel.setLookAndFeel(&titleLookAndFeel);
    titleLabel.setJustificationType(Justification::right);
    addAndMakeVisible(titleLabel);

	addAndMakeVisible(&tempoControls);

    setLookAndFeel(&mimicryLookAndFeel);
    for (int i = 0; i < numStereoDelayLines; i++) {
        delayHeadControllers.add(new DelayHeadController(String{i + 1}));
		const String iStr{i};
		String delayGainParamString = String("rhythmGain") + iStr;
        String semitoneParamString = String("pitchShift") + iStr;
		String feedbackParamString = String("feedback") + iStr;

        DelayHeadController* controller = delayHeadControllers.getLast();
        controller->resetDelayGainAttachment(valueTreeState, delayGainParamString);
        controller->resetSemitoneAttachment(valueTreeState, semitoneParamString);
		controller->resetFeedbackAttachment(valueTreeState, feedbackParamString);
        controller->setLookAndFeel(&delayControllerLookAndFeel);
        addAndMakeVisible(controller);
    }

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1125, 540);

	setResizeLimits(670, 250, 4500, 2160);

	setResizable(true, true);
	setBounds(getLocalBounds());
}


MimicAudioProcessorEditor::~MimicAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}


//==============================================================================
void MimicAudioProcessorEditor::paint (juce::Graphics& g)
{
	auto bounds = getLocalBounds().toFloat();

	// Base gradient
	juce::ColourGradient baseGradient(
			juce::Colour(mimicry::Colors::getBgGradientCol1()),
			bounds.getTopLeft(),
			juce::Colour(mimicry::Colors::getBgGradientCol1()),
			bounds.getBottomRight(),
			false
	);

	baseGradient.addColour(0.25f, mimicry::Colors::getBgGradientCol2());
	baseGradient.addColour(0.75f, mimicry::Colors::getBgGradientCol2());

	g.setGradientFill(baseGradient);
	g.fillRect(bounds);

	// Overlay gradients with slight offsets and low opacity
	for (int i = 0; i < 3; ++i)
	{
		float offset = static_cast<float>(i + 1) * 30;

		juce::ColourGradient overlayGradient(
				juce::Colour(mimicry::Colors::getBgGradientCol1()).withAlpha(0.3f),
				bounds.getTopLeft().translated(offset, offset),
				juce::Colour(mimicry::Colors::getBgGradientCol1()).withAlpha(0.3f),
				bounds.getBottomRight().translated(-offset, -offset),
				false
		);
		baseGradient.addColour(0.25f, mimicry::Colors::getBgGradientCol2());
		baseGradient.addColour(0.75f, mimicry::Colors::getBgGradientCol2());

		g.setGradientFill(overlayGradient);
		g.fillRect(bounds);
	}


}

void MimicAudioProcessorEditor::resized()
{
    area = getLocalBounds().toFloat();
	bannerArea = area.removeFromTop(60);
    delayHeadsArea = area.reduced(0, 5);
    delayHeadsTopArea = area.removeFromTop(static_cast<int>(
        static_cast<float>(area.getHeight()) / 2.0f))
        .reduced(30, 15);

    delayHeadsBottomArea = area.reduced(30, 15);

	juce::FlexBox bannerFlexBox;
	bannerFlexBox.flexDirection = juce::FlexBox::Direction::row;
	bannerFlexBox.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	bannerFlexBox.alignItems = juce::FlexBox::AlignItems::stretch;
	bannerFlexBox.flexWrap = juce::FlexBox::Wrap::noWrap;

	bannerFlexBox.items.add(juce::FlexItem(tempoControls).withFlex(0.5).withMinHeight(bannerArea.getHeight()) );
	bannerFlexBox.items.add(juce::FlexItem(titleLabel).withFlex(0.5) );

	bannerFlexBox.performLayout(bannerArea);

    // place delay line components in two rows
	float margin = 2.0f;
    constexpr int delayHeadsPerRow = numStereoDelayLines / 2;
    const float delayHeadControllerSpacing = delayHeadsTopArea.getWidth() / delayHeadsPerRow;
    for(int i = 0; i < numStereoDelayLines; i++){
        if(i < delayHeadsPerRow)
            delayHeadControllers[i]->setBounds(delayHeadsTopArea.removeFromLeft(delayHeadControllerSpacing).reduced(margin).toNearestInt());
        else
            delayHeadControllers[i]->setBounds(delayHeadsBottomArea.removeFromLeft(delayHeadControllerSpacing).reduced(margin).toNearestInt());
    }

}
