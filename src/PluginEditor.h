
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "PluginProcessor.h"
#include "./gui/MimicryLookAndFeel.h"
#include "gui/TempoDisplay.h"
#include "gui/title/TitleLookAndFeel.h"
#include "gui/DelayHeadController.h"
#include "gui/WhiteLabelLookAndFeel.h"
#include "gui/DelayControllerLookAndFeel.h"

using namespace mimicry;
using namespace juce;

typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


//==============================================================================
/**
*/
class MimicAudioProcessorEditor  : public AudioProcessorEditor, public Button::Listener
{
public:
    MimicAudioProcessorEditor (MimicAudioProcessor&, AudioProcessorValueTreeState&);
    ~MimicAudioProcessorEditor();

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;




private:

    // reference to the processor
    MimicAudioProcessor& processor;

    // reference to the audio paramter tree
    AudioProcessorValueTreeState& valueTreeState;


    MimicryLookAndFeel mimicryLookAndFeel;
    TitleLookAndFeel titleLookAndFeel;
    SubTitleLookAndFeel subTitleLookAndFeel;
    class DigitalLookAndFeel : public MimicryLookAndFeel{
        Font getLabelFont(Label& label) override {
            return mimicry::getSegmentFont(50);
        }
    public:
        DigitalLookAndFeel(){
            setColour(Label::textColourId, Colour::fromRGBA(45, 255, 180, 150));
            setColour(Label::backgroundColourId, Colours::black);
        }
    } digitalLookAndFeel;


    WhiteLabelLookAndFeel whiteLabelLookAndFeel;
    DelayControllerLookAndFeel delayControllerLookAndFeel;

    // listener callback
    void buttonStateChanged(Button*) override;
    void buttonClicked(Button*) override;

    // child components
    //=======================
    TempoDisplay tempoDisplay;

    Slider mixKnob;
    std::unique_ptr<SliderAttachment> mixAttachment; // attach slider to processor param

    Slider tempoKnob;
    std::unique_ptr<SliderAttachment> tempoKnobAttachment;

    Slider divisionKnob;
    std::unique_ptr<SliderAttachment> divisionKnobAttachment;

    
    TextButton tempoSyncBtn;
    std::unique_ptr<ButtonAttachment> tempoSyncBtnAttachment;


    // areas to place components
    Rectangle<int> area;
    Rectangle<int> leftArea;
    Rectangle<int> headerArea;
    Rectangle<int> delayHeadsArea;
    Rectangle<int> delayHeadsTopArea;
    Rectangle<int> delayHeadsBottomArea;

    Label titleLabel;
    Label subTitleLabel;
    Label tempoLabel;
    Label divLabel;
    Label mixLabel;

    // array of sliders for the rhythm parameters
    OwnedArray<DelayHeadController> delayHeadControllers;
    OwnedArray<SliderAttachment> rhythmSliderAttachments;

    //========================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MimicAudioProcessorEditor);
};
