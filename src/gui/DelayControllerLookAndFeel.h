
#pragma once

#include "WhiteLabelLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry
{
    class DelayControllerLookAndFeel : public WhiteLabelLookAndFeel
    {

        juce::Font getLabelFont(juce::Label& /*label*/) override
        {
            return Fonts::getNormalFont(12);
        }

    };
}
