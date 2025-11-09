#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <atomic>
#include "DesignTokens.h"

class LevelMeter : public juce::Component, private juce::Timer
{
public:
    LevelMeter()
    {
        startTimerHz(60);
    }

    void setLevel(float newLevel)
    {
        currentLevel.store(newLevel, std::memory_order_relaxed);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Background
        g.setColour(DesignTokens::Colors::bgTertiary);
        g.fillRoundedRectangle(bounds, DesignTokens::Radius::sm);
        
        // Get current levels
        float level = currentLevel.load(std::memory_order_relaxed);
        float displayLevel = juce::jlimit(0.0f, 1.0f, level);
        
        // Update peak hold
        if (displayLevel > peakLevel)
        {
            peakLevel = displayLevel;
            peakHoldTime = 60; // 1 second at 60Hz
        }
        
        // Update RMS (simple moving average)
        rmsLevel = rmsLevel * 0.95f + displayLevel * 0.05f;
        
        // Draw RMS bar
        auto rmsHeight = bounds.getHeight() * rmsLevel;
        auto rmsBounds = bounds.removeFromBottom(rmsHeight);
        
        // Color zones: green (<-12dB = 0.25), yellow (-12..-3dB = 0.25..0.7), red (>-3dB = 0.7)
        juce::Colour barColor;
        if (rmsLevel < 0.25f)
            barColor = DesignTokens::Colors::success;
        else if (rmsLevel < 0.7f)
            barColor = DesignTokens::Colors::warning;
        else
            barColor = DesignTokens::Colors::danger;
        
        juce::ColourGradient gradient(barColor.darker(0.3f), rmsBounds.getX(), rmsBounds.getBottom(),
                                     barColor, rmsBounds.getX(), rmsBounds.getY(), false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(rmsBounds, DesignTokens::Radius::sm);
        
        // Draw peak hold line
        if (peakLevel > 0.01f)
        {
            auto peakY = bounds.getY() + bounds.getHeight() * (1.0f - peakLevel);
            g.setColour(DesignTokens::Colors::textPrimary);
            g.drawLine(bounds.getX(), peakY, bounds.getRight(), peakY, 2.0f);
        }
        
        // Draw scale marks
        g.setColour(DesignTokens::Colors::textMuted.withAlpha(0.3f));
        for (int i = 1; i < 4; ++i)
        {
            auto y = bounds.getY() + bounds.getHeight() * (i / 4.0f);
            g.drawLine(bounds.getX(), y, bounds.getRight(), y, 1.0f);
        }
    }

private:
    void timerCallback() override
    {
        // Decay peak hold
        if (peakHoldTime > 0)
            peakHoldTime--;
        else if (peakLevel > 0.0f)
            peakLevel *= 0.95f;
        
        // Decay RMS
        rmsLevel *= 0.98f;
        
        repaint();
    }

    std::atomic<float> currentLevel{0.0f};
    float peakLevel = 0.0f;
    float rmsLevel = 0.0f;
    int peakHoldTime = 0;
};
