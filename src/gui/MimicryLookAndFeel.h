

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
                const juce::Slider::SliderStyle style, juce::Slider& slider) override
        {

            float thumbSize = 30.0f;
            float thumbThickness = 13.0f;
            float trackThickness = 5.0f;

            auto centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
//            auto centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;

            if (style == juce::Slider::LinearVertical) {
                // draw the slider track
                juce::Rectangle<float> r(centreX - (trackThickness / 2.0f), static_cast<float>(y), trackThickness, static_cast<float>(height));
                g.setColour(slider.findColour(juce::Slider::backgroundColourId));
                g.drawRoundedRectangle(r, 4.0f, trackThickness);

                // draw the thumb and notch
                juce::Rectangle<float> thumbRect(
                        centreX - (thumbThickness / 2.0f),
                        sliderPos - (thumbSize / 2.0f),
                        thumbThickness,
                        thumbSize
                        );
                g.setColour(slider.findColour(juce::Slider::thumbColourId));
                g.fillRoundedRectangle(thumbRect, 2.0f);

                juce::Rectangle<float> notchRect(
                        centreX - (thumbThickness / 2.0f),
                        sliderPos,
                        thumbThickness,
                        1.0f
                        );
                g.setColour(MIMICRY_GRAY);
                g.fillRect(notchRect);

            }
            else if(style == juce::Slider::LinearHorizontal){
                // todo if needed
            }

        }



        void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
            float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
        {
            using namespace juce;
            auto diameter = static_cast<float>(jmin(width, height)) - 4.0f;
            auto radius = (diameter / 2.0f) * std::cos(MathConstants<float>::pi / 4.0f);
            auto centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
            auto centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;
            auto rx = centreX - radius;
            auto ry = centreY - radius;
            auto rw = radius * 2.0f;
            auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
//            bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

//            auto baseColour = slider.isEnabled() ? slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 0.8f : 1.0f)
//                : Colour(0x80808080);

            Rectangle<float> r(rx, ry, rw, rw);

//            auto x1 = r.getTopLeft().getX();
//            auto y1 = r.getTopLeft().getY();
//            auto x2 = r.getBottomLeft().getX();
//            auto y2 = r.getBottomLeft().getY();

            // draw background circle
            g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
            g.fillEllipse(r);

            // draw the outline and needle
            g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
            g.drawEllipse(r, 3.0f);

            Path needle;
            auto r2 = r * 0.3f;
            r2.setWidth(4.0f);
            needle.addRoundedRectangle(r2.withPosition({ r.getCentreX() - (r2.getWidth() / 2.0f), r.getY() }), 2.0f);

            g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
            g.fillPath(needle, AffineTransform::rotation(angle, r.getCentreX(), r.getCentreY()));
        }


        void drawLabel(juce::Graphics& g, juce::Label& label) override {
            using namespace juce;
            //g.fillAll(label.findColour(Label::backgroundColourId));
            //g.fillRoundedRectangle(g.getClipBounds().toFloat(), 5);
            //g.fillRect(g.getClipBounds().toFloat());
            if (!label.isBeingEdited())
            {
                auto alpha = label.isEnabled() ? 1.0f : 0.5f;
                const Font font(getLabelFont(label));

                g.setColour(label.findColour(Label::textColourId).withMultipliedAlpha(alpha));
                g.setFont(font);

                auto textArea = getLabelBorderSize(label).subtractedFrom(label.getLocalBounds());

                g.drawFittedText(label.getText(), textArea, label.getJustificationType(),
                    jmax(1, static_cast<int>(textArea.getHeight() / font.getHeight())),
                    label.getMinimumHorizontalScale());

                g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
            }
            else if (label.isEnabled())
            {
                g.setColour(label.findColour(Label::outlineColourId));
            }

            //g.drawRect(label.getLocalBounds());
        }

    private:
        int thumbRadius = 15;

    };

}
