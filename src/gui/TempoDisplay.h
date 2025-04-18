
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"

/**
* Simple component that's just a rectange with a label.
*/
class TempoDisplay : public juce::Component, public juce::Slider::Listener
{
public:
    TempoDisplay();

    void paint(juce::Graphics& g) override;

    void resized() override;

    void sliderValueChanged(juce::Slider* slider) override;

private:
    juce::Label tempoLabel;
    juce::Label glowLabel;

//    GlowEffect glowEffect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoDisplay)
};
