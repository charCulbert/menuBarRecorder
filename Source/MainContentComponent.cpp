#include "MainContentComponent.h"


MainContentComponent::MainContentComponent()
{
    setWantsKeyboardFocus(true);

    // Loopback device selector (Initially hidden)
    addAndMakeVisible(loopbackDeviceSelector);
    loopbackDeviceSelector.setVisible(false);
    loopbackDeviceSelector.onChange = [this]() {
        auto selectedDevice = loopbackDeviceSelector.getText().trim();
        if (!selectedDevice.isEmpty()) {
            AudioHandler::getInstance().setLoopbackDevice(selectedDevice);
        }
    };

    // Custom record button
    addAndMakeVisible(recordButton);
    recordButton.onClick = [this]() {
        if (!isRecording) {
            startRecording();
        } else {
            stopRecording();
        }
    };
    recordButton.setClickingTogglesState(true);

    // Duration label
    addAndMakeVisible(durationLabel);
    durationLabel.setFont(juce::Font(18.0f, juce::Font::plain));
    durationLabel.setJustificationType(juce::Justification::centred);

    // Filename text editor
    addAndMakeVisible(fileNameEditor);
    fileNameEditor.setText("recording_{timestamp}", false);
    fileNameEditor.setJustification(juce::Justification::centred);
    fileNameEditor.setFont(juce::Font(12.0f, juce::Font::plain));
    fileNameEditor.setBorder(juce::BorderSize<int>(0)); // Remove border
    fileNameEditor.setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
    fileNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black.withAlpha(0.85f));
    fileNameEditor.setColour(juce::TextEditor::textColourId, juce::Colours::white);

    populateLoopbackDevices();
    setSize(300, 200);
}


MainContentComponent::~MainContentComponent()
{
    stopTimer();
}

void MainContentComponent::paint(juce::Graphics& g)
{

    auto bounds = getLocalBounds().toFloat();
    float cornerSize = 8.0f; // Adjust for macOS-like softness

    g.setColour(juce::Colours::black.withAlpha(0.85f));

    juce::Path roundedRect;
    roundedRect.addRoundedRectangle(bounds, cornerSize);

    g.reduceClipRegion(roundedRect);
    g.fillPath(roundedRect);


    g.fillAll(juce::Colours::black.withAlpha(0.85f));
}

void MainContentComponent::resized()
{
    auto bounds = getLocalBounds().reduced(5); // Add padding around the layout

    juce::FlexBox flexBox;
    flexBox.flexDirection = juce::FlexBox::Direction::column; // Arrange items vertically
    flexBox.justifyContent = juce::FlexBox::JustifyContent::center; // Center items
    flexBox.alignItems = juce::FlexBox::AlignItems::center; // Align items to center

    // Loopback Selector (Shown only when Alt is held)
    if (isLoopbackVisible)
    {
        flexBox.items.add(juce::FlexItem(loopbackDeviceSelector).withWidth(200.0f).withHeight(30.0f));
    }

    // Duration Label
    flexBox.items.add(juce::FlexItem(durationLabel).withWidth(200.0f).withHeight(30.0f).withMargin(juce::FlexItem::Margin(5.0f)));

    // Record Button
    flexBox.items.add(juce::FlexItem(recordButton).withWidth(50.0f).withHeight(50.0f).withMargin(juce::FlexItem::Margin(5.0f)));

    // Filename Text Field
    flexBox.items.add(juce::FlexItem(fileNameEditor).withWidth(200.0f).withHeight(30.0f).withMargin(juce::FlexItem::Margin(5.0f)));

    // Apply layout
    flexBox.performLayout(bounds);
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
        if (SystemAudioDeviceSwitcher::getDeviceName(deviceID, deviceName, sizeof(deviceName))) {
            // Hard-coded: only allow "BlackHole 2ch or MenuBarRecorder"
            if (juce::String(deviceName).compareIgnoreCase("MenuBarRecorder 2ch") == 0 || juce::String(deviceName).compareIgnoreCase("BlackHole 2ch") == 0)            {
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
                                             "No supported loopback audio device was found.",
                                             "Application is hardcoded to only support MenuBarRecorder 2ch or Blackhole 2ch. Rebuild if other loopback device is needed.",
                                             "OK",
                                             nullptr,
                                             &quitCallback);
    }
}

void MainContentComponent::startRecording()
{
    auto userFileName = fileNameEditor.getText().trim();
    if (userFileName.isEmpty())
        userFileName = "recording_{timestamp}";

    auto timestamp = juce::Time::getCurrentTime().formatted("%Y-%m-%d_%H-%M-%S");
    userFileName = userFileName.replace("{timestamp}", timestamp);

    auto file = juce::File::getSpecialLocation(juce::File::userDesktopDirectory)
                 .getChildFile(userFileName + ".wav");

    AudioHandler::getInstance().switchToLoopbackDevice();
    AudioHandler::getInstance().startRecording(file);

    isRecording = true;
    recordingStartTime = juce::Time::getMillisecondCounter();
    recordButton.setToggleState(true, juce::dontSendNotification);
    startTimer(16);
    recordedFile = file;
    this->setAlwaysOnTop(true);
}


void MainContentComponent::stopRecording()
{
    AudioHandler::getInstance().stopRecording();
    AudioHandler::getInstance().restoreOriginalOutputDevice();

    isRecording = false;
    recordButton.setToggleState(false, juce::dontSendNotification); // Change to record icon
    stopTimer();
    durationLabel.setText("00:00:00:000", juce::dontSendNotification);
    recordedFile.revealToUser();
    this->setAlwaysOnTop(false);
    juce::Component::unfocusAllComponents();
    this->setVisible(false);
}



void MainContentComponent::focusLost(FocusChangeType)
{
    if (!isParentOf(juce::Component::getCurrentlyFocusedComponent()))
    {
        if (!isRecording) {
            setVisible(false); // Hide only if focus moves outside this component
        }
    }
}

void MainContentComponent::mouseDown(const juce::MouseEvent& event)
{
    if (event.mods.isRightButtonDown())
    {
        isLoopbackVisible = !isLoopbackVisible; // Toggle visibility
        loopbackDeviceSelector.setVisible(isLoopbackVisible);
        resized(); // Ensure layout updates
        repaint(); // Redraw component
    }

}
