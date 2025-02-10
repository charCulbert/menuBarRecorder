//
// Created by Charlie Culbert on 2/9/25.
//

// SystemAudioDeviceSwitcher.h
#pragma once
#include <CoreAudio/CoreAudio.h>

class SystemAudioDeviceSwitcher {
public:
    static bool switchToBlackHole();
    static bool restoreOriginalOutput();
    static bool isUsingBlackHole() { return usingBlackHole; }
    static const char* getOriginalDeviceName();  // Changed to static

private:
    static AudioDeviceID findDevice(const char* name);
    static bool setOutputDevice(AudioDeviceID deviceID);

    inline static AudioDeviceID originalDevice = 0;
    inline static bool usingBlackHole = false;
};