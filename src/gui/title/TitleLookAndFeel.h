
#pragma once


#include "juce_gui_basics/juce_gui_basics.h"
#include "../font.h"
#include "../MimicryLookAndFeel.h"

class TitleLookAndFeel: public juce::LookAndFeel_V4 {


    juce::Font getLabelFont(juce::Label& label) override
    {
        return mimicry::getHeaderFont(50);
    }

};


class SubTitleLookAndFeel : public mimicry::MimicryLookAndFeel {

    juce::Font getLabelFont(juce::Label& label) override
    {
        return mimicry::getNormalFont(12);
    }
};