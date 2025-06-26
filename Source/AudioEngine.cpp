#include "AudioEngine.h"
#include <cmath>
#include <juce_events/juce_events.h>

AudioEngine::AudioEngine()
    : currentSampleRate(44100.0)
    , currentAngle(0.0)
    , angleDelta(0.0)
    , isPlaying(false)
    , currentNoteIndex(0)
    , noteDuration(0.5f)
    , samplesSinceNoteStart(0)
    , currentPattern(PlaybackPattern::Ascending)
    , random(juce::Time::currentTimeMillis())
{
    // Initialize the modes with their interval patterns (in semitones from root)
    modes[ModeType::Ionian] = {0, 2, 4, 5, 7, 9, 11, 12};      // Major scale
    modes[ModeType::Dorian] = {0, 2, 3, 5, 7, 9, 10, 12};      // Natural minor with raised 6th
    modes[ModeType::Phrygian] = {0, 1, 3, 5, 7, 8, 10, 12};    // Natural minor with lowered 2nd
    modes[ModeType::Lydian] = {0, 2, 4, 6, 7, 9, 11, 12};      // Major with raised 4th
    modes[ModeType::Mixolydian] = {0, 2, 4, 5, 7, 9, 10, 12};  // Major with lowered 7th
    modes[ModeType::Aeolian] = {0, 2, 3, 5, 7, 8, 10, 12};     // Natural minor
    modes[ModeType::Locrian] = {0, 1, 3, 5, 6, 8, 10, 12};     // Diminished scale
    
    modeNames[ModeType::Ionian] = "Ionian";  // Major
    modeNames[ModeType::Dorian] = "Dorian";
    modeNames[ModeType::Phrygian] = "Phrygian";
    modeNames[ModeType::Lydian] = "Lydian";
    modeNames[ModeType::Mixolydian] = "Mixolydian";
    modeNames[ModeType::Aeolian] = "Aeolian";  // Natural minor
    modeNames[ModeType::Locrian] = "Locrian";
    
    // Initialize pattern names
    patternNames[PlaybackPattern::Ascending] = "Ascending";
    patternNames[PlaybackPattern::Descending] = "Descending";
    patternNames[PlaybackPattern::Intervallic] = "Thirds Ascending";
    patternNames[PlaybackPattern::IntervallicDescending] = "Thirds Descending";
    patternNames[PlaybackPattern::Random] = "Random";
}

void AudioEngine::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    currentSampleRate = sampleRate;
}

void AudioEngine::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!isPlaying)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto* leftBuffer = bufferToFill.buffer->getWritePointer(0, bufferToFill.startSample);
    auto* rightBuffer = bufferToFill.buffer->getWritePointer(1, bufferToFill.startSample);

    for (auto sample = 0; sample < bufferToFill.numSamples; ++sample)
    {
        // Calculate envelope (fade in/out for each note)
        float envelope = calculateEnvelope();
        
        // Generate sine wave
        auto currentSample = static_cast<float>(std::sin(currentAngle)) * 0.125f * envelope;

        leftBuffer[sample] = currentSample;
        rightBuffer[sample] = currentSample;

        currentAngle += angleDelta;
        samplesSinceNoteStart++;

        // Check if we need to move to the next note
        if (samplesSinceNoteStart >= static_cast<int>(noteDuration * currentSampleRate))
        {
            currentNoteIndex++;
            if (currentNoteIndex >= playbackOrder.size())
            {
                isPlaying = false;
                // Call callback to notify playback finished - marshal to message thread
                if (onPlaybackFinished)
                {
                    juce::MessageManager::callAsync([this]()
                    {
                        if (onPlaybackFinished)
                            onPlaybackFinished();
                    });
                }
                break;
            }
            else
            {
                playNextNote();
            }
        }
    }
}

void AudioEngine::releaseResources()
{
    // Clean up any resources if needed
}

