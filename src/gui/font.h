

#pragma once

#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry {
    static const juce::Font getSegmentFont(int heigth) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::Segment74Gml_otf, BinaryData::Segment74Gml_otfSize);
        juce::Font font(typeface);
        font.setHeight(heigth);
        return font;
    }

    static const juce::Font getHeaderFont(int height) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::CinzelDecorativeRegular_ttf, BinaryData::CinzelDecorativeRegular_ttfSize);
        juce::Font font(typeface);
        font.setHeight(height);
        return font;
    }

    static const juce::Font getNormalFont(int height) {
        static auto typeface = juce::Typeface::createSystemTypefaceFor(BinaryData::UbuntuMedium_ttf, BinaryData::UbuntuMedium_ttfSize);
        juce::Font font(typeface);
        font.setHeight(height);
        return font;
    }
}
