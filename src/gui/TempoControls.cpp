#include "TempoControls.h"

using namespace juce;

mimicry::TempoControls::TempoControls(MimicAudioProcessor &processor, juce::AudioProcessorValueTreeState &vts)
    : mValueTreeState(&vts), tempoDisplay(&vts)
{

	addAndMakeVisible(tempoDisplay);
	addAndMakeVisible(tempoSyncBtn);
	addAndMakeVisible(mixKnob);
	addAndMakeVisible(divisionKnob);

	tempoKnob.getLabel().setText("Tempo", NotificationType::dontSendNotification);
	tempoKnob.getLabel().setJustificationType(Justification::centred);
	addAndMakeVisible(tempoKnob);

	divLabel.setText("Div", NotificationType::dontSendNotification);
	divLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(divLabel);

	mixLabel.setText("Mix", NotificationType::dontSendNotification);
	mixLabel.setJustificationType(Justification::centred);
	addAndMakeVisible(mixLabel);

	// connect components to processor
	mixAttachment = std::make_unique<SliderAttachment>(*mValueTreeState, "mix", mixKnob);
	tempoKnobAttachment = std::make_unique<SliderAttachment>(*mValueTreeState, "bpm", tempoKnob.getSlider());;
	tempoSyncBtnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(*mValueTreeState, "tempoSync", tempoSyncBtn);
	divisionKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(*mValueTreeState, "division", divisionKnob);

}


void mimicry::TempoControls::resized()
{
	auto area = getLocalBounds();

	juce::FlexBox fb;
	fb.flexDirection = juce::FlexBox::Direction::row;
	fb.justifyContent = juce::FlexBox::JustifyContent::flexStart;
	fb.alignItems = juce::FlexBox::AlignItems::stretch;
	fb.flexWrap = juce::FlexBox::Wrap::noWrap;

	for (Component& item : std::initializer_list<std::reference_wrapper<juce::Component>>{
		tempoDisplay, tempoKnob, tempoSyncBtn, divisionKnob})
	{
		fb.items.add(FlexItem(item).withFlex(1.0f).withMinHeight(50));
	}

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
