#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "DesignTokens.h"

class CustomKnob : public juce::Slider
{
public:
    CustomKnob()
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 18);
        setRotaryParameters(juce::MathConstants<float>::pi * 1.25f,
                           juce::MathConstants<float>::pi * 2.75f,
                           true);
        setMouseDragSensitivity(150);
        setDoubleClickReturnValue(true, 0.5);
        setPopupDisplayEnabled(true, true, nullptr);
        
        setColour(juce::Slider::rotarySliderFillColourId, DesignTokens::Colors::accent);
        setColour(juce::Slider::rotarySliderOutlineColourId, DesignTokens::Colors::bgTertiary);
        setColour(juce::Slider::thumbColourId, DesignTokens::Colors::textPrimary);
        setColour(juce::Slider::textBoxTextColourId, DesignTokens::Colors::textSecondary);
        setColour(juce::Slider::textBoxBackgroundColourId, DesignTokens::Colors::bgSecondary);
        setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        auto centre = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f - 10.0f;
        
        // Background circle
        g.setColour(DesignTokens::Colors::bgSecondary);
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f);
        
        // Track (outline)
        auto trackWidth = 3.0f;
        g.setColour(DesignTokens::Colors::bgTertiary);
        juce::Path trackPath;
        trackPath.addCentredArc(centre.x, centre.y, radius, radius,
                               0.0f,
                               getRotaryParameters().startAngleRadians,
                               getRotaryParameters().endAngleRadians,
                               true);
        g.strokePath(trackPath, juce::PathStrokeType(trackWidth));
        
        // Value arc
        auto toAngle = getRotaryParameters().startAngleRadians +
                      (getRotaryParameters().endAngleRadians - getRotaryParameters().startAngleRadians) *
                      valueToProportionOfLength(getValue());
        
        juce::Path valuePath;
        valuePath.addCentredArc(centre.x, centre.y, radius, radius,
                               0.0f,
                               getRotaryParameters().startAngleRadians,
                               toAngle,
                               true);
        
        // Hover/active glow
        if (isMouseOverOrDragging())
        {
            g.setColour(DesignTokens::Colors::accentHover.withAlpha(0.3f));
            g.strokePath(valuePath, juce::PathStrokeType(trackWidth + 4.0f));
        }
        
        g.setColour(DesignTokens::Colors::accent);
        g.strokePath(valuePath, juce::PathStrokeType(trackWidth));
        
        // Tick marks
        g.setColour(DesignTokens::Colors::textMuted);
        for (int i = 0; i <= 10; ++i)
        {
            auto angle = getRotaryParameters().startAngleRadians +
                        (getRotaryParameters().endAngleRadians - getRotaryParameters().startAngleRadians) * (i / 10.0f);
            
            auto tickLength = (i % 5 == 0) ? 6.0f : 3.0f;
            auto innerRadius = radius - tickLength;
            
            juce::Point<float> inner(centre.x + innerRadius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                    centre.y + innerRadius * std::sin(angle - juce::MathConstants<float>::halfPi));
            juce::Point<float> outer(centre.x + radius * std::cos(angle - juce::MathConstants<float>::halfPi),
                                    centre.y + radius * std::sin(angle - juce::MathConstants<float>::halfPi));
            
            g.drawLine(inner.x, inner.y, outer.x, outer.y, 1.0f);
        }
        
        // Pointer
        auto pointerLength = radius * 0.6f;
        auto pointerThickness = 3.0f;
        juce::Point<float> pointerEnd(centre.x + pointerLength * std::cos(toAngle - juce::MathConstants<float>::halfPi),
                                     centre.y + pointerLength * std::sin(toAngle - juce::MathConstants<float>::halfPi));
        
        g.setColour(DesignTokens::Colors::textPrimary);
        g.drawLine(centre.x, centre.y, pointerEnd.x, pointerEnd.y, pointerThickness);
        
        // Center dot
        g.fillEllipse(centre.x - 3.0f, centre.y - 3.0f, 6.0f, 6.0f);
        
        // Focus ring
        if (hasKeyboardFocus(true))
        {
            g.setColour(DesignTokens::Colors::borderFocus);
            g.drawEllipse(centre.x - radius - 2.0f, centre.y - radius - 2.0f,
                         (radius + 2.0f) * 2.0f, (radius + 2.0f) * 2.0f, 2.0f);
        }
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isAltDown())
            setMouseDragSensitivity(600); // Fine adjust
        else if (e.mods.isShiftDown())
            setMouseDragSensitivity(50);  // Coarse adjust
        else
            setMouseDragSensitivity(150); // Normal
        
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
        
        if (key.isKeyCode(juce::KeyPress::upKey) || key.isKeyCode(juce::KeyPress::rightKey))
        {
            setValue(juce::jlimit(getMinimum(), getMaximum(), getValue() + delta * (getMaximum() - getMinimum())));
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::downKey) || key.isKeyCode(juce::KeyPress::leftKey))
        {
            setValue(juce::jlimit(getMinimum(), getMaximum(), getValue() - delta * (getMaximum() - getMinimum())));
            return true;
        }
        
        return juce::Slider::keyPressed(key);
    }
};
