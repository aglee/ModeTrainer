#pragma once

#include <juce_core/juce_core.h>

namespace EmbeddedContent
{
const juce::String README_CONTENT = R"(<!DOCTYPE html>
<html>
<head>
    <style>
        body { font-family: Arial, sans-serif; font-size: 14px; padding: 20px; line-height: 1.4; margin: 0; }
        h1, h2, h3, h4, h5, h6 { color: #333; margin-top: 16px; margin-bottom: 6px; }
        h1 { font-size: 20px; margin-top: 0; } h2 { font-size: 18px; } h3 { font-size: 16px; }
        h4 { font-size: 15px; } h5 { font-size: 14px; } h6 { font-size: 14px; }
        code { background-color: #f4f4f4; padding: 2px 4px; border-radius: 3px; font-family: monospace; font-size: 13px; }
        pre { background-color: #f4f4f4; padding: 8px; border-radius: 5px; overflow-x: auto; margin: 8px 0; }
        pre code { background-color: transparent; padding: 0; font-size: 13px; }
        ul { margin: 6px 0; padding-left: 20px; }
        li { margin: 2px 0; }
        p { margin: 6px 0; }
        strong { font-weight: bold; }
        em { font-style: italic; }
    </style>
</head>
<body>

<h1>Musical Mode Trainer</h1>

<p>A desktop application built with JUCE for practicing musical mode recognition. This app helps musicians develop their ear training skills by playing different musical modes and challenging them to identify which mode is being played.</p>

<h2>Features</h2>

<h3>Core Training Features</h3>
<ul>
<li><strong>7 Musical Modes</strong>: Ionian (Major), Dorian, Phrygian, Lydian, Mixolydian, Aeolian (Natural Minor), and Locrian</li>
<li><strong>Two Training Modes</strong>:
  <ul>
    <li><strong>Quiz Mode</strong>: Press "Play Random Mode" to test your mode recognition skills</li>
    <li><strong>Practice Mode</strong>: Click any mode button to practice and learn specific modes</li>
  </ul>
</li>
<li><strong>Score Tracking</strong>: Real-time feedback with running score and percentage accuracy</li>
<li><strong>Smart Mode Selection</strong>: Prevents playing the same mode twice in a row for better training variety</li>
</ul>

<h3>Customization Options</h3>
<ul>
<li><strong>Root Note Selection</strong>: Choose starting note from A3 to A6, displayed as musical notes (A4, C#5, Bb3, etc.)</li>
<li><strong>Playback Speed Control</strong>: Adjust tempo from 0.5x (slow) to 3.0x (fast) for different skill levels</li>
<li><strong>Multiple Playback Patterns</strong>:
  <ul>
    <li><strong>Ascending</strong>: Traditional scale from low to high</li>
    <li><strong>Descending</strong>: Scale from high to low</li>
    <li><strong>Thirds Ascending</strong>: Intervallic pattern for advanced ear training</li>
    <li><strong>Thirds Descending</strong>: Descending intervallic pattern for advanced ear training</li>
    <li><strong>Random</strong>: Notes played in random order for maximum challenge</li>
  </ul>
</li>
<li><strong>Randomization Features</strong>:
  <ul>
    <li><strong>Button Order</strong>: Randomize mode button positions to prevent location memorization</li>
    <li><strong>Root Pitch</strong>: Automatically select random root notes to avoid absolute pitch dependency</li>
  </ul>
</li>
</ul>

<h3>User Interface</h3>
<ul>
<li><strong>Clean Design</strong>: Simple, intuitive interface focused on training</li>
<li><strong>About Dialog</strong>: Built-in help and information accessible via About button</li>
<li><strong>Visual Feedback</strong>: Clear status indicators for playing, correct/incorrect responses, and completion</li>
<li><strong>Keyboard Shortcuts</strong>: Return and Escape keys can close dialog windows</li>
</ul>

<h3>Audio Quality</h3>
<ul>
<li><strong>Professional Audio</strong>: Clean sine wave synthesis with musical attack and release envelopes</li>
<li><strong>Precise Tuning</strong>: Equal temperament tuning with mathematically accurate frequencies</li>
<li><strong>JUCE Audio Engine</strong>: Professional-grade audio processing and real-time synthesis</li>
</ul>

<h2>How to Use</h2>

<h3>Quick Start</h3>
<ol>
<li><strong>Practice Mode</strong>: Click any mode button (Ionian, Dorian, etc.) to hear that specific mode</li>
<li><strong>Quiz Mode</strong>: Click "Play Random Mode" to start ear training challenges</li>
<li>Listen carefully to the characteristic sound of the mode</li>
<li>Click on the mode button that matches what you heard</li>
<li>Get immediate feedback and see your running score</li>
</ol>

<h3>Customization</h3>
<ul>
<li><strong>Adjust Root Note</strong>: Use the "Root" slider to change the starting pitch</li>
<li><strong>Change Speed</strong>: Use the "Speed" slider (0.5x-3.0x) to adjust playback tempo</li>
  <li><strong>Select Pattern</strong>: Choose from Ascending, Descending, Thirds Ascending, Thirds Descending, or Random</li>
<li><strong>Enable Randomization</strong>: Check boxes to randomize button order and/or root pitch for advanced training</li>
</ul>

<h3>Training Progression</h3>
<ol>
<li><strong>Beginner</strong>: Start with ascending pattern, normal speed, fixed root note</li>
<li><strong>Intermediate</strong>: Try different patterns, enable button randomization</li>
<li><strong>Advanced</strong>: Use random pattern with both randomization options enabled</li>
</ol>

<h2>Musical Modes Included</h2>

<ul>
<li><strong>Ionian (Major)</strong>: The familiar major scale sound</li>
<li><strong>Dorian</strong>: Minor with a raised 6th degree</li>
<li><strong>Phrygian</strong>: Minor with a lowered 2nd degree</li>
<li><strong>Lydian</strong>: Major with a raised 4th degree</li>
<li><strong>Mixolydian</strong>: Major with a lowered 7th degree</li>
<li><strong>Aeolian (Natural Minor)</strong>: The natural minor scale</li>
<li><strong>Locrian</strong>: The most dissonant mode with a diminished 5th</li>
</ul>

<h2>Building the Project</h2>

<h3>Setting Up</h3>

<ul>
<li>Download and install <a href="https://juce.com/get-juce">Projucer</a> (Part of JUCE)</li>
</ul>

<h3>Build Instructions</h3>

<ol>
<li>Open the Projucer application</li>
<li>Select "Open Existing Project" and choose <code>ModeTrainer.jucer</code></li>
<li>Configure desired settings and click "Save and Open in IDE"</li>
<li>Choose your development environment (Xcode, Visual Studio, etc.)</li>
<li>Follow the IDE-specific steps to build and run the project</li>
</ol>

<h3>Running the Application</h3>

<p>On macOS, if using Xcode, look for <code>ModeTrainer.app</code> in the build output directory.</p>

<h2>Technical Details</h2>

<ul>
<li><strong>Framework</strong>: Built with JUCE 8.0.4 for cross-platform audio and GUI</li>
<li><strong>Audio Engine</strong>: Real-time sine wave synthesis with mathematical precision</li>
<li><strong>Note Duration</strong>: Configurable timing (default 0.5 seconds per note) with speed control</li>
<li><strong>Envelope Shaping</strong>: Professional audio envelopes (5% attack, 75% sustain, 20% release)</li>
<li><strong>Tuning System</strong>: Equal temperament with A4 = 440 Hz reference</li>
<li><strong>Pattern Generation</strong>: Multiple algorithmic approaches for different playback sequences</li>
<li><strong>Random Generation</strong>: Fisher-Yates shuffle for fair randomization</li>
<li><strong>Modern C++</strong>: C++17 features with clean, maintainable architecture</li>
</ul>

<h2>Tips for Training</h2>

<ul>
<li>Start with the most distinctive modes (Lydian, Phrygian, Locrian)</li>
<li>Focus on the characteristic intervals that define each mode</li>
<li>Practice with different root frequencies to avoid memorizing absolute pitch</li>
<li>Listen for the overall "flavor" or mood of each mode rather than individual notes</li>
<li>Use the intervallic patterns (Thirds Ascending/Thirds Descending) to hear modes in a different context</li>
<li>Enable randomization options as you become more confident to increase difficulty</li>
</ul> 

<p>Happy practicing!</p>

</body>
</html>)";
}
