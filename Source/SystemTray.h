// SystemTray.h
#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class TrayIcon : public juce::SystemTrayIconComponent
{
public:
    explicit TrayIcon(std::function<void()> onClick) : onClickCallback(onClick)
    {
        // Create icon image
        juce::Image icon(juce::Image::ARGB, 32, 32, true);
        juce::Graphics g(icon);
        g.fillAll(juce::Colours::black);
        setIconImage(icon, icon);
    }

    void mouseDown(const juce::MouseEvent& event) override
    {
        if (event.mods.isLeftButtonDown() && onClickCallback)
        {
            onClickCallback();
        }
    }

private:
    std::function<void()> onClickCallback;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrayIcon)
};