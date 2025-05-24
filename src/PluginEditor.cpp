

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "gui/colors.h"

#include <memory>

using namespace juce;
using namespace mimicry;


String mixValueToString(double v)
{
    return String(v * 100.0) + "%";
}

String outputGainToString(double v)
{
    return String(v);
}


//==============================================================================
MimicAudioProcessorEditor::MimicAudioProcessorEditor(MimicAudioProcessor& p, AudioProcessorValueTreeState& vts)
        : AudioProcessorEditor(&p), processor(p), valueTreeState(vts), tempoControls(p, vts),
          mixKnob(mixValueToString), outputGainKnob(outputGainToString)
{

    titleLabel.setText("mimicry", NotificationType::dontSendNotification);
    titleLabel.setLookAndFeel(&titleLookAndFeel);
    titleLabel.setJustificationType(Justification::right);
    addAndMakeVisible(titleLabel);

    addAndMakeVisible(&tempoControls);

    setLookAndFeel(&mimicryLookAndFeel);
    for (int i = 0; i < numStereoDelayLines; i++)
    {
        delayHeadControllers.add(new DelayHeadController(String{i + 1}));
        const String iStr{i};
        String delayGainParamString = String("rhythmGain") + iStr;
        String semitoneParamString = String("pitchShift") + iStr;
        String feedbackParamString = String("feedback") + iStr;

        DelayHeadController* controller = delayHeadControllers.getLast();
        controller->resetDelayGainAttachment(valueTreeState, delayGainParamString);
        controller->resetSemitoneAttachment(valueTreeState, semitoneParamString);
        controller->resetFeedbackAttachment(valueTreeState, feedbackParamString);
        controller->setLookAndFeel(&delayControllerLookAndFeel);
        addAndMakeVisible(controller);
    }

    mixKnob.getSlider().setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    mixKnob.getLabel().setText("Dry/Wet", dontSendNotification);
    mixKnob.getSlider().setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, true, 30, 20);

    mixAttachment = std::make_unique<SliderAttachment>(valueTreeState, "mix", mixKnob.getSlider());
    addAndMakeVisible(mixKnob);

    outputGainKnob.getSlider().setSliderStyle(Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    outputGainKnob.getSlider().setName("Output Gain");
    outputGainKnob.getLabel().setText("Output Gain", dontSendNotification);
    outputGainKnob.getSlider().setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxRight, true, 30, 20);
    outputGainAttachment = std::make_unique<SliderAttachment>(valueTreeState, "outputGain", outputGainKnob.getSlider());
    addAndMakeVisible(outputGainKnob);

    addAndMakeVisible(bannderDivider);
    addAndMakeVisible(delayRowDivider);
    addAndMakeVisible(rightPanelDivider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(1125, 540);

    setResizeLimits(670, 250, 4500, 2160);

    setResizable(true, true);
    setBounds(getLocalBounds());
}


MimicAudioProcessorEditor::~MimicAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}


//==============================================================================
void MimicAudioProcessorEditor::paint(Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();

    // Base gradient
    ColourGradient baseGradient(
            Colour(mimicry::Colors::getBgGradientCol1()),
            bounds.getTopLeft(),
            Colour(mimicry::Colors::getBgGradientCol1()),
            bounds.getBottomRight(),
            false
    );

    baseGradient.addColour(0.25f, mimicry::Colors::getBgGradientCol2());
    baseGradient.addColour(0.75f, mimicry::Colors::getBgGradientCol2());

    g.setGradientFill(baseGradient);
    g.fillRect(bounds);

    // Overlay gradients with slight offsets and low opacity
    for (int i = 0; i < 3; ++i)
    {
        float offset = static_cast<float>(i + 1) * 30;

        ColourGradient overlayGradient(
                Colour(mimicry::Colors::getBgGradientCol1()).withAlpha(0.3f),
                bounds.getTopLeft().translated(offset, offset),
                Colour(mimicry::Colors::getBgGradientCol1()).withAlpha(0.3f),
                bounds.getBottomRight().translated(-offset, -offset),
                false
        );
        baseGradient.addColour(0.25f, mimicry::Colors::getBgGradientCol2());
        baseGradient.addColour(0.75f, mimicry::Colors::getBgGradientCol2());

        g.setGradientFill(overlayGradient);
        g.fillRect(bounds);
    }


}

