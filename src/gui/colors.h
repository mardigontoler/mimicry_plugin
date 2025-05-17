
#pragma once

#include "juce_gui_basics/juce_gui_basics.h"


namespace mimicry
{
	class Colors
	{
	public:
		static juce::Colour getPrimaryColor() {
			return juce::Colour::fromRGB(0x00, 0xFF, 0x90);
		}

		static juce::Colour getDividerColor() {
			return juce::Colour::fromRGB(0x45, 0x4F, 0x52);
		}

		static juce::Colour getBgGradientCol1() {
			return juce::Colour::fromRGB(0x33, 0x33, 0x33);
		}

		static juce::Colour getBgGradientCol2() {
			return juce::Colour::fromRGB(0x16, 0x16, 0x16);
		}
	};

}
