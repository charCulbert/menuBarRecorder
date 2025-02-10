// MainContentComponent.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "SystemAudioDeviceSwitcher.h"
#include <AudioHandler.h>

class MainContentComponent : public juce::Component,
                           public juce::Button::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override {}
    void populateLoopbackDevices();
private:
    juce::TextButton switchButton;
    juce::TextButton startButton;
    juce::TextButton stopButton;
    juce::TextButton switchBackButton;
    juce::ComboBox loopbackDeviceSelector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};