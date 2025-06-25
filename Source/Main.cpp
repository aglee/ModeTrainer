#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include "MainComponent.h"

class ModeTrainerApplication : public juce::JUCEApplication
{
public:
    ModeTrainerApplication() {}

    const juce::String getApplicationName() override       { return "Mode Trainer"; }
    const juce::String getApplicationVersion() override    { return "1.0.0"; }
    bool moreThanOneInstanceAllowed() override             { return true; }

    void initialise(const juce::String& commandLine) override
    {
        // This method is where you should put your application's initialisation code..
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    void shutdown() override
    {
        // Add your application's shutdown code here..
        mainWindow = nullptr; // (deletes our window)
    }

    void systemRequestedQuit() override
    {
        // This is called when the app is being asked to quit: you can ignore this
        // request and let the app carry on running, or call quit() to allow the app to close.
        quit();
    }

    void anotherInstanceStarted(const juce::String& commandLine) override
    {
        // When another instance of the app is launched while this one is running,
        // this method is invoked, and the commandLine parameter tells you what
        // the other instance's command-line arguments were.
    }

    class MainWindow : public juce::DocumentWindow
    {
    public:
        MainWindow(juce::String name)
            : DocumentWindow(name,
                           juce::Desktop::getInstance().getDefaultLookAndFeel()
                                                       .findColour(juce::ResizableWindow::backgroundColourId),
                           DocumentWindow::minimiseButton)
        {
            setUsingNativeTitleBar(true);
            
            mainComponent = new MainComponent();
            setContentOwned(mainComponent, true);
            

#if JUCE_IOS || JUCE_ANDROID
            setFullScreen(true);
#else
            setResizable(true, true);
            centreWithSize(800, 600);
#endif

            setVisible(true);
        }
        
        ~MainWindow() override
        {
        }

        void closeButtonPressed() override
        {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }
        

    private:
        MainComponent* mainComponent;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

START_JUCE_APPLICATION(ModeTrainerApplication)
