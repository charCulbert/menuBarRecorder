// AudioRecorder.h
#pragma once
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_utils/juce_audio_utils.h> // This is needed for AudioIODeviceCallback
#include <juce_audio_devices/juce_audio_devices.h>
#include "SystemAudioDeviceSwitcher.h"

class AudioRecorder : public juce::AudioIODeviceCallback
{
public:
    AudioRecorder()
    {
        deviceManager.initialiseWithDefaultDevices(2, 2);
        juce::Logger::writeToLog("AudioRecorder initialized with default devices");
    }

    bool start(const juce::String& inputDeviceName, const juce::String& outputDeviceName)
    {
        stop();

        auto setup = deviceManager.getAudioDeviceSetup();
        juce::Logger::writeToLog("Current setup - Input: " + setup.inputDeviceName +
                               ", Output: " + setup.outputDeviceName);

        // Print device details
        if (auto* deviceType = deviceManager.getCurrentDeviceTypeObject())
        {
            auto devices = deviceType->getDeviceNames();
            juce::Logger::writeToLog("\nAvailable devices:");
            for (const auto& device : devices)
                juce::Logger::writeToLog("  - " + device);
        }

        // Set new devices
        setup.inputDeviceName = inputDeviceName;
        setup.outputDeviceName = outputDeviceName;

        // Make sure we're actually going to get audio
        setup.useDefaultInputChannels = true;
        setup.useDefaultOutputChannels = true;
        setup.sampleRate = 48000;  // Set a standard sample rate
        setup.bufferSize = 512;    // Reasonable buffer size

        juce::Logger::writeToLog("\nAttempting to set - Input: " + inputDeviceName +
                               ", Output: " + outputDeviceName +
                               "\nSample Rate: " + juce::String(setup.sampleRate) +
                               "\nBuffer Size: " + juce::String(setup.bufferSize));

        auto error = deviceManager.setAudioDeviceSetup(setup, true);
        if (error.isNotEmpty())
        {
            juce::Logger::writeToLog("Error setting devices: " + error);
            return false;
        }

        juce::Logger::writeToLog("Successfully set devices");
        isMonitoring = true;  // Add flag for monitoring
        deviceManager.addAudioCallback(this);

        // Print final configuration
        if (auto* device = deviceManager.getCurrentAudioDevice())
        {
            juce::Logger::writeToLog("Final device setup:"
                                   "\nInput channels: " + juce::String(device->getActiveInputChannels().toInteger()) +
                                   "\nOutput channels: " + juce::String(device->getActiveOutputChannels().toInteger()) +
                                   "\nCurrent sample rate: " + juce::String(device->getCurrentSampleRate()) +
                                   "\nCurrent buffer size: " + juce::String(device->getCurrentBufferSizeSamples()));
        }

        return true;
    }

    void stop()
    {
        isMonitoring = false;
        deviceManager.removeAudioCallback(this);
        juce::Logger::writeToLog("AudioRecorder stopped");
    }

private:
    void audioDeviceIOCallbackWithContext (const float* const* inputChannelData,
                                             int numInputChannels,
                                             float* const* outputChannelData,
                                             int numOutputChannels,
                                             int numSamples,
                                             const juce::AudioIODeviceCallbackContext& context) override
    {
        if (!isMonitoring) return;

        static bool firstCallback = true;
        if (firstCallback)
        {
            juce::Logger::writeToLog("First audio callback - Channels: In=" +
                                   juce::String(numInputChannels) +
                                   " Out=" + juce::String(numOutputChannels));
            firstCallback = false;
        }

        // Simple monitoring - copy input to output
        for (int channel = 0; channel < juce::jmin(numInputChannels, numOutputChannels); ++channel)
        {
            if (inputChannelData[channel] != nullptr && outputChannelData[channel] != nullptr)
            {
                juce::FloatVectorOperations::copy(outputChannelData[channel],
                                                inputChannelData[channel],
                                                numSamples);
            }
        }
    }

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override
    {
        juce::Logger::writeToLog("Audio device about to start: " +
                               (device != nullptr ? device->getName() : "null"));
    }

    void audioDeviceStopped() override
    {
        juce::Logger::writeToLog("Audio device stopped");
    }

    juce::AudioDeviceManager deviceManager;
    bool isMonitoring = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioRecorder)
};