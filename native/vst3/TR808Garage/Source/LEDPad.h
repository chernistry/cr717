#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DesignTokens.h"

class LEDPad : public juce::Button
{
public:
    LEDPad() : juce::Button("")
    {
        setClickingTogglesState(true);
    }

    void setCurrentStep(bool isCurrent)
    {
        isCurrentStep = isCurrent;
        repaint();
    }

    void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = getLocalBounds().toFloat().reduced(2.0f);
        
        // Base color
        juce::Colour baseColor = DesignTokens::Colors::bgSecondary;
        if (getToggleState())
            baseColor = DesignTokens::Colors::accent;
        
        // Hover/down states
        if (shouldDrawButtonAsDown)
            baseColor = baseColor.brighter(0.3f);
        else if (shouldDrawButtonAsHighlighted)
            baseColor = baseColor.brighter(0.1f);
        
        // Current step outer glow
        if (isCurrentStep)
        {
            g.setColour(DesignTokens::Colors::success.withAlpha(0.4f));
            g.fillRoundedRectangle(bounds.expanded(3.0f), DesignTokens::Radius::md);
        }
        
        // Button background
        g.setColour(baseColor);
        g.fillRoundedRectangle(bounds, DesignTokens::Radius::md);
        
        // Inner LED circle
        if (getToggleState())
        {
            auto center = bounds.getCentre();
            auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.3f;
            
            // Glow
            g.setColour(DesignTokens::Colors::accent.brighter(0.5f).withAlpha(0.5f));
            g.fillEllipse(center.x - radius * 1.5f, center.y - radius * 1.5f,
                         radius * 3.0f, radius * 3.0f);
            
            // LED
            g.setColour(DesignTokens::Colors::textPrimary);
            g.fillEllipse(center.x - radius, center.y - radius, radius * 2.0f, radius * 2.0f);
        }
        
        // Focus ring
        if (hasKeyboardFocus(true))
        {
            g.setColour(DesignTokens::Colors::borderFocus);
            g.drawRoundedRectangle(bounds, DesignTokens::Radius::md, 2.0f);
        }
    }

private:
    bool isCurrentStep = false;
};
