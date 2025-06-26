#include "MainComponent.h"
#include "AboutDialog.h"

MainComponent::MainComponent()
    : audioEngine()
    , random(juce::Time::currentTimeMillis())
    , score(0)
    , totalQuestions(0)
    , currentMode(AudioEngine::ModeType::Ionian)
    , lastPlayedMode(AudioEngine::ModeType::Ionian)
    , gameActive(false)
    , isPracticeMode(false)
{
    setSize(800, 600);
    
    
    // Make sure the window is properly sized
    if (auto* window = getTopLevelComponent())
        window->setSize(800, 600);

    // Set up the play button
    playButton.setButtonText("Play Random Mode");
    playButton.onClick = [this] { playRandomMode(); };
    addAndMakeVisible(playButton);

    // Set up the stop button
    stopButton.setButtonText("Stop");
    stopButton.onClick = [this] { stopPlaying(); };
    addAndMakeVisible(stopButton);
    
    // Set up the about button
    aboutButton.setButtonText("About");
    aboutButton.onClick = [this] { showAboutDialog(); };
    addAndMakeVisible(aboutButton);

    // Set up mode selection buttons
    auto modes = audioEngine.getAllModes();
    modeOrder = modes; // Store original order
    for (size_t i = 0; i < modes.size(); ++i)
    {
        auto mode = modes[i];
        auto button = std::make_unique<juce::TextButton>();
        button->setButtonText(audioEngine.getModeName(mode));
        button->onClick = [this, i] { 
            // Use index to get mode from current order
            auto mode = modeOrder[i];
            if (gameActive) 
                guessMode(mode); 
            else 
                practiceMode(mode); 
        };
        button->setEnabled(true); // Always enabled for practice mode
        button->setColour(juce::TextButton::buttonColourId, juce::Colours::darkblue);
        button->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        button->setColour(juce::TextButton::buttonOnColourId, juce::Colours::lightblue);
        modeButtons.push_back(std::move(button));
        addAndMakeVisible(modeButtons.back().get());
    }

    // Set up labels
    instructionLabel.setText("Click 'Play Random Mode' to start, or click any mode button to practice!", juce::dontSendNotification);
    instructionLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(instructionLabel);

    scoreLabel.setText("Score: 0/0", juce::dontSendNotification);
    scoreLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(scoreLabel);

    statusLabel.setText("", juce::dontSendNotification);
    statusLabel.setJustificationType(juce::Justification::centred);
    auto font = statusLabel.getFont();
    font.setHeight(16.0f);
    font.setBold(true);
    statusLabel.setFont(font);
    addAndMakeVisible(statusLabel);

    // Set up root note slider (now using note indices instead of frequencies)
    // Range: 12-24 represents A4 to A5 (one octave)
    rootNoteSlider.setRange(12.0, 24.0, 1.0); 
    rootNoteSlider.setValue(15.0); // C5 (15 semitones above A3)
    rootNoteSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    rootNoteSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 60, 20); // true = read-only
    
    // Custom text display for note names
    rootNoteSlider.textFromValueFunction = [this](double value) {
        return frequencyToNoteName(noteNameToFrequency(static_cast<int>(value)));
    };
    
    addAndMakeVisible(rootNoteSlider);

    rootNoteLabel.setText("Root Note:", juce::dontSendNotification);
    rootNoteLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rootNoteLabel);

    // Ensure the text box reflects the note at startup
    rootNoteSlider.updateText();

    // Set up speed slider
    speedSlider.setRange(0.5, 3.0, 0.1);
    speedSlider.setValue(1.0); // Normal speed
    speedSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, true, 60, 20); // true = read-only
    speedSlider.setTextValueSuffix("x");
    speedSlider.onValueChange = [this] { audioEngine.setPlaybackSpeed(static_cast<float>(speedSlider.getValue())); };
    addAndMakeVisible(speedSlider);

    speedLabel.setText("Speed:", juce::dontSendNotification);
    speedLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(speedLabel);

    // Set up pattern selection ComboBox
    auto patterns = audioEngine.getAllPatterns();
    for (size_t i = 0; i < patterns.size(); ++i)
    {
        patternComboBox.addItem(audioEngine.getPatternName(patterns[i]), static_cast<int>(i + 1));
    }
    patternComboBox.setSelectedId(1); // Default to Ascending
    addAndMakeVisible(patternComboBox);

    patternLabel.setText("Pattern:", juce::dontSendNotification);
    patternLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(patternLabel);

    // Set up randomize buttons checkbox
    randomizeButtonsCheckbox.setButtonText("Randomize order on each turn");
    randomizeButtonsCheckbox.setToggleState(false, juce::dontSendNotification); // Off by default
    
    // Note: ToggleButton font is controlled by the LookAndFeel
    
    // Style as square checkbox (not radio button)
    randomizeButtonsCheckbox.setRadioGroupId(0); // 0 means not part of radio group
    randomizeButtonsCheckbox.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    randomizeButtonsCheckbox.setColour(juce::ToggleButton::tickColourId, juce::Colours::lightblue);
    
    // Ensure it's clickable
    randomizeButtonsCheckbox.setClickingTogglesState(true);
    
    addAndMakeVisible(randomizeButtonsCheckbox);
    
    // Set up mode buttons label
    modeButtonsLabel.setText("Mode Buttons:", juce::dontSendNotification);
    modeButtonsLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(modeButtonsLabel);

    // Set up root pitch randomize checkbox
    randomizeRootPitchCheckbox.setButtonText("Randomize");
    randomizeRootPitchCheckbox.setToggleState(false, juce::dontSendNotification); // Off by default
    
    // Style as square checkbox (not radio button)
    randomizeRootPitchCheckbox.setRadioGroupId(0); // 0 means not part of radio group
    randomizeRootPitchCheckbox.setColour(juce::ToggleButton::textColourId, juce::Colours::white);
    randomizeRootPitchCheckbox.setColour(juce::ToggleButton::tickColourId, juce::Colours::lightblue);
    
    // Ensure it's clickable
    randomizeRootPitchCheckbox.setClickingTogglesState(true);
    
    addAndMakeVisible(randomizeRootPitchCheckbox);
    
    // Set up root pitch label
    rootSelectionLabel.setText("Root Selection:", juce::dontSendNotification);
    rootSelectionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rootSelectionLabel);

    // Some platforms require permissions to open input channels so request that here
    if (juce::RuntimePermissions::isRequired(juce::RuntimePermissions::recordAudio)
        && !juce::RuntimePermissions::isGranted(juce::RuntimePermissions::recordAudio))
    {
        juce::RuntimePermissions::request(juce::RuntimePermissions::recordAudio,
                                         [&](bool granted) { setAudioChannels(granted ? 2 : 0, 2); });
    }
    else
    {
        setAudioChannels(0, 2);
    }
    
    // Force initial layout to ensure buttons are visible
    juce::MessageManager::callAsync([this]()
    {
        resized();
        repaint();
    });
}

