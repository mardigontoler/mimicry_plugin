

#pragma once

#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry {
    static juce::Font getSegmentFont(float heigth) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Segment74Gml_otf, BinaryData::Segment74Gml_otfSize);
        juce::Font font((juce::FontOptions(typeface)));
        font.setHeight(heigth);
        return font;
    }

    static juce::Font getHeaderFont(float height) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::CinzelDecorativeRegular_ttf, BinaryData::CinzelDecorativeRegular_ttfSize);
		juce::Font font((juce::FontOptions(typeface)));
        font.setHeight(height);
        return font;
    }

    static juce::Font getNormalFont(float height) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuMedium_ttf, BinaryData::UbuntuMedium_ttfSize);
		juce::Font font((juce::FontOptions(typeface)));
        font.setHeight(height);
        return font;
    }
}
