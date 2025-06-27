#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "MarkdownConverter.h"
#include "EmbeddedMarkdown.h"
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
        
        // Convert embedded README markdown to HTML and display in a web view
        MarkdownConverter converter;
        std::string markdownContent = EmbeddedContent::README_MARKDOWN.toStdString();
        std::string convertedHtml = converter.convertToHtml(markdownContent);
        
        // Wrap in full HTML document with styling
        std::string htmlContent = createFullHtmlDocument(convertedHtml);
        
        auto tempFile = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("readme.html");
        tempFile.replaceWithText(htmlContent);
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
    
    std::string createFullHtmlDocument(const std::string& bodyHtml)
    {
        return R"(<!DOCTYPE html>
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
        ul, ol { margin: 8px 0 8px 0; padding-left: 0; list-style-position: outside; }
        ul { list-style-type: disc; }
        ol { list-style-type: decimal; }
        li { margin: 2px 0 2px 20px; }
        p { margin: 6px 0; }
        strong { font-weight: bold; }
        em { font-style: italic; }
        a { color: #007acc; text-decoration: none; }
        a:hover { text-decoration: underline; }
    </style>
</head>
<body>
)" + bodyHtml + R"(
</body>
</html>)";
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AboutDialog)
};