MainComponent::~MainComponent()
{
    shutdownAudio();
}

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    audioEngine.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    audioEngine.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
    audioEngine.releaseResources();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("Musical Mode Trainer", getLocalBounds().removeFromTop(60), juce::Justification::centred);
}

void MainComponent::showAboutDialog()
{
    juce::DialogWindow::LaunchOptions options;
    options.content.setOwned(new AboutDialog());
    options.content->setSize(600, 500);
    options.dialogTitle = "About Musical Mode Trainer";
    options.dialogBackgroundColour = juce::Colours::darkgrey;
    options.escapeKeyTriggersCloseButton = true;
    options.useNativeTitleBar = true;
    options.resizable = false;
    options.useBottomRightCornerResizer = false;

    options.launchAsync();
}



void MainComponent::resized()
{
    auto area = getLocalBounds();
	auto spaceForLabel = 120;
	auto labelRightMargin = 0;
	auto windowHorizontalMargin = 24;
    
    // Title area
    area.removeFromTop(60);

    // Instruction and score labels
    instructionLabel.setBounds(area.removeFromTop(25));
    scoreLabel.setBounds(area.removeFromTop(25));

    // Root note area: label, slider
    auto rootNoteArea = area.removeFromTop(35).reduced(0, 10);
    auto rootNoteLabelBounds = rootNoteArea.removeFromLeft(spaceForLabel);
	rootNoteLabelBounds.removeFromRight(labelRightMargin);
    rootNoteLabel.setBounds(rootNoteLabelBounds);
	rootNoteArea.removeFromRight(windowHorizontalMargin);
    rootNoteSlider.setBounds(rootNoteArea);

	// Root selection area: label, checkbox
	auto rootSelectionArea = area.removeFromTop(35).reduced(0, 10);
	auto rootSelectionLabelBounds = rootSelectionArea.removeFromLeft(spaceForLabel);
	rootSelectionLabelBounds.removeFromRight(labelRightMargin);
	rootSelectionLabel.setBounds(rootSelectionLabelBounds);
	rootSelectionArea.removeFromRight(windowHorizontalMargin);
	randomizeRootPitchCheckbox.setBounds(rootSelectionArea.expanded(0, 3).translated(-3, 0));
	
    // Speed slider area: label, slider
    auto speedArea = area.removeFromTop(35).reduced(0, 10);
    auto speedLabelBounds = speedArea.removeFromLeft(spaceForLabel);
	speedLabelBounds.removeFromRight(labelRightMargin);
    speedLabel.setBounds(speedLabelBounds);
	speedArea.removeFromRight(windowHorizontalMargin);
    speedSlider.setBounds(speedArea);

    // Pattern area: label, combo box
    auto patternArea = area.removeFromTop(35).reduced(0, 10);
    auto patternLabelBounds = patternArea.removeFromLeft(spaceForLabel);
    patternLabelBounds.removeFromRight(labelRightMargin);
    patternLabel.setBounds(patternLabelBounds);
	patternArea.removeFromRight(windowHorizontalMargin);
	patternComboBox.setBounds(patternArea.expanded(0, 4));

    // Mode buttons area: label, checkbox
    auto modeButtonsArea = area.removeFromTop(35).reduced(0, 10);
    auto modeButtonsLabelBounds = modeButtonsArea.removeFromLeft(spaceForLabel);
    modeButtonsLabelBounds.removeFromRight(labelRightMargin);
    modeButtonsLabel.setBounds(modeButtonsLabelBounds);
	modeButtonsArea.removeFromRight(windowHorizontalMargin);
	randomizeButtonsCheckbox.setBounds(modeButtonsArea.expanded(0, 3).translated(-3, 0));

    area.removeFromTop(10); // Add some spacing

    // Control buttons
	auto controlSpacing = 8;
	auto controlArea = area.removeFromTop(50).reduced(windowHorizontalMargin, 5);
	auto playArea = controlArea.removeFromLeft((controlArea.getWidth() - 2*controlSpacing) / 3);
	controlArea.removeFromLeft(controlSpacing);
    auto stopArea = controlArea.removeFromLeft((controlArea.getWidth() - 1*controlSpacing) / 2);
	controlArea.removeFromLeft(controlSpacing);
    auto aboutArea = controlArea;
    
    playButton.setBounds(playArea);
    stopButton.setBounds(stopArea);
    aboutButton.setBounds(aboutArea);

    // Status label
    statusLabel.setBounds(area.removeFromTop(40));
    
    area.removeFromTop(10); // Add spacing before mode buttons

    // Mode selection buttons - use remaining space at bottom
    // Reserve space for mode buttons at the bottom
	auto modeArea = area.reduced(windowHorizontalMargin, 0);
    int buttonHeight = 45;
    int buttonSpacing = 8;
    int buttonsPerRow = 4;
    
    // Calculate button width to fit 4 buttons per row with spacing
    int totalButtonWidth = modeArea.getWidth() - (buttonsPerRow + 1) * buttonSpacing;
    int buttonWidth = totalButtonWidth / buttonsPerRow;
    
    for (size_t i = 0; i < modeButtons.size() && i < 7; ++i)
    {
        int row = static_cast<int>(i) / buttonsPerRow;
        int col = static_cast<int>(i) % buttonsPerRow;
        
        // Center the last row if it has fewer buttons (3 buttons in row 2)
        int xOffset = 0;
        if (row == 1) // Second row with 3 buttons
        {
            int buttonsInLastRow = static_cast<int>(modeButtons.size()) - buttonsPerRow; // 7 - 4 = 3
            xOffset = (buttonsPerRow - buttonsInLastRow) * (buttonWidth + buttonSpacing) / 2;
        }
        
        int x = modeArea.getX() + buttonSpacing + xOffset + col * (buttonWidth + buttonSpacing);
        int y = modeArea.getY() + row * (buttonHeight + buttonSpacing);
        
        modeButtons[i]->setBounds(x, y, buttonWidth, buttonHeight);
    }
}

