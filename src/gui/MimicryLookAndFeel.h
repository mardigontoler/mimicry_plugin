

#pragma once

#include "font.h"
#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

namespace mimicry {

    class MimicryLookAndFeel : public LookAndFeel_V4 {


    public:

        Colour MIMICRY_GRAY = Colour::fromRGB(196,196,196);

        MimicryLookAndFeel() {
            setColour(Label::textColourId, Colour::fromRGB(30, 142, 177));
            setColour(Label::backgroundColourId, Colour::fromRGB(22,22,22));
            setColour(ResizableWindow::backgroundColourId, Colour::fromRGB(6, 7, 14));
            setColour(Slider::rotarySliderOutlineColourId, Colour::fromRGB(30, 177, 175));
            setColour(Slider::rotarySliderFillColourId, Colour::fromRGB(62,71,73));
            setColour(Slider::backgroundColourId, Colour::fromRGB(62,71,73));
            setColour(Slider::thumbColourId, Colour::fromRGB(30,142,177));
        }

        Font getLabelFont(Label& label) override
        {
            return mimicry::getNormalFont(15);
        }


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

        void drawTickBox(Graphics& g, Component& component,
            float x, float y, float w, float h,
            bool ticked,
            bool isEnabled,
            bool isMouseOverButton,
            bool isButtonDown) override
        {
            auto boxSize = w * 0.7f;

            auto isDownOrDragging = component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

            auto colour = component.findColour(TextButton::buttonOnColourId)
                .withMultipliedSaturation((component.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
                .withMultipliedAlpha(component.isEnabled() ? 1.0f : 0.7f);

            g.setColour(colour);

            Rectangle<float> r(x, y + (h - boxSize) * 0.5f, boxSize, boxSize);
            g.fillRect(r);

            if (ticked)
            {
                auto tickPath = LookAndFeel_V4::getTickShape(6.0f);
                g.setColour(isEnabled ? findColour(TextButton::buttonColourId) : Colours::grey);

                auto transform = RectanglePlacement(RectanglePlacement::centred)
                    .getTransformToFit(tickPath.getBounds(),
                        r.reduced(r.getHeight() * 0.05f));

                g.fillPath(tickPath, transform);
            }
        }



        int getSliderThumbRadius(Slider& slider) override{
            return thumbRadius;
        }

        void setThumbRadius(int radius){
            thumbRadius = radius;
        }


        void drawLinearSlider (Graphics& g, int x, int y, int width, int height,
                float sliderPos, float minSliderPos, float maxSliderPos,
                const Slider::SliderStyle style, Slider& slider) override
        {

            float thumbSize = 30.0f;
            float thumbThickness = 13.0f;
            float trackThickness = 5.0f;

            auto centreX = x + width * 0.5f;
            auto centreY = y + height * 0.5f;

            if (style == Slider::LinearVertical) {
                // draw the slider track
                Rectangle<float> r(centreX - (trackThickness / 2.0f), y, trackThickness, height);
                g.setColour(slider.findColour(Slider::backgroundColourId));
                g.drawRoundedRectangle(r, 4.0f, trackThickness);

                // draw the thumb and notch
                Rectangle<float> thumbRect(
                        centreX - (thumbThickness / 2.0f),
                        sliderPos - (thumbSize / 2.0f),
                        thumbThickness,
                        thumbSize
                        );
                g.setColour(slider.findColour(Slider::thumbColourId));
                g.fillRoundedRectangle(thumbRect, 2.0f);

                Rectangle<float> notchRect(
                        centreX - (thumbThickness / 2.0f),
                        sliderPos,
                        thumbThickness,
                        1.0f
                        );
                g.setColour(MIMICRY_GRAY);
                g.fillRect(notchRect);

            }
            else if(style == Slider::LinearHorizontal){
                // todo if needed
            }

        }



        void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
            float rotaryStartAngle, float rotaryEndAngle, Slider& slider) override
        {
            auto diameter = jmin(width, height) - 4.0f;
            auto radius = (diameter / 2.0f) * std::cos(MathConstants<float>::pi / 4.0f);
            auto centreX = x + width * 0.5f;
            auto centreY = y + height * 0.5f;
            auto rx = centreX - radius;
            auto ry = centreY - radius;
            auto rw = radius * 2.0f;
            auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
            bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

            auto baseColour = slider.isEnabled() ? slider.findColour(Slider::rotarySliderFillColourId).withAlpha(isMouseOver ? 0.8f : 1.0f)
                : Colour(0x80808080);

            Rectangle<float> r(rx, ry, rw, rw);

            auto x1 = r.getTopLeft().getX();
            auto y1 = r.getTopLeft().getY();
            auto x2 = r.getBottomLeft().getX();
            auto y2 = r.getBottomLeft().getY();

            // draw background circle
            g.setColour(slider.findColour(Slider::rotarySliderFillColourId));
            g.fillEllipse(r);

            // draw the outline and needle
            g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
            g.drawEllipse(r, 3);

            Path needle;
            auto r2 = r * 0.3f;
            r2.setWidth(4);
            needle.addRoundedRectangle(r2.withPosition({ r.getCentreX() - (r2.getWidth() / 2.0f), r.getY() }), 2.0f);

            g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
            g.fillPath(needle, AffineTransform::rotation(angle, r.getCentreX(), r.getCentreY()));
        }


        void drawLabel(Graphics& g, Label& label) override {
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
                    jmax(1, (int)(textArea.getHeight() / font.getHeight())),
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
