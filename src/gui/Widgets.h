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


    class DividerLine : public juce::Component
    {
    public:
        explicit DividerLine(bool isVertical = false)
                : vertical(isVertical)
        {
            setColour(juce::Colours::grey);
        }

        void setColour(juce::Colour newColour)
        {
            colour = newColour;
            repaint();
        }

        void paint(juce::Graphics& g) override
        {
            g.setColour(colour);
            if (vertical)
                g.fillRect(getLocalBounds().withWidth(1));
            else
                g.fillRect(getLocalBounds().withHeight(1));
        }

    private:
        bool vertical;
        juce::Colour colour;
    };

}