void MainComponent::playRandomMode()
{
    if (audioEngine.isCurrentlyPlaying())
        return;

    auto modes = audioEngine.getAllModes();
    
    // Avoid playing the same mode twice in a row
    AudioEngine::ModeType newMode;
    do {
        int randomIndex = random.nextInt(static_cast<int>(modes.size()));
        newMode = modes[randomIndex];
    } while (newMode == lastPlayedMode && modes.size() > 1);
    
    currentMode = newMode;
    lastPlayedMode = currentMode;

    // Handle root pitch randomization
    float rootFreq;
    if (randomizeRootPitchCheckbox.getToggleState())
    {
        // Generate random note index within slider range
        double minNote = rootNoteSlider.getMinimum();
        double maxNote = rootNoteSlider.getMaximum();
        int randomNoteIndex = static_cast<int>(minNote + random.nextDouble() * (maxNote - minNote));
        
        // Update slider to reflect the randomly chosen value
        rootNoteSlider.setValue(randomNoteIndex, juce::dontSendNotification);
        
        // Convert note index to frequency
        rootFreq = static_cast<float>(noteNameToFrequency(randomNoteIndex));
    }
    else
    {
        // Convert current slider value (note index) to frequency
        int noteIndex = static_cast<int>(rootNoteSlider.getValue());
        rootFreq = static_cast<float>(noteNameToFrequency(noteIndex));
    }
    
    auto selectedPattern = getSelectedPattern();
    audioEngine.onPlaybackFinished = [this]() {
        statusLabel.setText("", juce::dontSendNotification);
    };
    audioEngine.playMode(currentMode, rootFreq, selectedPattern);

    gameActive = true;
    instructionLabel.setText("Listen to the mode and select your answer below:", juce::dontSendNotification);
    statusLabel.setText("Playing...", juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::lightblue);

    // Randomize button order if checkbox is checked
    randomizeButtonOrder();

    // Enable mode buttons
    for (auto& button : modeButtons)
        button->setEnabled(true);

    // Force a layout update to make sure buttons appear
    resized();
    repaint();
}

