

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/colors.h"

#include <memory>

using namespace juce;
using namespace mimicry;


//==============================================================================
MimicAudioProcessorEditor::MimicAudioProcessorEditor (MimicAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts), tempoDisplay(&vts)

{

    titleLabel.setText("mimicry", NotificationType::dontSendNotification);
    titleLabel.setLookAndFeel(&titleLookAndFeel);
    titleLabel.setJustificationType(Justification::right);
    addAndMakeVisible(titleLabel);

    tempoLabel.setText("Tempo", NotificationType::dontSendNotification);
    tempoLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(tempoLabel);

    divLabel.setText("Div", NotificationType::dontSendNotification);
    divLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(divLabel);

    mixLabel.setText("Mix", NotificationType::dontSendNotification);
    mixLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(mixLabel);



    tempoDisplay.setLookAndFeel(&digitalLAF);
    addAndMakeVisible(tempoDisplay);

    mixKnob.setSliderStyle(Slider::SliderStyle::Rotary);
    mixKnob.setTextBoxStyle(Slider::NoTextBox, true, 0,0);
    addAndMakeVisible(mixKnob);


    tempoKnob.setSliderStyle(Slider::SliderStyle::Rotary);
    tempoKnob.setTextBoxStyle(Slider::NoTextBox, true, 80, 50);
    tempoKnob.addListener(&tempoDisplay);
    addAndMakeVisible(tempoKnob);


    tempoSyncBtn.setButtonText("Sync");
    tempoSyncBtn.setClickingTogglesState(true);
    tempoSyncBtn.addListener(this); // handle callbacks if the tempo sync state is clicked
    addAndMakeVisible(tempoSyncBtn);


    divisionKnob.setRange(divisionKnob.getRange(), 1.0f);
    divisionKnob.setSliderStyle(Slider::SliderStyle::Rotary);
    divisionKnob.setTextBoxStyle(Slider::TextBoxBelow, false, 50, 25);
    divisionKnob.setLookAndFeel(&delayControllerLookAndFeel);
    addAndMakeVisible(divisionKnob);


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


    // connect components to processor
    mixAttachment = std::make_unique<SliderAttachment>(valueTreeState, "mix", mixKnob);
    tempoKnobAttachment = std::make_unique<SliderAttachment>(valueTreeState, "bpm", tempoKnob);
    tempoSyncBtnAttachment = std::make_unique<ButtonAttachment>(valueTreeState, "tempoSync", tempoSyncBtn);
    divisionKnobAttachment = std::make_unique<SliderAttachment>(valueTreeState, "division", divisionKnob);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1125, 540);

	setResizeLimits(670, 250, 4500, 2160);

	setResizable(true, true);
	setBounds(getLocalBounds());

	valueTreeState.addParameterListener("tempoSync", &tempoDisplay);

}


