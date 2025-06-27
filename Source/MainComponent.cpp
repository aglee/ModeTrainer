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
    setSize(800, 400);
    
    // Set up title label
    titleLabel.setText("Musical Mode Trainer", juce::dontSendNotification);
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setFont(juce::Font(24.0f, juce::Font::bold));
    addAndMakeVisible(titleLabel);
    
    // Make sure the window is properly sized
    if (auto* window = getTopLevelComponent()) {
        window->setSize(800, 400);
    }
    
    // Set up the play button
    playButton.setButtonText("Play Random Scale");
    playButton.onClick = [this] { playRandomScale(); };
    playButton.setColour(juce::TextButton::buttonColourId, juce::Colour(0x880000ff));
	playButton.setColour(juce::TextButton::textColourOffId, juce::Colours::white);
//	playButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::red);
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
        modeButtons.push_back(std::move(button));
        addAndMakeVisible(modeButtons.back().get());
    }
    
    // Set up labels - instruction text moved to status field
    
    scoreLabel.setText("Score: 0/0", juce::dontSendNotification);
    scoreLabel.setJustificationType(juce::Justification::centred);
    scoreLabel.setFont(scoreLabel.getFont().withHeight(20.0f));
    addAndMakeVisible(scoreLabel);
    
    statusLabel.setJustificationType(juce::Justification::centred);
    auto statusLabelFont = statusLabel.getFont();
    statusLabelFont.setHeight(16.0f);
    statusLabelFont.setBold(true);
    statusLabel.setFont(statusLabelFont);
    showInstructionsText();
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
    randomizeModeButtonsCheckbox.setButtonText("Randomize button order on each turn");
    randomizeModeButtonsCheckbox.setToggleState(false, juce::dontSendNotification); // Off by default
    
    // Note: ToggleButton font is controlled by the LookAndFeel
    
    // Style as square checkbox (not radio button)
    randomizeModeButtonsCheckbox.setRadioGroupId(0); // 0 means not part of radio group
    
    // Ensure it's clickable
    randomizeModeButtonsCheckbox.setClickingTogglesState(true);
    
    addAndMakeVisible(randomizeModeButtonsCheckbox);
    
    // Set up mode buttons label
    modeButtonsLabel.setText("Mode Buttons:", juce::dontSendNotification);
    modeButtonsLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(modeButtonsLabel);
    
    // Set up root pitch randomize checkbox
    randomizeRootCheckbox.setButtonText("Randomize");
    randomizeRootCheckbox.setToggleState(false, juce::dontSendNotification); // Off by default
    
    // Style as square checkbox (not radio button)
    randomizeRootCheckbox.setRadioGroupId(0); // 0 means not part of radio group
    
    // Ensure it's clickable
    randomizeRootCheckbox.setClickingTogglesState(true);
    
    addAndMakeVisible(randomizeRootCheckbox);
    
    // Set up root pitch label
    rootSelectionLabel.setText("Root Selection:", juce::dontSendNotification);
    rootSelectionLabel.setJustificationType(juce::Justification::centredRight);
    addAndMakeVisible(rootSelectionLabel);
    
    // Set up light mode toggle
	colorsLabel.setText("Colors:", juce::dontSendNotification);
	colorsLabel.setJustificationType(juce::Justification::centredRight);
	addAndMakeVisible(colorsLabel);

    lightModeToggle.setButtonText("Light mode");
    lightModeToggle.setToggleState(true, juce::dontSendNotification); // Light mode by default
    lightModeToggle.setRadioGroupId(0); // Not part of radio group
    lightModeToggle.setClickingTogglesState(true);
    lightModeToggle.onClick = [this] { 
        bool isLightMode = lightModeToggle.getToggleState();
        if (isLightMode) {
            setCustomLookAndFeel(lightModeLookAndFeel);
        } else {
            setCustomLookAndFeel(darkModeLookAndFeel);
        }
        repaint();
    };
    addAndMakeVisible(lightModeToggle);
	lightModeToggle.onClick();
    
    // Set up options section label
    optionsLabel.setText("Options", juce::dontSendNotification);
    optionsLabel.setJustificationType(juce::Justification::centred);
    auto difficultyFont = optionsLabel.getFont();
    difficultyFont.setHeight(16.0f);
    difficultyFont.setBold(true);
    optionsLabel.setFont(difficultyFont);
    addAndMakeVisible(optionsLabel);
    
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
    // Clean up custom LookAndFeel
    patternComboBox.setLookAndFeel(nullptr);
    setLookAndFeel(nullptr);
    
    shutdownAudio();
}

