//
// Created by Charlie Culbert on 2/10/25.
//
#pragma once
#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

class RecordButton : public juce::Button
{
public:
    RecordButton() : juce::Button("RecordButton") {}

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat();
        float size = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.6f; // Keep it square
        auto center = bounds.getCentre();

        g.setColour(juce::Colours::white);

        if (getToggleState())
        {
            // Draw a square (stop icon)
            g.fillRect(center.x - size * 0.5f, center.y - size * 0.5f, size, size);
        }
        else
        {
            // Draw a circle (record icon)
            g.fillEllipse(center.x - size * 0.5f, center.y - size * 0.5f, size, size);
        }
    }
};
