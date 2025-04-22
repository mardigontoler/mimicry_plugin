
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "MimicryLookAndFeel.h"

namespace mimicry
{
	class DigitalLookAndFeel : public MimicryLookAndFeel {
		juce::Font getLabelFont(juce::Label &label) override
		{
			return getSegmentFont(50);
		}

	public:
		DigitalLookAndFeel()
		{
			setColour(juce::Label::textColourId, juce::Colour::fromRGBA(45, 255, 180, 150));
			setColour(juce::Label::backgroundColourId, juce::Colours::black);
		}
	};
}
