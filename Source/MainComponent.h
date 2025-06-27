#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioEngine.h"
#include "CustomLookAndFeel.h"

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
	
	enum class GameStatus : int {
		instructions,
		playingForPractice,
		playingForGuess,
		waitingForGuess,
		correctGuess,
		incorrectGuess
	};
	
    // Game state
	GameStatus gameStatus;
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
	DarkModeLookAndFeel darkModeLookAndFeel;
	std::reference_wrapper<CustomLookAndFeel> currentLookAndFeel = lightModeLookAndFeel;
	void setCustomLookAndFeel(CustomLookAndFeel &lookAndFeel) {
		setLookAndFeel(&lookAndFeel);
		currentLookAndFeel = lookAndFeel;
		updateStatusLabelColour();
	}

    // Methods
	void setStatusWithText(GameStatus status, juce::String text);
	void updateStatusLabelColour();
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
