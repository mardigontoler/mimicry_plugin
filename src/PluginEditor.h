
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "PluginProcessor.h"
#include "./gui/MimicryLookAndFeel.h"
#include "gui/TempoDisplay.h"
#include "gui/title/TitleLookAndFeel.h"
#include "gui/DelayHeadController.h"
#include "gui/WhiteLabelLookAndFeel.h"
#include "gui/DelayControllerLookAndFeel.h"
#include "gui/DigitalLookAndFeel.h"
#include "gui/TempoControls.h"

#include "melatonin_inspector/melatonin_inspector.h"

typedef juce::AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
typedef juce::AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;


//==============================================================================
/**
*/
class MimicAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    MimicAudioProcessorEditor(MimicAudioProcessor&, juce::AudioProcessorValueTreeState&);

    ~MimicAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;

    void resized() override;

private:

//	melatonin::Inspector inspector { *this };

    // reference to the processor
    MimicAudioProcessor& processor;

    // reference to the audio paramter tree
    juce::AudioProcessorValueTreeState& valueTreeState;

    mimicry::MimicryLookAndFeel mimicryLookAndFeel;
    TitleLookAndFeel titleLookAndFeel;
    SubTitleLookAndFeel subTitleLookAndFeel;

    mimicry::WhiteLabelLookAndFeel whiteLabelLookAndFeel;
    mimicry::DelayControllerLookAndFeel delayControllerLookAndFeel;


    // child components
    //=======================

    mimicry::TempoControls tempoControls;

    juce::Label titleLabel;

    LabeledSlider mixKnob;
    std::unique_ptr<SliderAttachment> mixAttachment; // attach slider to processor param

    LabeledSlider outputGainKnob;
    std::unique_ptr<SliderAttachment> outputGainAttachment;

    // array of sliders for the rhythm parameters
    juce::OwnedArray<DelayHeadController> delayHeadControllers;
    juce::OwnedArray<SliderAttachment> rhythmSliderAttachments;

    //========================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MimicAudioProcessorEditor)
};
