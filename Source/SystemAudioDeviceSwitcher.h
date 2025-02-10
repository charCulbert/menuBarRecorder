#pragma once

#include <CoreAudio/CoreAudio.h>
#include <vector>

class SystemAudioDeviceSwitcher {
public:
    // Get current default output device
    static AudioDeviceID getCurrentOutputDevice();

    // Get device name from ID
    static bool getDeviceName(AudioDeviceID deviceId, char* outName, UInt32 maxLength);

    // Find device by name
    static AudioDeviceID findDeviceByName(const char* name);

    // Get all available audio devices
    static std::vector<AudioDeviceID> getAllDevices();

    // Set output device
    static bool setOutputDevice(AudioDeviceID deviceID);
};