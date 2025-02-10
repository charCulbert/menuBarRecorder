#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include <memory>
#include "SystemAudioDeviceSwitcher.h"
#include "SystemTray.h"
#include "MainWindow.h"
#include "AudioHandler.h"
#include "CustomLookAndFeel.h"

class MenuBarApp : public juce::JUCEApplication {
public:
    MenuBarApp() {}

    const juce::String getApplicationName() override { return "MenuBarApp"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    CustomLookAndFeel customLookAndFeel;

    void initialise(const juce::String &) override {
        juce::LookAndFeel::setDefaultLookAndFeel(&customLookAndFeel);
        mainWindow = std::make_unique<MainWindow>();

        // Create system tray icon with callback
        trayIcon = std::make_unique<TrayIcon>([this]() {
            toggleMainWindow();
        });


    }

    void shutdown() override {
        trayIcon = nullptr;
        mainWindow = nullptr;
        // No need to delete AudioHandler since it's a singleton
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<TrayIcon> trayIcon;

    void toggleMainWindow() {
        if (!mainWindow->isVisible()) {
            mainWindow->setVisible(true);
#if JUCE_MAC
            juce::Process::makeForegroundProcess();
#endif
        } else {
            mainWindow->setVisible(false);
        }
    }
};

START_JUCE_APPLICATION(MenuBarApp)
