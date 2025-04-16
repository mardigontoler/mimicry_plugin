

#pragma once

#include "MimicryLookAndFeel.h"

namespace mimicry {
    class WhiteLabelLookAndFeel : public MimicryLookAndFeel {
    public:
        WhiteLabelLookAndFeel() {
            setColour(Label::textColourId, Colour::fromRGB(196, 196, 196));
        }
    };
}
