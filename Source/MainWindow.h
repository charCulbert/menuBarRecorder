#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "MainContentComponent.h"
#include "SystemAudioDeviceSwitcher.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow() 
        : DocumentWindow("Audio Device Switcher",
                         juce::Colours::transparentBlack,
                         DocumentWindow::closeButton)
    {
        setUsingNativeTitleBar(true);
        mainContent = std::make_unique<MainContentComponent>();

        setContentOwned(mainContent.release(), true); // Pass ownership correctly
        setAlwaysOnTop(true);
        centreWithSize(800, 400);
        setResizable(false, false);
    }

    void closeButtonPressed() override
    {
        setVisible(false);
    }

private:
    std::unique_ptr<MainContentComponent> mainContent; // Use smart pointer correctly
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};
