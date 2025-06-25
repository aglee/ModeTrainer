#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <map>
#include <vector>
#include <functional>

class AudioEngine
{
public:
    enum class ModeType
    {
        Ionian,
        Dorian,
        Phrygian,
        Lydian,
        Mixolydian,
        Aeolian,
        Locrian
    };
    
    enum class PlaybackPattern
    {
        Ascending,
        Descending,
        Intervallic,  // 1 3 2 4 3 5 4 6 5 7 6 1+ 7 2+ 1+
        IntervallicDescending,  // 8 6 7 5 6 4 5 3 4 2 3 1 2 7- 1
        Random
    };

    AudioEngine();

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();

    void playMode(ModeType mode, float rootFrequency, PlaybackPattern pattern = PlaybackPattern::Ascending);
    void stopPlaying();
    
    void setPlaybackSpeed(float speed); // 0.5 to 2.0, where 1.0 is normal speed
    void setPlaybackPattern(PlaybackPattern pattern);
    
    juce::String getPatternName(PlaybackPattern pattern) const;
    std::vector<PlaybackPattern> getAllPatterns() const;
    
    std::function<void()> onPlaybackFinished;

    bool isCurrentlyPlaying() const;

    juce::String getModeName(ModeType mode) const;
    std::vector<ModeType> getAllModes() const;

private:
    double currentSampleRate;
    double currentAngle;
    double angleDelta;
    bool isPlaying;

    int currentNoteIndex;
    float noteDuration;
    int samplesSinceNoteStart;

    ModeType currentMode;
    PlaybackPattern currentPattern;
    std::vector<float> currentScale;
    std::vector<int> playbackOrder;  // Indices for the order to play notes
    std::map<ModeType, std::vector<int>> modes;
    std::map<ModeType, juce::String> modeNames;
    std::map<PlaybackPattern, juce::String> patternNames;
    juce::Random random;

    void playNextNote();
    float calculateEnvelope();
};
