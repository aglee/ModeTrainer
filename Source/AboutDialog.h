#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "EmbeddedContent.h"
#include <juce_core/juce_core.h>
#include <juce_gui_extra/juce_gui_extra.h>

// Custom WebBrowserComponent that forwards specific keys to parent
class DialogWebBrowser : public juce::WebBrowserComponent
{
public:
    bool keyPressed(const juce::KeyPress& key) override
    {
        // Forward Return and Delete keys to parent, let Escape work normally
        if (key == juce::KeyPress::returnKey || key == juce::KeyPress::deleteKey)
        {
            if (auto* parent = getParentComponent())
            {
                return parent->keyPressed(key);
            }
        }
        
        // Let the web view handle all other keys normally
        return WebBrowserComponent::keyPressed(key);
    }
};

class AboutDialog : public juce::Component
{
public:
    AboutDialog()
    {
        setSize(600, 500);
        
        // Title
        titleLabel.setText("Musical Mode Trainer", juce::dontSendNotification);
        auto titleFont = titleLabel.getFont();
        titleFont.setHeight(24.0f);
        titleFont.setBold(true);
        titleLabel.setFont(titleFont);
        titleLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(titleLabel);
        
        // Author and version
        infoLabel.setText("Author: Andy Lee\nVersion: 1.0.0", juce::dontSendNotification);
        auto infoFont = infoLabel.getFont();
        infoFont.setHeight(14.0f);
        infoLabel.setFont(infoFont);
        infoLabel.setJustificationType(juce::Justification::centred);
        addAndMakeVisible(infoLabel);
        
        // Use HTML content directly and display in a web view
        auto tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("readme.html");
        tempFile.replaceWithText(EmbeddedContent::README_CONTENT);
        webView.goToURL("file://" + tempFile.getFullPathName());
        addAndMakeVisible(webView);

        // Close button
        closeButton.setButtonText("Close");
        closeButton.onClick = [this] { 
            // Find the dialog window and close it
            if (auto* dialogWindow = findParentComponentOfClass<juce::DialogWindow>())
            {
                dialogWindow->exitModalState(0);
            }
        };
closeButton.setTriggeredOnMouseDown(false);
closeButton.addShortcut(juce::KeyPress(juce::KeyPress::returnKey));
closeButton.addShortcut(juce::KeyPress(juce::KeyPress::escapeKey));
closeButton.addShortcut(juce::KeyPress(juce::KeyPress::deleteKey));
        addAndMakeVisible(closeButton);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colours::darkgrey);
        g.setColour(juce::Colours::white);
        g.drawRect(getLocalBounds(), 2);
    }
    
    void resized() override
    {
        auto area = getLocalBounds().reduced(20);
        
        titleLabel.setBounds(area.removeFromTop(40));
        area.removeFromTop(10);
        
        infoLabel.setBounds(area.removeFromTop(50));
        area.removeFromTop(15);
        
        closeButton.setBounds(area.removeFromBottom(30));
        area.removeFromBottom(10);
        
        webView.setBounds(area);
    }
    
    
private:
    juce::Label titleLabel;
    juce::Label infoLabel;
    DialogWebBrowser webView;
    juce::TextButton closeButton;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutDialog)
};
