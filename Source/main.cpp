#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>
#include "SystemAudioDeviceSwitcher.h"
#include "SystemTray.h"
#include "MainWindow.h"



class MenuBarApp : public juce::JUCEApplication
{
public:
    MenuBarApp() {}

    const juce::String getApplicationName() override { return "MenuBarApp"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }

    void initialise(const juce::String&) override
    {
        mainWindow.reset(new MainWindow());
        // mainWindow->setVisible(true);  // Add this if you want it visible on startup

        // Create system tray icon with callback
        trayIcon.reset(new TrayIcon([this]() {
       if (!mainWindow->isVisible())
       {
        mainWindow->setVisible(true);
        #if JUCE_MAC
        juce::Process::makeForegroundProcess();
        #endif
       }
       else
       {
           mainWindow->setVisible(false);
       }
   }));
    }

    void shutdown() override
    {
        trayIcon = nullptr;
        mainWindow = nullptr;
    }

private:
    std::unique_ptr<MainWindow> mainWindow;
    std::unique_ptr<TrayIcon> trayIcon;
};

START_JUCE_APPLICATION(MenuBarApp)