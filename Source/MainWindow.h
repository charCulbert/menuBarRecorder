// MainWindow.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "MainContentComponent.h"
#include "SystemAudioDeviceSwitcher.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow() : DocumentWindow("Audio Device Switcher",
                                juce::Colours::transparentBlack,
                                DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        mainContent = new MainContentComponent();
        setContentOwned(mainContent, true);
        setAlwaysOnTop(true);
        // Set a reasonable default size
        centreWithSize(400, 200);

        // We don't want to actually quit when closing, just hide
        setResizable(true, false); // Resizable by user, no constraints
    }

    void closeButtonPressed() override
    {

        setVisible(false);
        if (SystemAudioDeviceSwitcher::isUsingBlackHole()) {
            bool restored = SystemAudioDeviceSwitcher::restoreOriginalOutput();
        }
        mainContent->resetSwitchButton();
    }



private:
    MainContentComponent* mainContent;  // Keep a pointer to access it
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};