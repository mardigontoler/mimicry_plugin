

#pragma once

#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry {
	class Fonts{
	public:
		static juce::Font getSegmentFont(float heigth) {
			static juce::Font font((juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::Segment74Gml_otf, BinaryData::Segment74Gml_otfSize))));
			return font.withHeight(heigth);
		}

//		static juce::Font getHeaderFont(float height) {
//			static juce::Font font((juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::CinzelDecorativeRegular_ttf, BinaryData::CinzelDecorativeRegular_ttfSize))));
//			return font.withHeight(height);
//		}

		static juce::Font getNormalFont(float height) {
			juce::Font font((juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::InterVariable_ttf, BinaryData::InterVariable_ttfSize))));
			return font.withHeight(height);;
		}
	};
}
