
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

/**
* Simple component that's just a rectange with a label.
*/
class TempoDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    TempoDisplay() {
        tempoLabel.setText("120", juce::NotificationType::dontSendNotification);
        glowLabel.setText("120", juce::NotificationType::dontSendNotification);
        tempoLabel.setJustificationType(juce::Justification::right);
        glowLabel.setJustificationType(juce::Justification::right);
        tempoLabel.setMinimumHorizontalScale(1.0f);
        glowLabel.setMinimumHorizontalScale(1.0f);

        addAndMakeVisible(tempoLabel);
        addAndMakeVisible(glowLabel, 1);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(getLookAndFeel().findColour(juce::Label::backgroundColourId));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), 5);
    }

    void resized() override {
        using namespace juce;
//        tempoLabel.setBounds(getLocalBounds());
        Rectangle<int> localBounds = getLocalBounds();
        Rectangle<int> textBoxBounds(
                localBounds.getX() - 5,
                localBounds.getY() + 5,
                localBounds.getWidth(),
                localBounds.getHeight()
                );
        Rectangle<int> glowTextBoxBounds = textBoxBounds;
        glowTextBoxBounds.translate(-2, -2);

        Colour mainColour = tempoLabel.findColour(Label::textColourId, false);
        Colour glowLabelColour = mainColour.withAlpha(0.4f);
        glowLabel.setColour(Label::textColourId, glowLabelColour);

        tempoLabel.setBounds(textBoxBounds);
        glowLabel.setBounds(glowTextBoxBounds);

    }

    void sliderValueChanged(juce::Slider* slider) override{
        using namespace juce;
        int tempo = (int)slider->getValue();
        String str = String(tempo);
        tempoLabel.setText(str, dontSendNotification);
        glowLabel.setText(str, dontSendNotification);
    }


private:
    juce::Label tempoLabel;
    juce::Label glowLabel;

//    GlowEffect glowEffect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoDisplay)
};
