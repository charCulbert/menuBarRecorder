#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "SystemAudioDeviceSwitcher.h"
#include "MainContentComponent.h"
#include "AudioHandler.h"
#include "CustomLookAndFeel.h"
#include "SystemTray.h"

class MenuBarApp : public juce::JUCEApplication {
public:
    MenuBarApp() {}

    const juce::String getApplicationName() override { return "MenuBarApp"; }
    const juce::String getApplicationVersion() override { return "1.0.0"; }
    CustomLookAndFeel customLookAndFeel;

    void initialise(const juce::String&) override {
        juce::LookAndFeel::setDefaultLookAndFeel(&customLookAndFeel);

        DBG("Initializing app...");

        // Create system tray icon first
        trayIcon = std::make_unique<TrayIcon>([this]() {
            DBG("Tray icon clicked!");
            toggleMainContent();
        });

        // Create main component
        mainContent = std::make_unique<MainContentComponent>();
        mainContent->setOpaque(false);
        mainContent->setSize(200, 150);

        // Add to desktop with window properties
        mainContent->addToDesktop(juce::ComponentPeer::windowIsTemporary |
                                juce::ComponentPeer::windowHasDropShadow);

        // Position under the tray icon
        auto iconPos = trayIcon->getTrayIconPosition();
        DBG("Icon position: " << iconPos.toString());
        mainContent->setTopLeftPosition(iconPos.x, iconPos.y + 25);

        // Start hidden
        mainContent->setVisible(false);

        DBG("Initialization complete");
    }

    void shutdown() override {
        DBG("Shutting down...");
        trayIcon = nullptr;
        mainContent = nullptr;
    }

private:
    std::unique_ptr<MainContentComponent> mainContent;
    std::unique_ptr<TrayIcon> trayIcon;

    void toggleMainContent() {
        if (!mainContent->isVisible()) {
            DBG("Making window visible");

            // Update position before showing
            auto iconPos = trayIcon->getTrayIconPosition();
            mainContent->setTopLeftPosition(iconPos.x, iconPos.y + 25);

            mainContent->setVisible(true);
            mainContent->toFront(true);
#if JUCE_MAC
            juce::Process::makeForegroundProcess();
#endif
        } else {
            DBG("Hiding window");
            mainContent->setVisible(false);
        }
    }
};

START_JUCE_APPLICATION(MenuBarApp)