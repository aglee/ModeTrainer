#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioEngine.h"

// Custom LookAndFeel for Light Mode
class LightModeLookAndFeel : public juce::LookAndFeel_V4 {
public:
    LightModeLookAndFeel() {
        setColour(juce::ResizableWindow::backgroundColourId, juce::Colours::white);
        setColour(juce::Label::textColourId, juce::Colours::black);
        setColour(juce::TextButton::buttonColourId, juce::Colours::white);
        setColour(juce::TextButton::textColourOnId, juce::Colours::black);
        setColour(juce::TextButton::textColourOffId, juce::Colours::black);
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::white);
        setColour(juce::ComboBox::textColourId, juce::Colours::black);
        setColour(juce::ComboBox::outlineColourId, juce::Colours::darkgrey);
        setColour(juce::ComboBox::buttonColourId, juce::Colours::lightgrey);
        setColour(juce::ToggleButton::textColourId, juce::Colours::black);
        setColour(juce::ToggleButton::tickColourId, juce::Colours::darkblue);
        setColour(juce::ToggleButton::tickDisabledColourId, juce::Colours::grey);
		setColour(juce::Slider::backgroundColourId, juce::Colours::lightgrey);
        setColour(juce::Slider::thumbColourId, juce::Colours::darkblue);
        setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);
        setColour(juce::Slider::rotarySliderFillColourId, juce::Colours::darkblue);
        setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colours::darkgrey);
        setColour(juce::Slider::textBoxTextColourId, juce::Colours::black);
        setColour(juce::Slider::textBoxBackgroundColourId, juce::Colours::white);
        setColour(juce::Slider::textBoxHighlightColourId, juce::Colours::lightblue);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::darkgrey);
    }

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

class MainComponent  : public juce::AudioAppComponent
{
public:
    MainComponent();
    ~MainComponent() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    
    void showAboutDialog();
    

private:
    AudioEngine audioEngine;
    juce::Random random;
	const juce::String instructionsText = "Click \"Play Random Scale\" to test your knowledge, or click any mode button to hear that scale.";

    // Game state
    int score;
    int totalQuestions;
    AudioEngine::ModeType currentMode;
    AudioEngine::ModeType lastPlayedMode;
    bool gameActive;
    bool isPracticeMode;

    // UI Components
    juce::TextButton playButton;
    juce::TextButton stopButton;
    juce::TextButton aboutButton;
    std::vector<std::unique_ptr<juce::TextButton>> modeButtons;
    std::vector<AudioEngine::ModeType> modeOrder; // Current order of modes in buttons
    
    juce::Label titleLabel;
    juce::Label scoreLabel;
    juce::Label statusLabel;  // Combined status/feedback label
    juce::Label rootNoteLabel;
    juce::Label rootSelectionLabel;
    juce::Label speedLabel;
    juce::Label patternLabel;
	juce::Label modeButtonsLabel;
	juce::Label colorsLabel;
    juce::Label optionsLabel;
    
    juce::Slider rootNoteSlider;
    juce::Slider speedSlider;
    juce::ComboBox patternComboBox;
    juce::ToggleButton randomizeModeButtonsCheckbox;
    juce::ToggleButton randomizeRootCheckbox;
    juce::ToggleButton lightModeToggle;
    
    // Custom LookAndFeel
    LightModeLookAndFeel lightModeLookAndFeel;

    // Methods
    void playRandomScale();
    void stopPlaying();
    void guessMode(AudioEngine::ModeType guessedMode);
	void practiceMode(AudioEngine::ModeType mode);
	void showInstructionsText();
    AudioEngine::PlaybackPattern getSelectedPattern() const;
    void randomizeButtonOrder();
    juce::String frequencyToNoteName(double frequency) const;
    double noteNameToFrequency(int noteIndex) const;
    int frequencyToNoteIndex(double frequency) const;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
