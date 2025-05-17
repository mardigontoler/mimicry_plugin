
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"

class LabeledSlider : public juce::Component
{
public:
	explicit LabeledSlider(const juce::String& labelText);

	void resized() override;

	juce::Slider& getSlider() { return knob; }
	juce::Label& getLabel() { return label; }

private:
	juce::Label label;
	juce::Slider knob;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledSlider)
};