MimicAudioProcessorEditor::~MimicAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
    valueTreeState.removeParameterListener("tempoSync", &tempoDisplay);
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
    area = getLocalBounds();
	bannerArea = area.removeFromTop(40);
    delayHeadsArea = area.reduced(0, 5);
    delayHeadsTopArea = area.removeFromTop(static_cast<int>(
        static_cast<float>(area.getHeight()) / 2.0f))
        .reduced(30, 15);

    delayHeadsBottomArea = area.reduced(30, 15);

	// Create flexbox for the tempo controls
	juce::FlexBox tempoControls;
	tempoControls.flexDirection = juce::FlexBox::Direction::row;
	tempoControls.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	tempoControls.alignItems = juce::FlexBox::AlignItems::center;
	tempoControls.flexWrap = juce::FlexBox::Wrap::noWrap;

	// Add items to the tempo controls flexbox
	tempoControls.items.add(juce::FlexItem(tempoLabel).withMinWidth(30.0f).withMargin(3.0f));
	tempoControls.items.add(juce::FlexItem(tempoDisplay).withMinWidth(30.0f).withHeight(30.0f).withMargin(3.0f));
	tempoControls.items.add(juce::FlexItem(tempoKnob).withWidth(75.0f).withHeight(75.0f).withMargin(3.0f));
	tempoControls.items.add(juce::FlexItem(tempoSyncBtn).withWidth(45.0f).withHeight(25.0f).withMargin(3.0f));

	// Create flexbox for the division controls
	juce::FlexBox divisionControls;
	divisionControls.flexDirection = juce::FlexBox::Direction::row;
	divisionControls.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	divisionControls.alignItems = juce::FlexBox::AlignItems::center;
	divisionControls.flexWrap = juce::FlexBox::Wrap::noWrap;

	// Add items to the division controls flexbox
	divisionControls.items.add(juce::FlexItem(divLabel).withWidth(60.0f).withMargin(3.0f));
	divisionControls.items.add(juce::FlexItem(divisionKnob).withWidth(75.0f).withHeight(75.0f).withMargin(3.0f));


	// Create flexbox for the mix controls
	juce::FlexBox mixControls;
	mixControls.flexDirection = juce::FlexBox::Direction::row;
	mixControls.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	mixControls.alignItems = juce::FlexBox::AlignItems::center;
	mixControls.flexWrap = juce::FlexBox::Wrap::noWrap;

	// Add items to the mix controls flexbox
	mixControls.items.add(juce::FlexItem(mixLabel).withWidth(60.0f).withMargin(3.0f));
	mixControls.items.add(juce::FlexItem(mixKnob).withWidth(75.0f).withHeight(75.0f).withMargin(3.0f));

	// Create main vertical flexbox for the control panel
	juce::FlexBox controlPanel;
	controlPanel.flexDirection = juce::FlexBox::Direction::row;
	controlPanel.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	controlPanel.alignItems = juce::FlexBox::AlignItems::flexStart;
	controlPanel.flexWrap = juce::FlexBox::Wrap::noWrap;

	// Add the control groups to the main flexbox
	auto controlArea = bannerArea.removeFromLeft(800);
	controlPanel.items.add(juce::FlexItem(tempoControls).withWidth(280.0f).withMargin(5.0f));
	controlPanel.items.add(juce::FlexItem(divisionControls).withWidth(280.0f).withMargin(5.0f));
	controlPanel.items.add(juce::FlexItem(mixControls).withWidth(280.0f).withMargin(5.0f));

	// Perform the layout
	controlPanel.performLayout(controlArea.toFloat());


	Rectangle<int> titleLabelArea = bannerArea.removeFromRight(300);
    titleLabelArea.translate(-3, 10);
    titleLabel.setBounds(titleLabelArea);


    // place delay line components in two rows
	auto margin = 2;
    constexpr int delayHeadsPerRow = numStereoDelayLines / 2;
    const int delayHeadControllerSpacing = delayHeadsTopArea.getWidth() / delayHeadsPerRow;
    for(int i = 0; i < numStereoDelayLines; i++){
        if(i < delayHeadsPerRow)
            delayHeadControllers[i]->setBounds(delayHeadsTopArea.removeFromLeft(delayHeadControllerSpacing).reduced(margin));
        else
            delayHeadControllers[i]->setBounds(delayHeadsBottomArea.removeFromLeft(delayHeadControllerSpacing).reduced(margin));
    }

}

void MimicAudioProcessorEditor::buttonStateChanged(Button* button) {
    
    if (button == &tempoSyncBtn) {
        bool tempoSyncBtnState = tempoSyncBtn.getToggleState();
        #if JUCE_DEBUG
        std::string msg = tempoSyncBtnState ? "On" : "Off";
        DBG(msg);
        #endif

        // disable the tempo knob if tempo sync is active
        tempoKnob.setEnabled(!tempoSyncBtnState);
    }
}

void MimicAudioProcessorEditor::buttonClicked(Button* /*button*/) {
}
