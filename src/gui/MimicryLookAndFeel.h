

#pragma once

#include "font.h"
#include "juce_gui_basics/juce_gui_basics.h"

namespace mimicry {

    class MimicryLookAndFeel : public juce::LookAndFeel_V4 {


    public:

        juce::Colour MIMICRY_GRAY = juce::Colour::fromRGB(196,196,196);

        MimicryLookAndFeel();

        juce::Font getLabelFont(juce::Label& label) override;


        //        void drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour,
//            bool isMouseOverButton, bool isButtonDown) override
//        {
//            auto baseColour = backgroundColour.withMultipliedSaturation(button.hasKeyboardFocus(true) ? 1.3f : 0.9f)
//                .withMultipliedAlpha(button.isEnabled() ? 0.9f : 0.5f);
//
//            if (isButtonDown || isMouseOverButton)
//                baseColour = baseColour.contrasting(isButtonDown ? 0.2f : 0.1f);
//
//            auto width = button.getWidth() - 1.0f;
//            auto height = button.getHeight() - 1.0f;
//
//            if (width > 0 && height > 0)
//            {
//                g.setGradientFill(ColourGradient::vertical(baseColour, 0.0f,
//                    baseColour.darker(0.1f), height));
//
//                g.fillRect(button.getLocalBounds());
//            }
//        }

        void drawTickBox(juce::Graphics& g, juce::Component& component,
            float x, float y, float w, float h,
            bool ticked,
            bool isEnabled,
            bool isMouseOverButton,
            bool isButtonDown) override;


        int getSliderThumbRadius(juce::Slider& /*slider*/) override{
            return thumbRadius;
        }

//        void setThumbRadius(int radius){
//            thumbRadius = radius;
//        }


        void drawLinearSlider (juce::Graphics& g, int x, int y, int width, int height,
                float sliderPos, float /*minSliderPos*/, float /*maxSliderPos*/,
                const juce::Slider::SliderStyle style, juce::Slider& slider) override;



        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
            float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;


        void drawLabel(juce::Graphics& g, juce::Label& label) override;

    private:
        int thumbRadius = 15;

    };

}
