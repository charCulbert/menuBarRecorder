// MainContentComponent.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "SystemAudioDeviceSwitcher.h"
#include "AudioRecorder.h"

class MainContentComponent : public juce::Component,
                           public juce::Button::Listener
{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void buttonClicked(juce::Button* button) override;
    void resetSwitchButton();

private:


    juce::TextButton switchButton;
    juce::TextButton recordButton;
    juce::Label recordingTimeLabel;

    std::unique_ptr<AudioRecorder> audioRecorder;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};