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

		void resized() override;

		// listener callback
		void buttonStateChanged(juce::Button*) override;
		void buttonClicked(juce::Button*) override {}

	private:

		juce::AudioProcessorValueTreeState* mValueTreeState;

		mimicry::DigitalLookAndFeel digitalLAF;
		TempoDisplay tempoDisplay;

		juce::Slider mixKnob;
		std::unique_ptr<SliderAttachment> mixAttachment; // attach slider to processor param

		LabeledSlider tempoKnob;
		std::unique_ptr<SliderAttachment> tempoKnobAttachment;

		juce::Slider divisionKnob;
		std::unique_ptr<SliderAttachment> divisionKnobAttachment;

		juce::TextButton tempoSyncBtn;
		std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> tempoSyncBtnAttachment;

		juce::Label divLabel;
		juce::Label mixLabel;
	};
}
