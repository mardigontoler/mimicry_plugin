
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"
#include "MimicryLookAndFeel.h"
#include "colors.h"

namespace mimicry
{
	class DigitalLookAndFeel : public MimicryLookAndFeel {
		juce::Font getLabelFont(juce::Label & /*label*/) override
		{
			return Fonts::getSegmentFont(50);
		}

	public:
		DigitalLookAndFeel()
		{
			setColour(juce::Label::textColourId, mimicry::Colors::getPrimaryColor());
			setColour(juce::Label::backgroundColourId, juce::Colours::black);
		}
	};
}
