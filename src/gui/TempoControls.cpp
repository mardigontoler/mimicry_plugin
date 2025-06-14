#include "TempoControls.h"

using namespace juce;

mimicry::TempoControls::TempoControls(MimicAudioProcessor&/*processor*/, AudioProcessorValueTreeState& vts)
        : mValueTreeState(vts), tempoDisplay(vts), tempoKnob([](double v)
                                                             { return String(v); })
{

    tempoKnob.getLabel().setText("Tempo", NotificationType::dontSendNotification);
    tempoKnob.getLabel().setJustificationType(Justification::centred);
    tempoKnob.getSlider().setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
    tempoKnob.getSlider().setNumDecimalPlacesToDisplay(0);
    addAndMakeVisible(tempoKnob);

    tempoSyncBtn.setButtonText("Sync");
    tempoSyncBtn.setClickingTogglesState(true);

    auto divParamRange = mValueTreeState.getParameterRange("division");
    int divValue = static_cast<int>(divParamRange.start);
    int id = 1;
    while (divValue <= static_cast<int>(divParamRange.end))
    {
        divComboBox.addItem(String("1 / ") + String(divValue), id);
        divValue += static_cast<int>(divParamRange.interval);
        id += 1;
    }

    // connect components to processor
    tempoKnobAttachment = std::make_unique<SliderAttachment>(mValueTreeState, "bpm", tempoKnob.getSlider());;
    tempoSyncBtnAttachment = std::make_unique<AudioProcessorValueTreeState::ButtonAttachment>(mValueTreeState,
                                                                                              "tempoSync",
                                                                                              tempoSyncBtn);
    divComboBoxAttachment = std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(mValueTreeState,
                                                                                               "division", divComboBox);


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

    FlexBox fb;
    fb.flexDirection = FlexBox::Direction::row;
    fb.justifyContent = FlexBox::JustifyContent::flexStart;
    fb.alignItems = FlexBox::AlignItems::stretch;
    fb.flexWrap = FlexBox::Wrap::noWrap;

    fb.items.add(FlexItem(tempoDisplay).withMinWidth(90).withMaxWidth(90));
    fb.items.add(FlexItem(tempoKnob).withMinWidth(50).withFlex(0.0f));
    fb.items.add(FlexItem(tempoSyncBtn).withMinWidth(50).withMaxWidth(50).withFlex(0.0f));
    fb.items.add(FlexItem(divComboBox).withMinWidth(50).withMaxWidth(100).withFlex(1.0f));

    fb.performLayout(area);
}


void mimicry::TempoControls::buttonStateChanged(juce::Button* button)
{
    if (button == &tempoSyncBtn)
    {
        bool tempoSyncBtnState = tempoSyncBtn.getToggleState();
        // disable the tempo knob if tempo sync is active
        tempoKnob.setEnabled(!tempoSyncBtnState);
    }
}
