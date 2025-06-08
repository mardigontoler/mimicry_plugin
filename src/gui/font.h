

#pragma once

#include "BinaryData.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry
{
    class Fonts
    {
    public:
        static juce::Font getLcdFont(float heigth)
        {
            static juce::Font font((juce::FontOptions(
                    juce::Typeface::createSystemTypefaceFor(BinaryData::DigitalNumbersRegular_ttf,
                                                            BinaryData::DigitalNumbersRegular_ttfSize))));
            return font.withHeight(heigth);
        }

        static juce::Font getNormalFont(float height)
        {
            static juce::Font font((juce::FontOptions(juce::Typeface::createSystemTypefaceFor(BinaryData::InterDisplayBlack_ttf,
                                                                                       BinaryData::InterDisplayBlack_ttfSize))));

            return font.withHeight(height);;
        }
    };
}
