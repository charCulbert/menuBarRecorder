#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "SystemAudioDeviceSwitcher.h"
#include <AudioHandler.h>

class MainContentComponent : public juce::Component,
                           public juce::Timer

{
public:
    MainContentComponent();
    ~MainContentComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    void populateLoopbackDevices();

private:
    void startRecording();
    void stopRecording();

    juce::File recordedFile;

    juce::ComboBox loopbackDeviceSelector;
    juce::TextButton recordButton;
    juce::Label durationLabel;

    bool isRecording = false;
    juce::uint32 recordingStartTime = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};