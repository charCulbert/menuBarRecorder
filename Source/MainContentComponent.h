#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "SystemAudioDeviceSwitcher.h"
#include <AudioHandler.h>
#include <RecordButton.h>

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
    void focusLost(FocusChangeType) override;
    void mouseDown(const juce::MouseEvent& event) override;


private:
    void startRecording();
    void stopRecording();

    juce::File recordedFile;

    juce::ComboBox loopbackDeviceSelector;
    RecordButton recordButton;
    juce::Label durationLabel;
    juce::TextEditor fileNameEditor;  // New text field for filename

    std::unique_ptr<juce::DropShadowEffect> dropShadow;

    bool isRecording = false;
    bool isLoopbackVisible = false;
    juce::uint32 recordingStartTime = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContentComponent)
};
