#include "MainContentComponent.h"

MainContentComponent::MainContentComponent()
{
    // Loopback device selector
    addAndMakeVisible(loopbackDeviceSelector);
    loopbackDeviceSelector.onChange = [this]() {
        auto selectedDevice = loopbackDeviceSelector.getText().trim();
        if (!selectedDevice.isEmpty()) {
            AudioHandler::getInstance().setLoopbackDevice(selectedDevice);
        }
    };

    // Record button
    addAndMakeVisible(recordButton);
    recordButton.setButtonText("Start Recording");
    recordButton.onClick = [this]() {
        if (!isRecording) {
            startRecording();
        } else {
            stopRecording();
        }
    };

    // Recording duration label
    addAndMakeVisible(durationLabel);
    durationLabel.setJustificationType(juce::Justification::centred);
    durationLabel.setFont(juce::Font(juce::Font::getDefaultSansSerifFontName(), 16.0f, juce::Font::plain));

    populateLoopbackDevices();
    setSize(300, 150);
}

MainContentComponent::~MainContentComponent()
{
    stopTimer();
}

void MainContentComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.85f));

    if (isRecording) {
        // Draw recording indicator
        auto bounds = getLocalBounds().removeFromTop(30);
        g.setColour(juce::Colours::red);
        g.fillEllipse(bounds.getCentreX() - 5, bounds.getCentreY() - 5, 10, 10);
    }
}

void MainContentComponent::resized()
{
    auto area = getLocalBounds().reduced(5);
    auto buttonHeight = 30;
    auto spacing = 5;
    loopbackDeviceSelector.setBounds(area.removeFromTop(buttonHeight));
    area.removeFromTop(spacing);
    recordButton.setBounds(area.removeFromTop(buttonHeight));
    area.removeFromTop(spacing);
    durationLabel.setBounds(area.removeFromTop(buttonHeight));

}

void MainContentComponent::timerCallback()
{
    if (isRecording) {
        auto elapsed = juce::Time::getMillisecondCounter() - recordingStartTime;
        auto milliseconds = elapsed % 1000;
        auto totalSeconds = elapsed / 1000;
        auto seconds = totalSeconds % 60;
        auto minutes = (totalSeconds / 60) % 60;
        auto hours = totalSeconds / 3600;

        durationLabel.setText(juce::String::formatted("%02d:%02d:%02d.%03d",
            (int)hours, (int)minutes, (int)seconds, (int)milliseconds),
            juce::dontSendNotification);

        repaint(); // To update the recording indicator
    }
}



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
        struct QuitCallback : public juce::ModalComponentManager::Callback
        {
            void modalStateFinished (int) override
            {
                juce::JUCEApplication::quit();
            }
        };
        static QuitCallback quitCallback;

        juce::AlertWindow::showMessageBoxAsync(juce::AlertWindow::WarningIcon,
                                             "BlackHole 2ch Not Found",
                                             "Application is hardcoded to only support Blackhole 2ch. Rebuild if other loopback device is needed.",
                                             "OK",
                                             nullptr,
                                             &quitCallback);
    }
}

void MainContentComponent::startRecording()
{
    auto timestamp = juce::Time::getCurrentTime()
                      .formatted("%Y-%m-%d_%H-%M-%S");
    auto file = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                 .getChildFile("recording_" + timestamp + ".wav");

    AudioHandler::getInstance().switchToLoopbackDevice();
    AudioHandler::getInstance().startRecording(file);

    isRecording = true;
    recordingStartTime = juce::Time::getMillisecondCounter();
    recordButton.setButtonText("Stop Recording");
    startTimer(16); // Update at 60hz
    recordedFile = file;
}

void MainContentComponent::stopRecording()
{
    AudioHandler::getInstance().stopRecording();
    AudioHandler::getInstance().restoreOriginalOutputDevice();

    isRecording = false;
    recordButton.setButtonText("Start Recording");
    stopTimer();
    durationLabel.setText("00:00:00", juce::dontSendNotification);
    recordedFile.revealToUser();
    this->setVisible(false);
    repaint();
}