//
// Created by Charlie Culbert on 2/10/25.
//

#include "AudioHandler.h"
#include <juce_core/juce_core.h>


AudioHandler::AudioHandler() {
    // Initialize with 2 input and 2 output channels
    auto result = deviceManager.initialise(2, 2, nullptr, true);
    setAudioChannels(2, 2); // Request 2 inputs, 2 outputs


}

AudioHandler::~AudioHandler()
{
    isShuttingDown = true;
    stopTimer();
    deviceManager.closeAudioDevice();  // Add this line
    shutdownAudio(); // Stop audio processing
}



void AudioHandler::prepareToPlay(int samplesPerBlockExpected, double sampleRate) {
    // Called when the audio device is started
}

void AudioHandler::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (!device)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    auto activeInputChannels  = device->getActiveInputChannels();
    auto activeOutputChannels = device->getActiveOutputChannels();
    auto maxInputChannels  = activeInputChannels.getHighestBit() + 1;
    auto maxOutputChannels = activeOutputChannels.getHighestBit() + 1;

    if (maxInputChannels == 0 || maxOutputChannels == 0)
    {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    for (auto channel = 0; channel < maxOutputChannels; ++channel)
    {
        if (!activeOutputChannels[channel])
        {
            bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
        }
        else
        {
            auto inputChannel = channel % maxInputChannels; // Wrap input if fewer channels
            if (!activeInputChannels[inputChannel])
            {
                bufferToFill.buffer->clear(channel, bufferToFill.startSample, bufferToFill.numSamples);
            }
            else
            {
                auto* inBuffer = bufferToFill.buffer->getReadPointer(inputChannel, bufferToFill.startSample);
                auto* outBuffer = bufferToFill.buffer->getWritePointer(channel, bufferToFill.startSample);

                for (int sample = 0; sample < bufferToFill.numSamples; ++sample)
                {
                    outBuffer[sample] = inBuffer[sample]; // Direct pass-through
                }
            }
        }
    }

    // **Record audio if recording is enabled**
    if (recorder.isRecording())
    {
        recorder.process(bufferToFill);
    }
}




void AudioHandler::releaseResources() {
    // Called when the audio device stops
}

void AudioHandler::setLoopbackDevice(const juce::String& deviceName)
{
    if (deviceName.isEmpty()) {
        DBG("Error: Empty loopback device name received!");
        return;
    }

    loopbackDeviceName = deviceName;
    DBG("Loopback device set to: " + loopbackDeviceName);
}


void AudioHandler::printCurrentAppAudioSetup() const {
    // 1. Print system-level output device
    AudioDeviceID systemOutputDeviceID = SystemAudioDeviceSwitcher::getCurrentOutputDevice();
    char systemDeviceName[64] = {0};

    if (SystemAudioDeviceSwitcher::getDeviceName(systemOutputDeviceID, systemDeviceName, sizeof(systemDeviceName))) {
        DBG("System Output Device: " + juce::String(juce::CharPointer_UTF8(systemDeviceName)) + " (ID: " + juce::String(systemOutputDeviceID) + ")");
    } else {
        DBG("Failed to retrieve system output device.");
    }

    // 2. Print app's internal audio setup
    auto setup = deviceManager.getAudioDeviceSetup();
    DBG("App Audio Device Setup:");
    DBG(" Input Device: " << setup.inputDeviceName);
    DBG(" Output Device: " << setup.outputDeviceName);
    DBG(" Sample Rate: " << setup.sampleRate);
    DBG(" Buffer Size: " << setup.bufferSize);
    DBG(" Input Channels: " << setup.inputChannels.countNumberOfSetBits());
    DBG(" Output Channels: " << setup.outputChannels.countNumberOfSetBits());

    if (auto *device = deviceManager.getCurrentAudioDevice()) {
        DBG(" Device Type: " << device->getTypeName());
        DBG(" Current Latency: " << device->getOutputLatencyInSamples());
    }
}


