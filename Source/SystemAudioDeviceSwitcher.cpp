//
// Created by Charlie Culbert on 2/9/25.
//

#include "SystemAudioDeviceSwitcher.h"
// AudioDeviceSwitcher.cpp

bool SystemAudioDeviceSwitcher::switchToBlackHole() {
    if (!usingBlackHole) {
        // Store current output if we haven't already
        if (!originalDevice) {
            AudioDeviceID currentDevice;
            UInt32 size = sizeof(AudioDeviceID);
            AudioObjectPropertyAddress addr = {
                kAudioHardwarePropertyDefaultOutputDevice,
                kAudioObjectPropertyScopeGlobal,
                kAudioObjectPropertyElementMaster
            };
            AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &currentDevice);
            originalDevice = currentDevice;
        }

        // Switch to BlackHole
        if (AudioDeviceID blackHole = findDevice("BlackHole 2ch")) {
            if (setOutputDevice(blackHole)) {
                usingBlackHole = true;
                return true;
            }
        }
    }
    return false;
}

bool SystemAudioDeviceSwitcher::restoreOriginalOutput() {
    if (usingBlackHole && originalDevice) {
        if (setOutputDevice(originalDevice)) {
            usingBlackHole = false;
            return true;
        }
    }
    return false;
}

AudioDeviceID SystemAudioDeviceSwitcher::findDevice(const char* name) {
    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    UInt32 size = 0;
    AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, nullptr, &size);
    int deviceCount = size / sizeof(AudioDeviceID);

    AudioDeviceID* devices = new AudioDeviceID[deviceCount];
    AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, devices);

    AudioDeviceID result = 0;
    for (int i = 0; i < deviceCount; ++i) {
        char deviceName[64];
        size = sizeof(deviceName);
        addr.mSelector = kAudioDevicePropertyDeviceName;
        AudioObjectGetPropertyData(devices[i], &addr, 0, nullptr, &size, deviceName);
        if (strcmp(deviceName, name) == 0) {
            result = devices[i];
            break;
        }
    }

    delete[] devices;
    return result;
}

bool SystemAudioDeviceSwitcher::setOutputDevice(AudioDeviceID deviceID) {
    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    return AudioObjectSetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr,
                                    sizeof(AudioDeviceID), &deviceID) == noErr;
}


const char* SystemAudioDeviceSwitcher::getOriginalDeviceName()
{
    if (originalDevice)
    {
        static char deviceName[64];
        UInt32 size = sizeof(deviceName);
        AudioObjectPropertyAddress addr = {
            kAudioDevicePropertyDeviceName,
            kAudioObjectPropertyScopeGlobal,
            kAudioObjectPropertyElementMaster
        };
        AudioObjectGetPropertyData(originalDevice, &addr, 0, nullptr, &size, deviceName);
        return deviceName;
    }
    return nullptr;
}