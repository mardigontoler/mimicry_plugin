
#pragma once

#include "juce_audio_processors/juce_audio_processors.h"
#include "DigitalLookAndFeel.h"

/**
* Simple component that's just a rectange with a label.
*/
class TempoDisplay : public juce::Component, public juce::Slider::Listener, public juce::AudioProcessorValueTreeState::Listener
{
public:
    explicit TempoDisplay(juce::AudioProcessorValueTreeState* vts);

    ~TempoDisplay() override;

    void paint(juce::Graphics& g) override;

    void resized() override;

    void SetText(juce::String text);

    void sliderValueChanged(juce::Slider* slider) override;

    void TempoSyncChanged(bool syncActive);

    void parameterChanged(const juce::String&parameterID, float newValue) override;

private:
	mimicry::DigitalLookAndFeel digitalLAF;
    juce::AudioProcessorValueTreeState* mValueTreeState;
    juce::String mText;
    bool mSyncActive{false};
    juce::Label tempoLabel;
    juce::Label bgTextLabel;

//    GlowEffect glowEffect;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TempoDisplay)
};