void MainComponent::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
    auto windowHorizontalMargin = 24;
    
    // Title area
    titleLabel.setBounds(area.removeFromTop(60));
    area.removeFromTop(10); // Add some spacing
    
    // Score label
    scoreLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(20);
    
    // Status label
    statusLabel.setBounds(area.removeFromTop(25));
    
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
    
    area.removeFromTop(10); // Add spacing before mode buttons
    
    // Mode selection buttons - use middle space
    auto modeArea = area.removeFromTop(40).reduced(windowHorizontalMargin, 0);
    int buttonHeight = 28;
    int buttonSpacing = 8;
    int buttonsPerRow = 7;   //4;
    
    // Calculate button width to fit buttonsPerRow buttons per row with spacing
    int totalButtonWidth = modeArea.getWidth() - (buttonsPerRow + 1) * buttonSpacing;
    int buttonWidth = totalButtonWidth / buttonsPerRow;
    
    for (size_t i = 0; i < modeButtons.size() && i < 7; ++i)
    {
        int row = static_cast<int>(i) / buttonsPerRow;
        int col = static_cast<int>(i) % buttonsPerRow;
        
        // Center the second row if it has fewer buttons
        int xOffset = 0;
        if (row == 1)
        {
            int buttonsInLastRow = static_cast<int>(modeButtons.size()) - buttonsPerRow;
            xOffset = (buttonsPerRow - buttonsInLastRow) * (buttonWidth + buttonSpacing) / 2;
        }
        
        int x = modeArea.getX() + buttonSpacing + xOffset + col * (buttonWidth + buttonSpacing);
        int y = modeArea.getY() + row * (buttonHeight + buttonSpacing);
        
        modeButtons[i]->setBounds(x, y, buttonWidth, buttonHeight);
    }
    
    area.removeFromTop(20); // Add vertical spacing
    
    // === OPTIONS SECTION (at bottom) ===
    
    optionsLabel.setBounds(area.removeFromTop(25));
    area.removeFromTop(5); // Small spacing below section label
    
    auto layOutLabelAndControl = [&area](Component &label, Component &control) {
        auto slice = area.removeFromTop(35).reduced(24, 0);
        auto labelArea = slice.removeFromLeft(100);
        auto controlArea = slice;
        label.setBounds(labelArea);
        control.setBounds(controlArea);
    };
    
    layOutLabelAndControl(rootNoteLabel, rootNoteSlider);
    layOutLabelAndControl(rootSelectionLabel, randomizeRootCheckbox);
    layOutLabelAndControl(speedLabel, speedSlider);
    layOutLabelAndControl(patternLabel, patternComboBox);
	layOutLabelAndControl(modeButtonsLabel, randomizeModeButtonsCheckbox);
	layOutLabelAndControl(colorsLabel, lightModeToggle);
    
	// Small layout tweaks
    patternComboBox.setBounds(patternComboBox.getBounds().reduced(0, 6));
    auto checkboxNudgeX = -4;
    auto checkboxNudgeY = 1;
    randomizeRootCheckbox.setBounds(randomizeRootCheckbox.getBounds().translated(checkboxNudgeX, checkboxNudgeY));
	randomizeModeButtonsCheckbox.setBounds(randomizeModeButtonsCheckbox.getBounds().translated(checkboxNudgeX, checkboxNudgeY));
	lightModeToggle.setBounds(lightModeToggle.getBounds().translated(checkboxNudgeX, checkboxNudgeY));
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

