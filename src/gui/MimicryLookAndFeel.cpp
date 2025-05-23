
#include "MimicryLookAndFeel.h"

#include "juce_gui_basics/juce_gui_basics.h"
#include "colors.h"


using namespace juce;
using namespace mimicry;

mimicry::MimicryLookAndFeel::MimicryLookAndFeel()
{
    setColour(Label::textColourId, Colours::white);
    setColour(Label::backgroundColourId, Colour::fromRGB(22, 22, 22));
    setColour(ResizableWindow::backgroundColourId, Colors::getPrimaryColor());
    setColour(Slider::rotarySliderOutlineColourId, Colors::getPrimaryColor());
    setColour(Slider::rotarySliderFillColourId, Colors::getBgGradientCol1());
    setColour(Slider::backgroundColourId, Colors::getBgGradientCol1());
    setColour(Slider::thumbColourId, Colors::getPrimaryColor());
}

Font mimicry::MimicryLookAndFeel::getLabelFont(Label& /*label*/)
{
    return Fonts::getNormalFont(15);
}

void mimicry::MimicryLookAndFeel::drawTickBox(
        Graphics& g, Component& component,
        float x, float y, float w, float h,
        bool ticked, bool isEnabled, bool /*isMouseOverButton*/, bool /*isButtonDown*/)
{
    const auto boxSize = w * 0.7f;

    const auto isDownOrDragging =
            component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

    const auto colour = component.findColour(TextButton::buttonOnColourId)
            .withMultipliedSaturation((component.hasKeyboardFocus(false) || isDownOrDragging) ? 1.3f : 0.9f)
            .withMultipliedAlpha(component.isEnabled() ? 1.0f : 0.7f);

    g.setColour(colour);

    const Rectangle<float> r(x, y + (h - boxSize) * 0.5f, boxSize, boxSize);
    g.fillRect(r);

    if (ticked)
    {
        const auto tickPath = LookAndFeel_V4::getTickShape(6.0f);
        g.setColour(isEnabled ? findColour(TextButton::buttonColourId) : Colours::grey);

        const auto transform = RectanglePlacement(RectanglePlacement::centred)
                .getTransformToFit(tickPath.getBounds(),
                                   r.reduced(r.getHeight() * 0.05f)
                );

        g.fillPath(tickPath, transform);
    }
}


void mimicry::MimicryLookAndFeel::drawLinearSlider(Graphics& g, int x, int y,
                                                   int width, int height, float sliderPos, float minSliderPos,
                                                   float maxSliderPos, const juce::Slider::SliderStyle style,
                                                   Slider& slider)
{
    if (style == juce::Slider::LinearHorizontal)
    {
        LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
        return;
    }

    float thumbSize = static_cast<float>(height) / 8.0f;
    float thumbThickness = 4.0f;
    float trackThickness = 2.0f;

    auto centreX = static_cast<float>(x) + static_cast<float>(width) * 0.5f;
//            auto centreY = static_cast<float>(y) + static_cast<float>(height) * 0.5f;

    // draw the slider track
    juce::Rectangle<float> r(centreX - (trackThickness / 2.0f), static_cast<float>(y), trackThickness,
                             static_cast<float>(height));
    g.setColour(slider.findColour(juce::Slider::backgroundColourId));
    g.drawRoundedRectangle(r, 4.0f, trackThickness);

    // redraw the track up to the current value with a darker version of the thumb color
    juce::Rectangle<float> partialTrack(
            centreX - (trackThickness / 2.0f),
            sliderPos,
            trackThickness,
            (static_cast<float>(y) + static_cast<float>(height)) - sliderPos
    );
    g.setColour(slider.findColour(Slider::thumbColourId).darker(0.75f));
    g.drawRoundedRectangle(partialTrack, 4.0f, trackThickness);

    // draw the thumb
    juce::Rectangle<float> thumbRect(
            centreX - (thumbThickness / 2.0f),
            sliderPos - (thumbSize / 2.0f),
            thumbThickness,
            thumbSize
    );
    g.setColour(slider.findColour(juce::Slider::thumbColourId));
    g.fillRoundedRectangle(thumbRect, 2.0f);

}

void mimicry::MimicryLookAndFeel::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                                   float rotaryStartAngle, float rotaryEndAngle, Slider& slider)
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
    g.drawEllipse(r, 2.0f);

    Path needle;
    auto r2 = r * 0.3f;
    r2.setWidth(4.0f);
    needle.addRoundedRectangle(r2.withPosition({r.getCentreX() - (r2.getWidth() / 2.0f), r.getY()}), 2.0f);

    g.setColour(slider.findColour(Slider::rotarySliderOutlineColourId));
    g.fillPath(needle, AffineTransform::rotation(angle, r.getCentreX(), r.getCentreY()));
}

void mimicry::MimicryLookAndFeel::drawLabel(Graphics& g, Label& label)
{
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
                         jmax(1, static_cast<int>(textArea.toFloat().getHeight() / font.getHeight())),
                         label.getMinimumHorizontalScale());

        g.setColour(label.findColour(Label::outlineColourId).withMultipliedAlpha(alpha));
    } else if (label.isEnabled())
    {
        g.setColour(label.findColour(Label::outlineColourId));
    }

    //g.drawRect(label.getLocalBounds());
}


Slider::SliderLayout MimicryLookAndFeel::getSliderLayout(Slider& slider)
{
    // copied form lookandfeel_v2, with tweaks

    auto textBoxPos = slider.getTextBoxPosition();

    // override logic for left/right textboxes, else falsee back to original impl

    const auto& name = slider.getName();

    if (slider.isRotary() && (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight))
    {

        Slider::SliderLayout layout;
        auto localBounds = slider.getLocalBounds();

        int textBoxWidth = slider.getTextBoxWidth();
        int textBoxHeight = slider.getTextBoxHeight();

        // Calculate the square area needed for the rotary part
        int availableHeight = localBounds.getHeight();
        int availableWidth = localBounds.getWidth();

        // Find the maximum size that can fit both the rotary part and the text box
        int rotarySize = jmin(availableWidth - textBoxWidth, availableHeight);

        // Center the rotary bounds
        layout.sliderBounds = localBounds.withSizeKeepingCentre(rotarySize, rotarySize);

        auto directionSign = textBoxPos == Slider::TextBoxLeft ? 1 : -1;
        auto shiftAmt = directionSign * textBoxWidth / 2;
        layout.sliderBounds.translate(shiftAmt, 0);
        layout.sliderBounds = layout.sliderBounds.getIntersection(localBounds);

        auto textBoxX = textBoxPos == Slider::TextBoxLeft ?
                        layout.sliderBounds.getX() - textBoxWidth
                                                          : layout.sliderBounds.getRight();

        layout.textBoxBounds = localBounds
                .withSizeKeepingCentre(textBoxWidth, textBoxHeight)
                .withX(textBoxX);

        return layout;

    } else
    {
        return LookAndFeel_V2::getSliderLayout(slider);
    }

}
