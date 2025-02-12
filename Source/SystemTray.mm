#include "SystemTray.h"

#if JUCE_MAC
#import <Cocoa/Cocoa.h>
#endif

TrayIcon::TrayIcon(std::function<void()> onClick)
    : onClickCallback(onClick)
{
    juce::Image icon(juce::Image::ARGB, 32, 32, true);
    juce::Graphics g(icon);

    // Fill background with transparency
    g.fillAll(juce::Colours::transparentBlack);

    // Draw a solid white dot in the center
    const int centerX = icon.getWidth() / 2;
    const int centerY = icon.getHeight() / 2;
    const int radius = 6;

    g.setColour(juce::Colours::white);
    g.fillEllipse(centerX - radius, centerY - radius, radius * 2, radius * 2);

    setIconImage(icon, icon);
}




void TrayIcon::mouseDown(const juce::MouseEvent& event)
{
    if (onClickCallback)
    {
        onClickCallback();
    }
}

juce::Point<int> TrayIcon::getTrayIconPosition() const
{
#if JUCE_MAC
    NSStatusBar *statusBar = [NSStatusBar systemStatusBar];
    NSArray *items = [statusBar valueForKey:@"items"];

    for (NSStatusItem *item in items) {
        NSButton *button = [item button];
        if (button) {
            NSWindow *window = [button window];
            if (window) {
                // Get button frame in window coordinates
                NSRect frame = [button frame];

                // Convert to screen coordinates
                NSPoint screenPoint = [window convertPointToScreen:frame.origin];

                // Get screen dimensions and flip Y coordinate
                NSRect screenRect = [[NSScreen mainScreen] frame];
                CGFloat flippedY = screenRect.size.height - (screenPoint.y + frame.size.height);

                // Create point with explicit casting to avoid warnings
                juce::Point<int> position(static_cast<int>(screenPoint.x),
                                        static_cast<int>(flippedY));

                DBG("Tray Icon Position: " << position.toString());
                return position;
            }
        }
    }
#endif
    return {0, 0};
}