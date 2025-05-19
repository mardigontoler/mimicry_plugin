#include "TempoControls.h"

using namespace juce;

mimicry::TempoControls::TempoControls(MimicAudioProcessor &processor, juce::AudioProcessorValueTreeState &vts)
    : mValueTreeState(&vts), tempoDisplay(&vts)
{

	tempoKnob.getLabel().setText("Tempo", NotificationType::dontSendNotification);
	tempoKnob.getLabel().setJustificationType(Justification::centred);
	addAndMakeVisible(tempoKnob);

	divisionKnob.getLabel().setText("Div", NotificationType::dontSendNotification);
	divisionKnob.getLabel().setJustificationType(Justification::centred);

	mixKnob.getLabel().setText("Mix", NotificationType::dontSendNotification);
	mixKnob.getLabel().setJustificationType(Justification::centred);

	// connect components to processor
	mixAttachment = std::make_unique<SliderAttachment>(*mValueTreeState, "mix", mixKnob.getSlider());
	tempoKnobAttachment = std::make_unique<SliderAttachment>(*mValueTreeState, "bpm", tempoKnob.getSlider());;
	tempoSyncBtnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(*mValueTreeState, "tempoSync", tempoSyncBtn);
	divisionKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*mValueTreeState, "division", divisionKnob.getSlider());

	addAndMakeVisible(tempoDisplay);
	addAndMakeVisible(tempoSyncBtn);
	addAndMakeVisible(mixKnob);
	addAndMakeVisible(divisionKnob);

}


void mimicry::TempoControls::resized()
{
	auto area = getLocalBounds();

	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::row;
	fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	fb.alignItems = juce::FlexBox::AlignItems::stretch;
	fb.flexWrap = juce::FlexBox::Wrap::noWrap;

	fb.items.add(FlexItem(tempoDisplay).withMinWidth(90).withMaxWidth(90));
	fb.items.add(FlexItem(tempoKnob).withMinWidth(50).withFlex(1.0f));
	fb.items.add(FlexItem(tempoSyncBtn).withMinWidth(50).withMaxWidth(50));
	fb.items.add(FlexItem(mixKnob).withMinWidth(50).withFlex(1.0f));

	fb.performLayout(area);
}


void mimicry::TempoControls::buttonStateChanged(juce::Button * button)
{
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