void MainComponent::setStatusWithText(GameStatus status, juce::String text)
{
	gameStatus = status;
	statusLabel.setText(text, juce::dontSendNotification);
	updateStatusLabelColour();
}

void MainComponent::updateStatusLabelColour()
{
	juce::Colour textColor = getLookAndFeel().findColour(juce::Label::textColourId);
	CustomLookAndFeel &lookAndFeel = currentLookAndFeel.get();
	switch (gameStatus) {
		case GameStatus::instructions:
			textColor = lookAndFeel.getInstructionsColour();
			break;
		case GameStatus::playingForPractice:
			textColor = lookAndFeel.getPlayingForPracticeColour();
			break;
		case GameStatus::playingForGuess:
			textColor = lookAndFeel.getPlayingForGuessColour();
			break;
		case GameStatus::waitingForGuess:
			textColor = lookAndFeel.getWaitingForGuessColour();
			break;
		case GameStatus::correctGuess:
			textColor = lookAndFeel.getCorrectGuessColour();
			break;
		case GameStatus::incorrectGuess:
			textColor = lookAndFeel.getIncorrectGuessColour();
			break;
		default:
			jassertfalse;
	}
	statusLabel.setColour(juce::Label::textColourId, textColor);
}

// MARK: - (Game play)

void MainComponent::guessMode(AudioEngine::ModeType guessedMode)
{
    if (!gameActive)
        return;
    
    totalQuestions++;
    bool correct = (guessedMode == currentMode);
    
    if (correct)
    {
        score++;
		setStatusWithText(GameStatus::correctGuess, "Correct! That was " + audioEngine.getModeName(currentMode) + ".");
    }
    else
    {
		setStatusWithText(GameStatus::incorrectGuess, "Incorrect. That was " + audioEngine.getModeName(currentMode) + 
						  ", you guessed " + audioEngine.getModeName(guessedMode) + ".");
    }
    
    // Update score display
    scoreLabel.setText("Score: " + juce::String(score) + "/" + juce::String(totalQuestions) + 
                       " (" + juce::String(static_cast<int>((static_cast<float>(score) / totalQuestions) * 100)) + "%)", 
                       juce::dontSendNotification);
    
    gameActive = false;
    
    // After a short delay, show the instruction for next turn
    juce::Timer::callAfterDelay(2000, [this]() {
        if (!gameActive) { // Only update if still not in game
			showInstructionsText();
        }
    });
    
    // Buttons stay enabled for practice mode
}

void MainComponent::showInstructionsText()
{
	setStatusWithText(GameStatus::instructions, "Click \"Play Random Scale\" to test your knowledge, or click any mode button to hear that scale.");
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
        // Show instructions when playback finishes
        if (!gameActive && !audioEngine.isCurrentlyPlaying()) {
            showInstructionsText();
        }
    };
    audioEngine.playMode(mode, rootFreq, selectedPattern);
    
	setStatusWithText(GameStatus::playingForPractice, "Playing " + audioEngine.getModeName(mode) + "... Try to remember how this sounds...");
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
    
    if (randomizeModeButtonsCheckbox.getToggleState())
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

// MARK: - (Playing scales)

void MainComponent::playRandomScale()
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
    if (randomizeRootCheckbox.getToggleState())
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
		setStatusWithText(GameStatus::waitingForGuess, "Click a mode button to enter your answer...");
    };
    audioEngine.playMode(currentMode, rootFreq, selectedPattern);
    
    gameActive = true;
	setStatusWithText(GameStatus::playingForGuess, "Playing... Listen and select your answer below...");
    
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
    showInstructionsText();
    repaint();
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

// MARK: - (AudioAppComponent)

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

