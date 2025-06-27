#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
	
	virtual juce::Colour getInstructionsColour() = 0;
	virtual juce::Colour getPlayingForPracticeColour() = 0;
	virtual juce::Colour getPlayingForGuessColour() = 0;
	virtual juce::Colour getWaitingForGuessColour() = 0;
	virtual juce::Colour getCorrectGuessColour() = 0;
	virtual juce::Colour getIncorrectGuessColour() = 0;
	
	// MARK: - (LookAndFeel)
	
	juce::Font getComboBoxFont(juce::ComboBox&) override
	{
		return juce::Font(14.0f);
	}
	
	juce::Font getPopupMenuFont() override
	{
		return juce::Font(14.0f);
	}
	
	void getIdealPopupMenuItemSize(const juce::String& text, bool isSeparator, int standardMenuItemHeight,
								   int& idealWidth, int& idealHeight) override
	{
		LookAndFeel_V4::getIdealPopupMenuItemSize(text, isSeparator, standardMenuItemHeight, idealWidth, idealHeight);
		idealHeight = 22;
	}
};

class LightModeLookAndFeel : public CustomLookAndFeel {
public:
    LightModeLookAndFeel() {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::lightgrey);

        setColour(juce::Label::textColourId, juce::Colours::black);

        setColour(juce::TextButton::buttonColourId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        setColour(juce::TextButton::textColourOffId, juce::Colours::black);

        setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour(juce::ComboBox::textColourId, juce::Colours::black);
        setColour(juce::ComboBox::arrowColourId, juce::Colours::black);
        setColour(juce::ComboBox::outlineColourId, juce::Colours::darkgrey);
        setColour(juce::ComboBox::buttonColourId, juce::Colours::lightgrey);

        setColour(juce::ToggleButton::textColourId, juce::Colours::black);
        setColour(juce::ToggleButton::tickColourId, juce::Colours::black);
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);

        setColour(juce::Slider::backgroundColourId, juce::Colour(0xffA8A8A8));
        setColour(juce::Slider::thumbColourId, juce::Colours::darkblue);
        setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkblue);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::lightblue);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::darkgrey);
        
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::white);
        setColour(juce::PopupMenu::textColourId, juce::Colours::black);
        setColour(juce::PopupMenu::headerTextColourId, juce::Colours::black);
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colours::lightblue);
        setColour(juce::PopupMenu::highlightedTextColourId, juce::Colours::black);
    }
	
	// MARK: - (CustomLookAndFeel)
	
	juce::Colour getInstructionsColour() override { return juce::Colours::blue; }
	juce::Colour getPlayingForPracticeColour() override { return juce::Colours::blue; }
	juce::Colour getPlayingForGuessColour() override { return juce::Colours::blue; }
	juce::Colour getWaitingForGuessColour() override { return juce::Colours::blue; }
	juce::Colour getCorrectGuessColour() override { return juce::Colours::green; }
	juce::Colour getIncorrectGuessColour() override { return juce::Colours::orangered; }
};

class DarkModeLookAndFeel : public CustomLookAndFeel {
public:
	DarkModeLookAndFeel() {
	}
	
	// MARK: - (CustomLookAndFeel)
	
	juce::Colour getInstructionsColour() override { return juce::Colours::lightblue; }
	juce::Colour getPlayingForPracticeColour() override { return juce::Colours::lightblue; }
	juce::Colour getPlayingForGuessColour() override { return juce::Colours::lightblue; }
	juce::Colour getWaitingForGuessColour() override { return juce::Colours::lightblue; }
	juce::Colour getCorrectGuessColour() override { return juce::Colours::lightgreen; }
	juce::Colour getIncorrectGuessColour() override { return juce::Colours::orangered; }
};

