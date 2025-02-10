// MainContentComponent.cpp
#include "MainContentComponent.h"

MainContentComponent::MainContentComponent()
{
    // Initialize device switch button
    addAndMakeVisible(switchButton);
    switchButton.setButtonText("Switch to BlackHole");
    switchButton.addListener(this);

    // Initialize record button
    addAndMakeVisible(recordButton);
    recordButton.setButtonText("Start Recording");
    recordButton.addListener(this);
    recordButton.setColour(juce::TextButton::buttonColourId, juce::Colours::green);

    // Initialize recording time label
    addAndMakeVisible(recordingTimeLabel);
    recordingTimeLabel.setText("00:00:00", juce::dontSendNotification);
    recordingTimeLabel.setJustificationType(juce::Justification::centred);

    // Initialize audio recorder
    audioRecorder = std::make_unique<AudioRecorder>();
}

MainContentComponent::~MainContentComponent()
{
    switchButton.removeListener(this);
    recordButton.removeListener(this);
}

void MainContentComponent::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black.withAlpha(0.8f));
}

void MainContentComponent::resized()
{
    auto area = getLocalBounds();
    auto buttonHeight = 50;
    auto spacing = 10;

    // Layout components vertically
    auto topArea = area.removeFromTop(buttonHeight + spacing);
    switchButton.setBounds(topArea.withSizeKeepingCentre(200, buttonHeight));

    auto middleArea = area.removeFromTop(buttonHeight + spacing);
    recordButton.setBounds(middleArea.withSizeKeepingCentre(200, buttonHeight));

    recordingTimeLabel.setBounds(area.removeFromTop(30).withSizeKeepingCentre(200, 25));
}

void MainContentComponent::buttonClicked(juce::Button* button)
{
    if (button == &switchButton)
    {
        if (!SystemAudioDeviceSwitcher::isUsingBlackHole())
        {
            juce::Logger::writeToLog("\nAttempting to switch to BlackHole...");
            if (SystemAudioDeviceSwitcher::switchToBlackHole())
            {
                switchButton.setButtonText("Restore Original");

                // When we switch to BlackHole, start monitoring
                if (auto originalDeviceName = SystemAudioDeviceSwitcher::getOriginalDeviceName())
                {
                    audioRecorder->start("BlackHole 2ch", originalDeviceName);
                }
            }
        }
        else
        {
            juce::Logger::writeToLog("\nAttempting to restore original output...");
            if (SystemAudioDeviceSwitcher::restoreOriginalOutput())
            {
                switchButton.setButtonText("Switch to BlackHole");
                audioRecorder->stop();
            }
        }
    }
    else if (button == &recordButton)
    {
        // We'll add recording functionality back later
        juce::Logger::writeToLog("Record button clicked (disabled for now)");
    }
}

void MainContentComponent::resetSwitchButton()
{
    switchButton.setButtonText("Switch to BlackHole");
}


