
#pragma once

#include "WhiteLabelLookAndFeel.h"
#include "juce_gui_basics/juce_gui_basics.h"
using namespace juce;

namespace mimicry {
    class DelayControllerLookAndFeel : public WhiteLabelLookAndFeel {
    private:
        Font getLabelFont(Label& label) override
        {
            return mimicry::getNormalFont(12);
        }

    };
}