void AudioHandler::switchToLoopbackDevice()
{
    if (isShuttingDown) return;

    DBG("=== Starting Audio Switch ===");

    originalOutputDeviceID = SystemAudioDeviceSwitcher::getCurrentOutputDevice();
    char originalDeviceNameBuffer[64] = {0};

    if (!SystemAudioDeviceSwitcher::getDeviceName(originalOutputDeviceID, originalDeviceNameBuffer, sizeof(originalDeviceNameBuffer))) {
        DBG("ERROR: Failed to get original device name");
        return;
    }

    originalOutputDeviceName = juce::String(juce::CharPointer_UTF8(originalDeviceNameBuffer));
    DBG("Original device: " + originalOutputDeviceName + " (ID: " + juce::String(originalOutputDeviceID) + ")");

    AudioDeviceID loopbackDeviceID = SystemAudioDeviceSwitcher::findDeviceByName(loopbackDeviceName.toRawUTF8());
    if (loopbackDeviceID == 0) {
        DBG("ERROR: Loopback device not found: " + loopbackDeviceName);
        return;
    }

    if (!SystemAudioDeviceSwitcher::setOutputDevice(loopbackDeviceID)) {
        DBG("ERROR: Failed to set system output to loopback");
        return;
    }

    DBG("System output switched to loopback");

    // 🔹 Start a timer that will trigger `timerCallback()` after 100ms
    startTimer(1);
}

void AudioHandler::timerCallback()
{
    if (isShuttingDown) return;  // ✅ Prevent execution if shutdown is happening

    stopTimer();  // ✅ Stop the timer immediately so it only runs once

    auto setup = deviceManager.getAudioDeviceSetup();
    setup.inputDeviceName = loopbackDeviceName;
    setup.outputDeviceName = originalOutputDeviceName;

    DBG("Setting app audio - Input: " + setup.inputDeviceName + ", Output: " + setup.outputDeviceName);
    juce::String error = deviceManager.setAudioDeviceSetup(setup, true);

    if (!error.isEmpty()) {
        DBG("ERROR setting audio devices: " + error);
    } else {
        DBG("App audio setup complete");
        printCurrentAppAudioSetup();
    }
}



void AudioHandler::restoreOriginalOutputDevice() {
    if (originalOutputDeviceID == 0) {
        DBG("No original output device stored. Cannot restore.");
        return;
    }

    // 1. Restore system output device
    if (!SystemAudioDeviceSwitcher::setOutputDevice(originalOutputDeviceID)) {
        DBG("Failed to restore system output device: " + originalOutputDeviceName);
        return;
    }
    DBG("System output restored to: " + originalOutputDeviceName);

    // 2. Restore app's internal audio setup
    auto setup = deviceManager.getAudioDeviceSetup();

    // Keep input as loopback if already set, otherwise disable input
    if (setup.inputDeviceName != loopbackDeviceName)
        setup.inputDeviceName = loopbackDeviceName;

    setup.outputDeviceName = ""; // **Disable output**

    juce::String error = deviceManager.setAudioDeviceSetup(setup, true);
    if (!error.isEmpty()) {
        DBG("Failed to restore internal audio device: " + error);
    } else {
        DBG("Internal audio restored. Input: " + setup.inputDeviceName + ", Output: None");
    }

    // Reset stored original device info
    originalOutputDeviceID = 0;
    originalOutputDeviceName.clear();
}

void AudioHandler::startRecording(const juce::File& file)
{
    auto* device = deviceManager.getCurrentAudioDevice();
    if (device)
    {
        double sampleRate = device->getCurrentSampleRate();
        int numChannels = device->getActiveInputChannels().countNumberOfSetBits();
        recorder.startRecording(file, sampleRate, numChannels);
    }
}

void AudioHandler::stopRecording()
{
    recorder.stopRecording();
}
