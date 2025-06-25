# Musical Mode Trainer

A desktop application built with JUCE for practicing musical mode recognition. This app helps musicians develop their ear training skills by playing different musical modes and challenging them to identify which mode is being played.

## Features

### Core Training Features
- **7 Musical Modes**: Ionian (Major), Dorian, Phrygian, Lydian, Mixolydian, Aeolian (Natural Minor), and Locrian
- **Two Training Modes**:
  - **Quiz Mode**: Press "Play Random Mode" to test your mode recognition skills
  - **Practice Mode**: Click any mode button to practice and learn specific modes
- **Score Tracking**: Real-time feedback with running score and percentage accuracy
- **Smart Mode Selection**: Prevents playing the same mode twice in a row for better training variety

### Customization Options
- **Root Note Selection**: Choose starting note from A3 to A6, displayed as musical notes (A4, C#5, Bb3, etc.)
- **Playback Speed Control**: Adjust tempo from 0.5x (slow) to 2.0x (fast) for different skill levels
- **Multiple Playback Patterns**:
  - **Ascending**: Traditional scale from low to high
  - **Descending**: Scale from high to low
  - **Ascending by Thirds**: Intervallic pattern for advanced ear training
  - **Descending by Thirds**: Descending intervallic pattern for advanced ear training
  - **Random**: Notes played in random order for maximum challenge
- **Randomization Features**:
  - **Button Order**: Randomize mode button positions to prevent location memorization
  - **Root Pitch**: Automatically select random root notes to avoid absolute pitch dependency

### User Interface
- **Clean Design**: Simple, intuitive interface focused on training
- **About Dialog**: Built-in help and information accessible via About button
- **Visual Feedback**: Clear status indicators for playing, correct/incorrect responses, and completion
- **Keyboard Shortcuts**: Return and Escape keys can close dialog windows

### Audio Quality
- **Professional Audio**: Clean sine wave synthesis with musical attack and release envelopes
- **Precise Tuning**: Equal temperament tuning with mathematically accurate frequencies
- **JUCE Audio Engine**: Professional-grade audio processing and real-time synthesis

## How to Use

### Quick Start
1. **Practice Mode**: Click any mode button (Ionian, Dorian, etc.) to hear that specific mode
2. **Quiz Mode**: Click "Play Random Mode" to start ear training challenges
3. Listen carefully to the characteristic sound of the mode
4. Click on the mode button that matches what you heard
5. Get immediate feedback and see your running score

### Customization
- **Adjust Root Note**: Use the "Root" slider to change the starting pitch
- **Change Speed**: Use the "Speed" slider (0.5x-2.0x) to adjust playback tempo
- **Select Pattern**: Choose from Ascending, Descending, Ascending by Thirds, Descending by Thirds, or Random
- **Enable Randomization**: Check boxes to randomize button order and/or root pitch for advanced training

### Training Progression
1. **Beginner**: Start with ascending pattern, normal speed, fixed root note
2. **Intermediate**: Try different patterns, enable button randomization
3. **Advanced**: Use random pattern with both randomization options enabled

## Musical Modes Included

- **Ionian (Major)**: The familiar major scale sound
- **Dorian**: Minor with a raised 6th degree
- **Phrygian**: Minor with a lowered 2nd degree
- **Lydian**: Major with a raised 4th degree
- **Mixolydian**: Major with a lowered 7th degree
- **Aeolian (Natural Minor)**: The natural minor scale
- **Locrian**: The most dissonant mode with a diminished 5th

## Building the Project

### Setting Up

- Download and install [Projucer](https://juce.com/get-juce) (Part of JUCE)

### Build Instructions

1. Open the Projucer application
2. Select "Open Existing Project" and choose `ModeTrainer.jucer`
3. Configure desired settings and click "Save and Open in IDE"
4. Choose your development environment (Xcode, Visual Studio, etc.)
5. Follow the IDE-specific steps to build and run the project

### Running the Application

On macOS, if using Xcode, look for `ModeTrainer.app` in the build output directory.

## Technical Details

- **Framework**: Built with JUCE 8.0.4 for cross-platform audio and GUI
- **Audio Engine**: Real-time sine wave synthesis with mathematical precision
- **Note Duration**: Configurable timing (default 0.5 seconds per note) with speed control
- **Envelope Shaping**: Professional audio envelopes (5% attack, 75% sustain, 20% release)
- **Tuning System**: Equal temperament with A4 = 440 Hz reference
- **Pattern Generation**: Multiple algorithmic approaches for different playback sequences
- **Random Generation**: Fisher-Yates shuffle for fair randomization
- **Modern C++**: C++17 features with clean, maintainable architecture

## Tips for Training

- Start with the most distinctive modes (Lydian, Phrygian, Locrian)
- Focus on the characteristic intervals that define each mode
- Practice with different root frequencies to avoid memorizing absolute pitch
- Listen for the overall "flavor" or mood of each mode rather than individual notes
- Use the intervallic patterns (Ascending/Descending by Thirds) to hear modes in a different context
- Enable randomization options as you become more confident to increase difficulty

Happy practicing!

## Development

### Project Structure
```
ModeTrainer/
├── ModeTrainer.jucer          # Projucer project file
├── Source/                    # Source code
│   ├── Main.cpp              # Application entry point
│   ├── MainComponent.cpp     # Main UI component
│   ├── AudioEngine.cpp       # Audio synthesis engine
│   └── AboutDialog.h         # About dialog with embedded help
├── ModeTrainer.icns          # Application icon
└── README.html               # Detailed HTML documentation
```

### Contributing
This project uses the JUCE framework and follows modern C++ practices. When contributing:
- Maintain thread safety (GUI operations on message thread only)
- Follow the existing code style and patterns
- Test audio functionality across different sample rates
- Ensure cross-platform compatibility

### License
This project is built with JUCE. Please refer to JUCE licensing terms for commercial use.
