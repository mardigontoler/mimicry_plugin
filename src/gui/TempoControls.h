#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "DelayHeadController.h"
#include "TempoDisplay.h"
#include "juce_audio_processors/juce_audio_processors.h"
#include "DigitalLookAndFeel.h"
#include "LabeledSlider.h"
#include "PluginProcessor.h"

namespace mimicry
{
class TempoControls : public juce::Component, public juce::Button::Listener {
	public:
		explicit TempoControls(MimicAudioProcessor& processor, juce::AudioProcessorValueTreeState& vts);
		 ~TempoControls() override;

		void resized() override;

		// listener callback
		void buttonStateChanged(juce::Button*) override;
		void buttonClicked(juce::Button*) override {}

	private:

		juce::AudioProcessorValueTreeState& mValueTreeState;

		mimicry::DigitalLookAndFeel digitalLAF;
		TempoDisplay tempoDisplay;

//		LabeledSlider mixKnob;
//		std::unique_ptr<SliderAttachment> mixAttachment; // attach slider to processor param

		LabeledSlider tempoKnob;
		std::unique_ptr<SliderAttachment> tempoKnobAttachment;

//		LabeledSlider divisionKnob;
//		std::unique_ptr<SliderAttachment> divisionKnobAttachment;

		juce::ComboBox divComboBox;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> divComboBoxAttachment;

		juce::TextButton tempoSyncBtn;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncBtnAttachment;
	};
}