void MainComponent::stopPlaying()
{
    audioEngine.stopPlaying();
    statusLabel.setText("", juce::dontSendNotification);
    repaint();
}

void MainComponent::guessMode(AudioEngine::ModeType guessedMode)
{
    if (!gameActive)
        return;

    totalQuestions++;
    bool correct = (guessedMode == currentMode);
    
    if (correct)
    {
        score++;
        statusLabel.setText("Correct! That was " + audioEngine.getModeName(currentMode), juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::green);
    }
    else
    {
        statusLabel.setText("Incorrect. That was " + audioEngine.getModeName(currentMode) + 
                            ", you guessed " + audioEngine.getModeName(guessedMode), juce::dontSendNotification);
        statusLabel.setColour(juce::Label::textColourId, juce::Colours::red);
    }

    // Update score display
    scoreLabel.setText("Score: " + juce::String(score) + "/" + juce::String(totalQuestions) + 
                      " (" + juce::String(static_cast<int>((static_cast<float>(score) / totalQuestions) * 100)) + "%)", 
                      juce::dontSendNotification);

    gameActive = false;
    instructionLabel.setText("Click 'Play Random Mode' for the next question, or practice with any mode button!", juce::dontSendNotification);
    
    // Buttons stay enabled for practice mode
}

void MainComponent::practiceMode(AudioEngine::ModeType mode)
{
    if (audioEngine.isCurrentlyPlaying() || gameActive)
        return;

    // Convert note index to frequency
    int noteIndex = static_cast<int>(rootNoteSlider.getValue());
    float rootFreq = static_cast<float>(noteNameToFrequency(noteIndex));
    auto selectedPattern = getSelectedPattern();
    audioEngine.onPlaybackFinished = [this]() {
        statusLabel.setText("", juce::dontSendNotification);
    };
    audioEngine.playMode(mode, rootFreq, selectedPattern);
    
    statusLabel.setText("Playing " + audioEngine.getModeName(mode), juce::dontSendNotification);
    statusLabel.setColour(juce::Label::textColourId, juce::Colours::orange);
}

