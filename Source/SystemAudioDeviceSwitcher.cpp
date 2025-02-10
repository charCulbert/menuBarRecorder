#include "SystemAudioDeviceSwitcher.h"
#include <cstring>

AudioDeviceID SystemAudioDeviceSwitcher::getCurrentOutputDevice() {
    AudioDeviceID currentDevice;
    UInt32 size = sizeof(AudioDeviceID);
    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    
    if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, &currentDevice) == noErr) {
        return currentDevice;
    }
    return 0;
}

bool SystemAudioDeviceSwitcher::getDeviceName(AudioDeviceID deviceId, char* outName, UInt32 maxLength) {
    if (!deviceId || !outName || maxLength == 0) return false;
    
    AudioObjectPropertyAddress addr = {
        kAudioDevicePropertyDeviceName,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    UInt32 size = maxLength;
    return AudioObjectGetPropertyData(deviceId, &addr, 0, nullptr, &size, outName) == noErr;
}

AudioDeviceID SystemAudioDeviceSwitcher::findDeviceByName(const char* name) {
    if (!name) return 0;

    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    UInt32 size = 0;
    if (AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, nullptr, &size) != noErr) {
        return 0;
    }

    int deviceCount = size / sizeof(AudioDeviceID);
    if (deviceCount == 0) return 0;

    std::vector<AudioDeviceID> devices(deviceCount);
    if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, devices.data()) != noErr) {
        return 0;
    }

    AudioDeviceID result = 0;
    for (const auto& device : devices) {
        char deviceName[64];
        if (getDeviceName(device, deviceName, sizeof(deviceName)) && strcmp(deviceName, name) == 0) {
            result = device;
            break;
        }
    }

    return result;
}

std::vector<AudioDeviceID> SystemAudioDeviceSwitcher::getAllDevices() {
    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDevices,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };

    UInt32 size = 0;
    if (AudioObjectGetPropertyDataSize(kAudioObjectSystemObject, &addr, 0, nullptr, &size) != noErr) {
        return {};
    }

    int deviceCount = size / sizeof(AudioDeviceID);
    if (deviceCount == 0) return {};

    std::vector<AudioDeviceID> devices(deviceCount);
    if (AudioObjectGetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr, &size, devices.data()) != noErr) {
        return {};
    }

    return devices;
}

bool SystemAudioDeviceSwitcher::setOutputDevice(AudioDeviceID deviceID) {
    if (!deviceID) return false;

    AudioObjectPropertyAddress addr = {
        kAudioHardwarePropertyDefaultOutputDevice,
        kAudioObjectPropertyScopeGlobal,
        kAudioObjectPropertyElementMaster
    };
    
    return AudioObjectSetPropertyData(kAudioObjectSystemObject, &addr, 0, nullptr,
                                    sizeof(AudioDeviceID), &deviceID) == noErr;
}