void AudioEngine::playMode(ModeType mode, float rootFrequency, PlaybackPattern pattern)
{
    if (modes.find(mode) == modes.end())
        return;

    currentMode = mode;
    currentPattern = pattern;
    currentScale.clear();
    playbackOrder.clear();
    
    // Calculate frequencies for each note in the mode
    auto& modeIntervals = modes[mode];
    for (int semitone : modeIntervals)
    {
        float frequency = rootFrequency * std::pow(2.0f, semitone / 12.0f);
        currentScale.push_back(frequency);
    }
    
    // Generate playback order based on pattern
    switch (pattern)
    {
        case PlaybackPattern::Ascending:
            for (int i = 0; i < static_cast<int>(currentScale.size()); ++i)
                playbackOrder.push_back(i);
            break;
            
        case PlaybackPattern::Descending:
            for (int i = static_cast<int>(currentScale.size()) - 1; i >= 0; --i)
                playbackOrder.push_back(i);
            break;
            
        case PlaybackPattern::Intervallic:
            // Pattern: 1 3 2 4 3 5 4 6 5 7 6 1+ 7 2+ 1+
            // Indices: 0 2 1 3 2 4 3 5 4 6 5 7  6 8  7  (where 7=octave, 8=9th)
            {
                // Add higher octave notes to scale
                float octaveMultiplier = 2.0f;
                for (int i = 1; i < 3 && i < static_cast<int>(modeIntervals.size()); ++i) // 9th and 10th
                {
                    float frequency = rootFrequency * std::pow(2.0f, modeIntervals[i] / 12.0f) * octaveMultiplier;
                    currentScale.push_back(frequency);
                }
                
                // Build intervallic pattern
                std::vector<int> pattern = {0, 2, 1, 3, 2, 4, 3, 5, 4, 6, 5, 7, 6};
                if (currentScale.size() > 8) pattern.push_back(8); // 9th if available
                pattern.push_back(7); // octave
                
                for (int idx : pattern)
                {
                    if (idx < static_cast<int>(currentScale.size()))
                        playbackOrder.push_back(idx);
                }
            }
            break;
            
        case PlaybackPattern::IntervallicDescending:
            // Pattern: 8 6 7 5 6 4 5 3 4 2 3 1 2 7- 1
            // Indices: 7 5 6 4 5 3 4 2 3 1 2 0 1 -1 0  (where -1 is 7 below octave)
            {
                // Add the 7th below the root (7-)
                float belowSeventh = rootFrequency * std::pow(2.0f, (modeIntervals[6] - 12) / 12.0f);
                currentScale.insert(currentScale.begin(), belowSeventh); // Insert at beginning
                
                // Adjust all existing indices by +1 since we inserted at beginning
                // Pattern with adjusted indices: 8 6 7 5 6 4 5 3 4 2 3 1 2 0 1
                // (where index 0 is now the 7- and index 1 is the root)
                std::vector<int> pattern = {8, 6, 7, 5, 6, 4, 5, 3, 4, 2, 3, 1, 2, 0, 1};
                
                for (int idx : pattern)
                {
                    if (idx < static_cast<int>(currentScale.size()))
                        playbackOrder.push_back(idx);
                }
            }
            break;
            
        case PlaybackPattern::Random:
            // Start with root
            playbackOrder.push_back(0);
            // Add remaining notes in random order
            std::vector<int> remainingNotes;
            for (int i = 1; i < static_cast<int>(currentScale.size()); ++i)
                remainingNotes.push_back(i);
            
            while (!remainingNotes.empty())
            {
                int randomIndex = random.nextInt(static_cast<int>(remainingNotes.size()));
                playbackOrder.push_back(remainingNotes[randomIndex]);
                remainingNotes.erase(remainingNotes.begin() + randomIndex);
            }
            break;
    }

    currentNoteIndex = 0;
    samplesSinceNoteStart = 0;
    isPlaying = true;
    
    playNextNote();
}

void AudioEngine::stopPlaying()
{
    isPlaying = false;
    currentAngle = 0.0;
}

bool AudioEngine::isCurrentlyPlaying() const
{
    return isPlaying;
}

juce::String AudioEngine::getModeName(ModeType mode) const
{
    auto it = modeNames.find(mode);
    return (it != modeNames.end()) ? it->second : "Unknown";
}

std::vector<AudioEngine::ModeType> AudioEngine::getAllModes() const
{
    return {ModeType::Ionian, ModeType::Dorian, ModeType::Phrygian, 
            ModeType::Lydian, ModeType::Mixolydian, ModeType::Aeolian, ModeType::Locrian};
}

void AudioEngine::setPlaybackSpeed(float speed)
{
    // Clamp speed between 0.5 and 3.0
    speed = juce::jlimit(0.5f, 3.0f, speed);
    // Faster speed = shorter note duration
    noteDuration = 0.5f / speed;
}

juce::String AudioEngine::getPatternName(PlaybackPattern pattern) const
{
    auto it = patternNames.find(pattern);
    return (it != patternNames.end()) ? it->second : "Unknown";
}

std::vector<AudioEngine::PlaybackPattern> AudioEngine::getAllPatterns() const
{
    return {PlaybackPattern::Ascending, PlaybackPattern::Descending,
            PlaybackPattern::Intervallic, PlaybackPattern::IntervallicDescending, PlaybackPattern::Random};
}

void AudioEngine::setPlaybackPattern(PlaybackPattern pattern)
{
    currentPattern = pattern;
}

void AudioEngine::playNextNote()
{
    if (currentNoteIndex < playbackOrder.size())
    {
        int noteIdx = playbackOrder[currentNoteIndex];
        float frequency = currentScale[noteIdx];
        angleDelta = frequency * 2.0 * juce::MathConstants<double>::pi / currentSampleRate;
        samplesSinceNoteStart = 0;
    }
}

float AudioEngine::calculateEnvelope()
{
    float samplesPerNote = noteDuration * currentSampleRate;
    float attackTime = 0.05f * samplesPerNote;  // 5% attack
    float releaseTime = 0.2f * samplesPerNote;  // 20% release
    
    if (samplesSinceNoteStart < attackTime)
    {
        // Attack phase
        return samplesSinceNoteStart / attackTime;
    }
    else if (samplesSinceNoteStart > samplesPerNote - releaseTime)
    {
        // Release phase
        return (samplesPerNote - samplesSinceNoteStart) / releaseTime;
    }
    else
    {
        // Sustain phase
        return 1.0f;
    }
}
