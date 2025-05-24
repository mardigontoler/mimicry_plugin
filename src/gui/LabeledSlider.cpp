
#include "LabeledSlider.h"

#include <utility>

LabeledSlider::LabeledSlider(std::function<juce::String(double)> strValFunc)
        : knob(std::move(strValFunc))
{
    // Setup label
    label.setText("", juce::dontSendNotification);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);

    // Setup knob
    knob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    knob.setDoubleClickReturnValue(true, 0.0);
    addAndMakeVisible(knob);
}

void LabeledSlider::resized()
{
    juce::GlyphArrangement glyphs;
    glyphs.addLineOfText(label.getFont(), label.getText(), 0.0f, 0.0f);
    auto labelHeight = label.getFont().getHeight() + 4;

    auto bounds = getLocalBounds().toFloat();
    auto knobBounds = bounds.reduced(0.0f, labelHeight);
    knob.setBounds(knobBounds.toNearestInt());

    if (knob.getName() == "Output Gain")
    {
        DBG("Here");
    }
    const auto renderedKnobBounds = getLookAndFeel().getSliderLayout(knob).sliderBounds.toFloat();

    auto labelBounds = renderedKnobBounds.withHeight(labelHeight);
    labelBounds.translate(0.0f, -labelHeight);

    label.setBounds(labelBounds.toNearestInt());

}
