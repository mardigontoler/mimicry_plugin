

#pragma once

#include "MimicryLookAndFeel.h"

namespace mimicry {
    class WhiteLabelLookAndFeel : public MimicryLookAndFeel {
    public:
        WhiteLabelLookAndFeel() {
            using namespace juce;
            setColour(Label::textColourId, Colour::fromRGB(196, 196, 196));
        }
    };
}
