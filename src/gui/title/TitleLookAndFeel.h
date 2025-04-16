
#pragma once


#include "juce_gui_basics/juce_gui_basics.h"
#include "../font.h"
#include "../MimicryLookAndFeel.h"

using namespace juce;
using namespace mimicry;

class TitleLookAndFeel: public LookAndFeel_V4 {


    Font getLabelFont(Label& label) override {
        return mimicry::getHeaderFont(50);
    }

};


class SubTitleLookAndFeel : public MimicryLookAndFeel {
private:
    Font getLabelFont(Label& label) override
    {
        return mimicry::getNormalFont(12);
    }
};