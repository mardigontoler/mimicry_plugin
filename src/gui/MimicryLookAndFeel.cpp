
#include "MimicryLookAndFeel.h"

#include "juce_gui_basics/juce_gui_basics.h"

using namespace juce;

mimicry::MimicryLookAndFeel::MimicryLookAndFeel()
{
	setColour(juce::Label::textColourId, Colour::fromRGB(30, 142, 177));
	setColour(Label::backgroundColourId, Colour::fromRGB(22,22,22));
	setColour(ResizableWindow::backgroundColourId, Colour::fromRGB(6, 7, 14));
	setColour(Slider::rotarySliderOutlineColourId, Colour::fromRGB(30, 177, 175));
	setColour(Slider::rotarySliderFillColourId, Colour::fromRGB(62,71,73));
	setColour(Slider::backgroundColourId, Colour::fromRGB(62,71,73));
	setColour(Slider::thumbColourId, Colour::fromRGB(30,142,177));
}
