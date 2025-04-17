
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "PluginProcessor.h"
#include "./gui/MimicryLookAndFeel.h"
#include "gui/TempoDisplay.h"
#include "gui/title/TitleLookAndFeel.h"
#include "gui/DelayHeadController.h"
#include "gui/WhiteLabelLookAndFeel.h"
#include "gui/DelayControllerLookAndFeel.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


//==============================================================================
/**
*/
class MimicAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Button::Listener
{
public:
    MimicAudioProcessorEditor (MimicAudioProcessor&, juce::AudioProcessorValueTreeState&);
    ~MimicAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:

    // reference to the processor
    MimicAudioProcessor& processor;

    // reference to the audio paramter tree
    juce::AudioProcessorValueTreeState& valueTreeState;


    mimicry::MimicryLookAndFeel mimicryLookAndFeel;
    TitleLookAndFeel titleLookAndFeel;
    SubTitleLookAndFeel subTitleLookAndFeel;

    class DigitalLookAndFeel : public mimicry::MimicryLookAndFeel
    {
        juce::Font getLabelFont(juce::Label& label) override
        {
            return mimicry::getSegmentFont(50);
        }
    public:
        DigitalLookAndFeel(){
            setColour(juce::Label::textColourId, juce::Colour::fromRGBA(45, 255, 180, 150));
            setColour(juce::Label::backgroundColourId, juce::Colours::black);
        }
    } digitalLookAndFeel;


    mimicry::WhiteLabelLookAndFeel whiteLabelLookAndFeel;
    mimicry::DelayControllerLookAndFeel delayControllerLookAndFeel;

    // listener callback
    void buttonStateChanged(juce::Button*) override;
    void buttonClicked(juce::Button*) override;

    // child components
    //=======================
    TempoDisplay tempoDisplay;

    juce::Slider mixKnob;
    std::unique_ptr<SliderAttachment> mixAttachment; // attach slider to processor param

    juce::Slider tempoKnob;
    std::unique_ptr<SliderAttachment> tempoKnobAttachment;

    juce::Slider divisionKnob;
    std::unique_ptr<SliderAttachment> divisionKnobAttachment;

    
    juce::TextButton tempoSyncBtn;
    std::unique_ptr<ButtonAttachment> tempoSyncBtnAttachment;


    // areas to place components
    juce::Rectangle<int> area;
    juce::Rectangle<int> leftArea;
    juce::Rectangle<int> headerArea;
    juce::Rectangle<int> delayHeadsArea;
    juce::Rectangle<int> delayHeadsTopArea;
    juce::Rectangle<int> delayHeadsBottomArea;

    juce::Label titleLabel;
    juce::Label subTitleLabel;
    juce::Label tempoLabel;
    juce::Label divLabel;
    juce::Label mixLabel;

    // array of sliders for the rhythm parameters
    juce::OwnedArray<DelayHeadController> delayHeadControllers;
    juce::OwnedArray<SliderAttachment> rhythmSliderAttachments;

    //========================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MimicAudioProcessorEditor);
};
