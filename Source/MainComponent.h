#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "AudioEngine.h"

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
    
    juce::Label instructionLabel;
    juce::Label scoreLabel;
    juce::Label statusLabel;  // Combined status/feedback label
    juce::Label rootNoteLabel;
    juce::Label rootSelectionLabel;
    juce::Label speedLabel;
    juce::Label patternLabel;
    juce::Label modeButtonsLabel;
    
    juce::Slider rootNoteSlider;
    juce::Slider speedSlider;
    juce::ComboBox patternComboBox;
    juce::ToggleButton randomizeButtonsCheckbox;
    juce::ToggleButton randomizeRootPitchCheckbox;

    // Methods
    void playRandomMode();
    void stopPlaying();
    void guessMode(AudioEngine::ModeType guessedMode);
    void practiceMode(AudioEngine::ModeType mode);
    AudioEngine::PlaybackPattern getSelectedPattern() const;
    void randomizeButtonOrder();
    juce::String frequencyToNoteName(double frequency) const;
    double noteNameToFrequency(int noteIndex) const;
    int frequencyToNoteIndex(double frequency) const;
    

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