AudioEngine::PlaybackPattern MainComponent::getSelectedPattern() const
{
    auto patterns = audioEngine.getAllPatterns();
    int selectedId = patternComboBox.getSelectedId();
    
    if (selectedId > 0 && selectedId <= static_cast<int>(patterns.size()))
    {
        return patterns[selectedId - 1]; // ComboBox IDs start at 1
    }
    
    // Default to first pattern if none selected
    return patterns[0];
}

void MainComponent::randomizeButtonOrder()
{
    auto modes = audioEngine.getAllModes();
    
    if (randomizeButtonsCheckbox.getToggleState())
    {
        // Shuffle the mode order
        modeOrder = modes; // Reset to original order first
        
        // Fisher-Yates shuffle
        for (int i = static_cast<int>(modeOrder.size()) - 1; i > 0; --i)
        {
            int j = random.nextInt(i + 1);
            std::swap(modeOrder[i], modeOrder[j]);
        }
    }
    else
    {
        // Reset to natural order when checkbox is off
        modeOrder = modes;
    }
    
    // Update button text to match current order
    for (size_t i = 0; i < modeButtons.size() && i < modeOrder.size(); ++i)
    {
        modeButtons[i]->setButtonText(audioEngine.getModeName(modeOrder[i]));
    }
}

juce::String MainComponent::frequencyToNoteName(double frequency) const
{
    // A4 = 440 Hz is our reference (note index 12)
    double a4 = 440.0;
    
    // Calculate the number of semitones from A4
    double semitonesFromA4 = 12.0 * log2(frequency / a4);
    int semitones = static_cast<int>(round(semitonesFromA4));
    
    // Array of note names (starting from A)
    const char* noteNames[] = {"A", "A#", "B", "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#"};
    
    // Calculate note and octave
    int noteIndex = ((semitones % 12) + 12) % 12; // Ensure positive modulo
    int octave = 4 + (semitones + (semitones < 0 ? -11 : 0)) / 12; // A4 is octave 4
    
    return juce::String(noteNames[noteIndex]) + juce::String(octave);
}

double MainComponent::noteNameToFrequency(int noteIndex) const
{
    // A3 = 220 Hz is note index 0
    // Each semitone up multiplies frequency by 2^(1/12)
    double a3 = 220.0;
    return a3 * pow(2.0, noteIndex / 12.0);
}

int MainComponent::frequencyToNoteIndex(double frequency) const
{
    // Convert frequency back to note index (inverse of noteNameToFrequency)
    double a3 = 220.0;
    return static_cast<int>(round(12.0 * log2(frequency / a3)));
}

