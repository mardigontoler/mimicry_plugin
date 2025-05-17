
#include "MimicryLookAndFeel.h"

#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

mimicry::MimicryLookAndFeel::MimicryLookAndFeel()
{
	setColour(Label::textColourId, Colour::fromRGB(30, 142, 177));
	setColour(Label::backgroundColourId, Colour::fromRGB(22,22,22));
	setColour(ResizableWindow::backgroundColourId, Colour::fromRGB(6, 7, 14));
	setColour(Slider::rotarySliderOutlineColourId, Colour::fromRGB(30, 177, 175));
	setColour(Slider::rotarySliderFillColourId, Colour::fromRGB(62,71,73));
	setColour(Slider::backgroundColourId, Colour::fromRGB(62,71,73));
	setColour(Slider::thumbColourId, Colour::fromRGB(30,142,177));
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

	const auto isDownOrDragging = component.isEnabled() && (component.isMouseOverOrDragging() || component.isMouseButtonDown());

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
