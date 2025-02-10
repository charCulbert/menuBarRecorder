#pragma once
#include <juce_gui_extra/juce_gui_extra.h>

class TrayIcon : public juce::SystemTrayIconComponent
{
public:
    explicit TrayIcon(std::function<void()> onClick);
    void mouseDown(const juce::MouseEvent& event) override;
    juce::Point<int> getTrayIconPosition() const;

private:
    std::function<void()> onClickCallback;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TrayIcon)
};