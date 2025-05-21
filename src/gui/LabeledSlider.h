
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "Widgets.h"


class LabeledSlider : public juce::Component
{
public:
	explicit LabeledSlider(std::function<juce::String (double)> strValFunc);

	void resized() override;

	mimicry::Widgets::CustomTextSlider& getSlider() { return knob; }
	juce::Label& getLabel() { return label; }

private:
	juce::Label label;
	mimicry::Widgets::CustomTextSlider knob;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledSlider)
};
