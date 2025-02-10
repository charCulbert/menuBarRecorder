#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <BinaryData.h>

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:
    CustomLookAndFeel() {
        // Set UI colors to black with alpha 0.85
        setColour(juce::PopupMenu::backgroundColourId, juce::Colours::black.withAlpha(0.85f));
        setColour(juce::TextButton::buttonColourId, juce::Colours::black.withAlpha(0.85f));
        setColour(juce::ComboBox::backgroundColourId, juce::Colours::black.withAlpha(0.85f));
        setColour(juce::Label::textColourId, juce::Colours::white);

        // Load IBM Plex Mono font from BinaryData
        typeface = juce::Typeface::createSystemTypefaceFor(
            BinaryData::IBMPlexMonoRegular_ttf,
            BinaryData::IBMPlexMonoRegular_ttfSize
        );
        setDefaultSansSerifTypeface(typeface);

        // Default font setup
        defaultFont = juce::Font(typeface).withHeight(20.0f);
    }

    // Button Fonts
    juce::Font getTextButtonFont(juce::TextButton &, int) override { return defaultFont; }
    juce::Font getComboBoxFont(juce::ComboBox &) override { return defaultFont; }
    juce::Font getLabelFont(juce::Label &) override { return defaultFont; }
    juce::Font getSliderPopupFont(juce::Slider &) override { return defaultFont; }
    juce::Font getPopupMenuFont() override { return defaultFont; }

    void drawPopupMenuItem(juce::Graphics &g,
                           const juce::Rectangle<int> &area,
                           bool isSeparator,
                           bool isActive,
                           bool isHighlighted,
                           bool isTicked,
                           bool hasSubMenu,
                           const juce::String &text,
                           const juce::String &shortcutKeyText,
                           const juce::Drawable *icon,
                           const juce::Colour *textColour) override {
        if (isSeparator) {
            // Separator as a white thin line
            g.setColour(juce::Colours::white);
            g.fillRect(area.reduced(5, 0).withHeight(1));
            return;
        }

        // Background (Always black)
        g.setColour(juce::Colours::black.withAlpha(0.85f));
        g.fillRect(area);

        // White border around menu items
        g.setColour(juce::Colours::white);
        g.drawRect(area, 1.0f);

        // Draw highlight overlay (subtle white overlay)
        if (isHighlighted) {
            g.setColour(juce::Colours::white.withAlpha(0.2f));
            g.fillRect(area);
        }

        // Draw text (always white)
        g.setColour(juce::Colours::white);
        g.setFont(getPopupMenuFont());
        g.drawText(text, area.reduced(30, 0), juce::Justification::centredLeft, true);

        // Draw custom check mark if item is ticked (white square)
        if (isTicked) {
            juce::Rectangle<int> tickBox(10, area.getCentreY() - 5, 10, 10);
            g.setColour(juce::Colours::white);
            g.drawRect(tickBox, 2.0f);
        }
    }

private:
    juce::Typeface::Ptr typeface;
    juce::Font defaultFont;
};
