//
// Created by Charlie Culbert on 2/10/25.
//
#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "SystemAudioDeviceSwitcher.h"
#include "AudioRecorder.h"

class AudioHandler : public juce::AudioAppComponent,
                     public juce::Timer
{
public:
    AudioHandler();
    ~AudioHandler() override;

    static AudioHandler& getInstance()
    {
        static AudioHandler instance;
        return instance;
    }

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void printCurrentAppAudioSetup() const;
    void switchToLoopbackDevice();
    void restoreOriginalOutputDevice();
    void setLoopbackDevice(const juce::String& deviceName);
    void startRecording(const juce::File& file);
    void stopRecording();

    void timerCallback() override;  // 🔹 This will handle all timer-based operations

private:
    juce::String loopbackDeviceName = "BlackHole 2ch";
    AudioDeviceID originalOutputDeviceID = 0;
    juce::String originalOutputDeviceName;
    Recorder recorder;
    std::atomic<bool> isShuttingDown{false};  // Make it atomic
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioHandler);
};