void MimicAudioProcessorEditor::resized()
{
    FlexBox bannerFlexBox; // banner at the top containing tempo controls, preset selector, logo, settings
    bannerFlexBox.flexDirection = FlexBox::Direction::row;
    bannerFlexBox.justifyContent = FlexBox::JustifyContent::flexStart;
    bannerFlexBox.alignItems = FlexBox::AlignItems::stretch;
    bannerFlexBox.flexWrap = FlexBox::Wrap::noWrap;

    bannerFlexBox.items.add(FlexItem(tempoControls).withFlex(0.5f).withMinHeight(60));
    bannerFlexBox.items.add(FlexItem(titleLabel).withFlex(0.5f).withMinHeight(60));

    // place delay line components in two rows

    FlexBox topDelays;
    topDelays.flexDirection = FlexBox::Direction::row;
    topDelays.justifyContent = FlexBox::JustifyContent::spaceAround;
    topDelays.alignItems = FlexBox::AlignItems::stretch;
    topDelays.flexWrap = FlexBox::Wrap::noWrap;

    FlexBox bottomDelays;
    bottomDelays.flexDirection = FlexBox::Direction::row;
    bottomDelays.justifyContent = FlexBox::JustifyContent::spaceAround;
    bottomDelays.alignItems = FlexBox::AlignItems::stretch;
    bottomDelays.flexWrap = FlexBox::Wrap::noWrap;

    constexpr int delayHeadsPerRow = numStereoDelayLines / 2;
    for (int i = 0; i < numStereoDelayLines; i++)
    {
        FlexBox* targetFB;
        if (i < delayHeadsPerRow)
        {
            targetFB = &topDelays;
        } else
        {
            targetFB = &bottomDelays;
        }
        targetFB->items.add(FlexItem(*delayHeadControllers[i])
                                    .withFlex(0.5f).withMinWidth(60).withMinHeight(60)
                                    .withMargin(FlexItem::Margin(10))
        );
    }


    FlexBox delays;
    delays.flexDirection = FlexBox::Direction::column;
    delays.justifyContent = FlexBox::JustifyContent::spaceAround;
    delays.alignItems = FlexBox::AlignItems::stretch;
    delays.flexWrap = FlexBox::Wrap::noWrap;

    delays.items.add(FlexItem(topDelays).withFlex(0.5f).withMinHeight(60));
    delays.items.add(FlexItem(delayRowDivider).withFlex(0.0f).withMinHeight(1).withMaxHeight(1).withMargin(10));
    delays.items.add(FlexItem(bottomDelays).withFlex(0.5f).withMinHeight(60));


    FlexBox rightPanel;
    rightPanel.flexDirection = FlexBox::Direction::column;
    rightPanel.justifyContent = FlexBox::JustifyContent::spaceAround;
    rightPanel.alignItems = FlexBox::AlignItems::stretch;
    rightPanel.flexWrap = FlexBox::Wrap::noWrap;

    rightPanel.items.add(FlexItem(outputGainKnob).withFlex(0.5f));
    rightPanel.items.add(FlexItem(mixKnob).withFlex(0.5f));

    FlexBox bottomHalf;
    bottomHalf.flexDirection = FlexBox::Direction::row;
    bottomHalf.justifyContent = FlexBox::JustifyContent::spaceBetween;
    bottomHalf.alignItems = FlexBox::AlignItems::stretch;
    bottomHalf.flexWrap = FlexBox::Wrap::noWrap;

    bottomHalf.items.add(FlexItem(delays).withMinHeight(200).withFlex(0.95f));
    bottomHalf.items.add(FlexItem(rightPanelDivider).withMinWidth(1).withMaxWidth(1).withMargin(10));
    bottomHalf.items.add(FlexItem(rightPanel).withMinHeight(200).withMinWidth(50).withFlex(0.05f));

    FlexBox uiFlexBox; // outermost
    uiFlexBox.flexDirection = FlexBox::Direction::column;
    uiFlexBox.justifyContent = FlexBox::JustifyContent::spaceBetween;
    uiFlexBox.alignItems = FlexBox::AlignItems::stretch;
    uiFlexBox.flexWrap = FlexBox::Wrap::noWrap;

    uiFlexBox.items.add(FlexItem(bannerFlexBox).withMinHeight(60).withMaxHeight(60));
    uiFlexBox.items.add(FlexItem(bannderDivider).withMinHeight(1).withMaxHeight(1).withMargin(10));
    uiFlexBox.items.add(FlexItem(bottomHalf).withFlex(1.0f));

    uiFlexBox.performLayout(getLocalBounds());

}
