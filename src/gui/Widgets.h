#pragma once

#include <utility>

#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry::Widgets
{

    class CustomTextSlider : public juce::Slider
    {
    public:
        explicit CustomTextSlider(std::function<juce::String(double)> strValFunc)
                : mStrValFunc(std::move(strValFunc))
        {}

        ~CustomTextSlider() override = default;

        juce::String getTextFromValue(double value) override
        { return mStrValFunc(value); }

    private:
        std::function<juce::String(double)> mStrValFunc;
    };

}
