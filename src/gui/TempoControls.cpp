#include "TempoControls.h"

using namespace juce;

mimicry::TempoControls::TempoControls(MimicAudioProcessor &/*processor*/, juce::AudioProcessorValueTreeState& vts)
    : mValueTreeState(vts), tempoDisplay(vts), tempoKnob([](double v){return String(v);})
{

	tempoKnob.getLabel().setText("Tempo", NotificationType::dontSendNotification);
	tempoKnob.getLabel().setJustificationType(Justification::centred);
	tempoKnob.getSlider().setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	tempoKnob.getSlider().setNumDecimalPlacesToDisplay(0);
	addAndMakeVisible(tempoKnob);

//	divisionKnob.getLabel().setText("Div", NotificationType::dontSendNotification);
//	divisionKnob.getSlider().setName("Div");
//	divisionKnob.getLabel().setJustificationType(Justification::centred);
//	divisionKnob.getSlider().setTextBoxStyle(Slider::TextBoxRight, true, 30, 18);
//	divisionKnob.getSlider().setNumDecimalPlacesToDisplay(0);

//	mixKnob.getLabel().setText("Mix", NotificationType::dontSendNotification);
//	mixKnob.getLabel().setJustificationType(Justification::centred);
//	mixKnob.getSlider().setTextBoxStyle(Slider::NoTextBox, true, 0, 0);

	tempoSyncBtn.setButtonText("Sync");
	tempoSyncBtn.setClickingTogglesState(true);

	auto divParamRange = mValueTreeState.getParameterRange("division");
	int divValue = static_cast<int>(divParamRange.start);
	int id = 1;
	while (divValue <= static_cast<int>(divParamRange.end)) {
		divComboBox.addItem(String("1 / ") + String(divValue), id);
		divValue += static_cast<int>(divParamRange.interval);
		id += 1;
	}

	// connect components to processor
//	mixAttachment = std::make_unique<SliderAttachment>(mValueTreeState, "mix", mixKnob.getSlider());
	tempoKnobAttachment = std::make_unique<SliderAttachment>(mValueTreeState, "bpm", tempoKnob.getSlider());;
	tempoSyncBtnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(mValueTreeState, "tempoSync", tempoSyncBtn);
//	divisionKnobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(mValueTreeState, "division", divisionKnob.getSlider());
	divComboBoxAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(mValueTreeState, "division", divComboBox);


	addAndMakeVisible(tempoDisplay);
	addAndMakeVisible(tempoSyncBtn);
//	addAndMakeVisible(mixKnob);
//	addAndMakeVisible(divisionKnob);
	addAndMakeVisible(divComboBox);

	tempoSyncBtn.addListener(this);

}


mimicry::TempoControls::~TempoControls()
{
	tempoSyncBtn.removeListener(this);
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
	fb.items.add(FlexItem(tempoKnob).withMinWidth(50).withFlex(0.0f));
	fb.items.add(FlexItem(tempoSyncBtn).withMinWidth(50).withMaxWidth(50).withFlex(0.0f));
//	fb.items.add(FlexItem(divisionKnob).withMinWidth(50).withMaxWidth(80).withFlex(1.0f));
	fb.items.add(FlexItem(divComboBox).withMinWidth(50).withMaxWidth(100).withFlex(1.0f));
//	fb.items.add(FlexItem(mixKnob).withMinWidth(50).withFlex(0.0f));

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
