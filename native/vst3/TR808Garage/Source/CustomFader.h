#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DesignTokens.h"

class CustomFader : public juce::Slider
{
public:
    CustomFader()
    {
        setSliderStyle(juce::Slider::LinearVertical);
        setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        setMouseDragSensitivity(150);
        setDoubleClickReturnValue(true, 0.8);
        setPopupDisplayEnabled(true, true, nullptr);
        
        setColour(juce::Slider::trackColourId, DesignTokens::Colors::bgTertiary);
        setColour(juce::Slider::thumbColourId, DesignTokens::Colors::accent);
        setColour(juce::Slider::textBoxTextColourId, DesignTokens::Colors::textSecondary);
        setColour(juce::Slider::textBoxBackgroundColourId, DesignTokens::Colors::bgSecondary);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto trackBounds = bounds.reduced(bounds.getWidth() * 0.3f, 10.0f);
        
        // Track background
        g.setColour(DesignTokens::Colors::bgTertiary);
        g.fillRoundedRectangle(trackBounds, DesignTokens::Radius::sm);
        
        // Inner highlight
        g.setColour(DesignTokens::Colors::bgElevated.brighter(0.1f));
        g.drawLine(trackBounds.getX() + 1.0f, trackBounds.getY(),
                  trackBounds.getX() + 1.0f, trackBounds.getBottom(), 1.0f);
        
        // Value fill
        auto proportion = valueToProportionOfLength(getValue());
        auto fillHeight = trackBounds.getHeight() * proportion;
        auto fillBounds = trackBounds.removeFromBottom(fillHeight);
        
        juce::ColourGradient gradient(DesignTokens::Colors::accent.darker(0.2f),
                                     fillBounds.getX(), fillBounds.getBottom(),
                                     DesignTokens::Colors::accent,
                                     fillBounds.getX(), fillBounds.getY(),
                                     false);
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(fillBounds, DesignTokens::Radius::sm);
        
        // Thumb
        auto thumbY = trackBounds.getY() + trackBounds.getHeight() * (1.0f - proportion);
        auto thumbBounds = juce::Rectangle<float>(bounds.getX(), thumbY - 10.0f,
                                                  bounds.getWidth(), 20.0f);
        
        // Thumb shadow
        if (isMouseOverOrDragging())
        {
            g.setColour(DesignTokens::Colors::accentHover.withAlpha(0.3f));
            g.fillRoundedRectangle(thumbBounds.expanded(2.0f), DesignTokens::Radius::md);
        }
        
        g.setColour(DesignTokens::Colors::accent);
        g.fillRoundedRectangle(thumbBounds, DesignTokens::Radius::md);
        
        // Thumb highlight
        g.setColour(DesignTokens::Colors::textPrimary.withAlpha(0.3f));
        g.fillRoundedRectangle(thumbBounds.reduced(2.0f, 8.0f), DesignTokens::Radius::sm);
        
        // Focus ring
        if (hasKeyboardFocus(true))
        {
            g.setColour(DesignTokens::Colors::borderFocus);
            g.drawRoundedRectangle(trackBounds.expanded(2.0f), DesignTokens::Radius::sm, 2.0f);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isAltDown())
            setMouseDragSensitivity(600);
        else if (e.mods.isShiftDown())
            setMouseDragSensitivity(50);
        else
            setMouseDragSensitivity(150);
        
        juce::Slider::mouseDown(e);
    }

    void mouseUp(const juce::MouseEvent& e) override
    {
        setMouseDragSensitivity(150);
        juce::Slider::mouseUp(e);
    }

    bool keyPressed(const juce::KeyPress& key) override
    {
        auto delta = 0.01;
        if (key.getModifiers().isShiftDown())
            delta = 0.1;
        
        if (key.isKeyCode(juce::KeyPress::upKey))
        {
            setValue(juce::jlimit(getMinimum(), getMaximum(), getValue() + delta * (getMaximum() - getMinimum())));
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::downKey))
        {
            setValue(juce::jlimit(getMinimum(), getMaximum(), getValue() - delta * (getMaximum() - getMinimum())));
            return true;
        }
        
        return juce::Slider::keyPressed(key);
    }
};
