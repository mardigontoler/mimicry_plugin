

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/font.h"

//==============================================================================
MimicAudioProcessorEditor::MimicAudioProcessorEditor (MimicAudioProcessor& p, AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vts)

{

    titleLabel.setText("Mimicry", NotificationType::dontSendNotification);
    titleLabel.setLookAndFeel(&titleLookAndFeel);
    titleLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(titleLabel);

    subTitleLabel.setText("Delay and Arpeggio", NotificationType::dontSendNotification);
    subTitleLabel.setJustificationType(Justification::centred);
    subTitleLabel.setLookAndFeel(&subTitleLookAndFeel);
    addAndMakeVisible(subTitleLabel);


    tempoLabel.setText("Tempo", NotificationType::dontSendNotification);
    tempoLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(tempoLabel);

    divLabel.setText("Div", NotificationType::dontSendNotification);
    divLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(divLabel);

    mixLabel.setText("Mix", NotificationType::dontSendNotification);
    mixLabel.setJustificationType(Justification::centred);
    addAndMakeVisible(mixLabel);



    tempoDisplay.setLookAndFeel(&digitalLookAndFeel);
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
        delayHeadControllers.add(new DelayHeadController());
        String delayGainParamString = String("rhythmGain") + String(i);
        String semitoneParamString = String("pitchShift") + String(i);

        DelayHeadController* controller = delayHeadControllers.getLast();
        controller->resetDelayGainAttachment(valueTreeState, delayGainParamString);
        controller->resetSemitoneAttachment(valueTreeState, semitoneParamString);
        controller->setLookAndFeel(&delayControllerLookAndFeel);
        addAndMakeVisible(controller);
    }




    // connect components to processor
    mixAttachment.reset(new SliderAttachment(valueTreeState, "mix", mixKnob));
    tempoKnobAttachment.reset(new SliderAttachment(valueTreeState, "bpm", tempoKnob));
    tempoSyncBtnAttachment.reset(new ButtonAttachment(valueTreeState, "tempoSync", tempoSyncBtn));
    divisionKnobAttachment.reset(new SliderAttachment(valueTreeState, "division", divisionKnob));



    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (742, 560);
    setResizable(false, false);
    setBounds(getLocalBounds());

}

MimicAudioProcessorEditor::~MimicAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

//==============================================================================
void MimicAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // draw rectangles with margins for the left area and the delay lines area
    g.setColour(Colour::fromRGB(27,27,28));
    g.fillRoundedRectangle(leftArea.reduced(10).toFloat(), 10.0f);
    g.fillRoundedRectangle(delayHeadsArea.reduced(5).toFloat(), 10.0f);
}

void MimicAudioProcessorEditor::resized()
{
    area = getLocalBounds();
    leftArea = area.removeFromLeft(210);
    headerArea = leftArea.removeFromTop(50);
    delayHeadsArea = area.reduced(0, 5);
    delayHeadsTopArea = area.removeFromTop((int)(area.getHeight() / 2.0f)).reduced(30, 15);
    delayHeadsBottomArea = area.reduced(30, 15);

    Rectangle<int> titleLabelArea = headerArea.removeFromTop((int)(2.0 * headerArea.getHeight() / 3.0f));
    titleLabelArea.translate(-3, 10);
    titleLabel.setBounds(titleLabelArea);

    Rectangle<int> subTitleArea = headerArea; // copy
    subTitleArea.translate(0, 10);
    subTitleLabel.setBounds(subTitleArea);

    // place components in sidebar
    int labelAreaAmount = 20;
    int labelMargin = 0;
    int knobSize = 75;
    int knobXMargin = 0;
    int knobYMargin = 0;
    int spacerSize = 30;

    Rectangle<int> leftBar = leftArea; // copy rectangle
    leftBar.removeFromTop(15);
    Rectangle<int> tempoLabelBounds = leftBar.removeFromTop(labelAreaAmount).reduced(labelMargin);
    tempoLabelBounds.translate(0, 15);
    tempoLabel.setBounds(tempoLabelBounds);
    tempoDisplay.setBounds(leftBar.removeFromTop(100).reduced(50, 15));
    tempoKnob.setBounds(leftBar.removeFromTop(knobSize).reduced(knobXMargin, knobYMargin));
    tempoSyncBtn.setBounds(leftBar.removeFromTop(45).reduced(50, 5));

    leftBar.removeFromTop(spacerSize);

    divLabel.setBounds(leftBar.removeFromTop(labelAreaAmount).reduced(labelMargin));
    divisionKnob.setBounds(leftBar.removeFromTop(knobSize).reduced(knobXMargin, knobYMargin));

    leftBar.removeFromTop(spacerSize);

    mixLabel.setBounds(leftBar.removeFromTop(labelAreaAmount).reduced(labelMargin));
    mixKnob.setBounds(leftBar.removeFromTop(knobSize).reduced(knobXMargin, knobYMargin));



    // place delay line components on right side in two rows
    int delayHeadsPerRow = numStereoDelayLines / 2;
    int delayHeadControllerSpacing = delayHeadsTopArea.getWidth() / delayHeadsPerRow;
    for(int i = 0; i < numStereoDelayLines; i++){
        if(i < delayHeadsPerRow)
            delayHeadControllers[i]->setBounds(delayHeadsTopArea.removeFromLeft(delayHeadControllerSpacing));
        else
            delayHeadControllers[i]->setBounds(delayHeadsBottomArea.removeFromLeft(delayHeadControllerSpacing));
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

void MimicAudioProcessorEditor::buttonClicked(Button* button) {
}
