#include "MainContentComponent.h"

#include "../../../../../Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX15.2.sdk/System/Library/Frameworks/ImageIO.framework/Headers/CGImageAnimation.h"


MainContentComponent::MainContentComponent()
{
    // Loopback device selector dropdown
    addAndMakeVisible(loopbackDeviceSelector);
    loopbackDeviceSelector.onChange = [this]() {
        auto selectedDevice = loopbackDeviceSelector.getText().trim();

        if (selectedDevice.isEmpty()) {
            DBG("Error: No loopback device selected!");
            return;
        }

        DBG("Selected Loopback Device: " + selectedDevice);
        AudioHandler::getInstance().setLoopbackDevice(selectedDevice);
    };


    populateLoopbackDevices();

    // Initialize switch button
    addAndMakeVisible(switchButton);
    switchButton.setButtonText("Switch to Loopback");
    switchButton.onClick = []() {
        AudioHandler::getInstance().switchToLoopbackDevice();
    };

    addAndMakeVisible(startButton);
    startButton.setButtonText("Start Recording");
    startButton.onClick = []() {
        AudioHandler::getInstance().startRecording(juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                            .getChildFile("my_recording.wav"));
    };

    addAndMakeVisible(stopButton);
    stopButton.setButtonText("Stop Recording");
    stopButton.onClick = []() {
        AudioHandler::getInstance().stopRecording();
    };


    // Initialize switch back button
    addAndMakeVisible(switchBackButton);
    switchBackButton.setButtonText("Switch Back");
    switchBackButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);
    switchBackButton.onClick = []() {
        AudioHandler::getInstance().restoreOriginalOutputDevice();
    };


}

MainContentComponent::~MainContentComponent() = default;

void MainContentComponent::populateLoopbackDevices()
{
    loopbackDeviceSelector.clear();
    auto devices = SystemAudioDeviceSwitcher::getAllDevices();

    int index = 1;
    for (const auto& deviceID : devices)
    {
        char deviceName[64] = {0};
        if (SystemAudioDeviceSwitcher::getDeviceName(deviceID, deviceName, sizeof(deviceName)))
        {
            // Hard-coded: only allow "BlackHole 2ch"
            if (juce::String(deviceName).compareIgnoreCase("BlackHole 2ch") == 0)
            {
                loopbackDeviceSelector.addItem(juce::String(deviceName), index++);
            }
        }
    }

    if (loopbackDeviceSelector.getNumItems() > 0)
    {
        loopbackDeviceSelector.setSelectedId(1);
    }
    else
    {
        // Define a callback class that quits the application when the modal is dismissed.
        struct QuitCallback : public juce::ModalComponentManager::Callback
        {
            void modalStateFinished (int /*result*/) override
            {
                juce::JUCEApplication::quit();
            }
        };
        static QuitCallback quitCallback;

        // Use the asynchronous alert to avoid leaks.
        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                               "BlackHole 2ch Not Found",
                                               "Application is hardcoded to only support Blackhole 2ch. Rebuild if other loopback device is needed.",
                                               "OK",

                                               nullptr,
                                               &quitCallback);
    }
}



void MainContentComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.85f));
}

void MainContentComponent::resized()
{
    auto area = getLocalBounds();
    auto buttonHeight = 40;
    auto spacing = 5;

    loopbackDeviceSelector.setBounds(area.removeFromTop(buttonHeight + spacing).withSizeKeepingCentre(200, buttonHeight));
    switchButton.setBounds(area.removeFromTop(buttonHeight + spacing).withSizeKeepingCentre(200, buttonHeight));
    switchBackButton.setBounds(area.removeFromTop(buttonHeight + spacing).withSizeKeepingCentre(200, buttonHeight));
    startButton.setBounds(area.removeFromTop(buttonHeight + spacing).withSizeKeepingCentre(200, buttonHeight));
    stopButton.setBounds(area.removeFromTop(buttonHeight + spacing).withSizeKeepingCentre(200, buttonHeight));

}
