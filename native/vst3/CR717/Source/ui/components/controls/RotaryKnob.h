#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../../DesignTokens.h"

/**
 * Professional rotary knob with vector graphics
 * Sizes: Small (32px), Medium (48px), Large (64px)
 * 300° sweep (-150° to +150°)
 * Features: Hover glow, active scale, detents, fine mode, keyboard control
 */
class RotaryKnob : public juce::Slider
{
public:
    enum class Size
    {
        Small = 32,
        Medium = 48,
        Large = 64
    };
    
    explicit RotaryKnob(Size size = Size::Medium, const juce::String& label = {})
        : knobSize(size)
        , labelText(label)
    {
        setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        
        // 300° sweep: -150° to +150°
        setRotaryParameters(
            juce::MathConstants<float>::pi * 1.25f,  // -150°
            juce::MathConstants<float>::pi * 2.75f,  // +150°
            true
        );
        
        // Drag sensitivity: 100px = full range
        setMouseDragSensitivity(100);
        
        // Double-click returns to default
        setDoubleClickReturnValue(true, 0.5);
        
        // Enable popup value display
        setPopupDisplayEnabled(true, true, nullptr);
        
        // Detents at 0%, 50%, 100%
        setVelocityBasedMode(false);
        setVelocityModeParameters(1.0, 1, 0.0, false);
        
        // Apply theme
        applyTheme();
        
        // Value label
        valueLabel.setJustificationType(juce::Justification::centred);
        valueLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 12.0f, juce::Font::plain));
        valueLabel.setInterceptsMouseClicks(false, false);
        addAndMakeVisible(valueLabel);
        
        // Parameter label
        if (labelText.isNotEmpty())
        {
            paramLabel.setText(labelText, juce::dontSendNotification);
            paramLabel.setJustificationType(juce::Justification::centred);
            paramLabel.setFont(juce::Font(DesignTokens::Typography::sm));
            paramLabel.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(paramLabel);
        }
        
        // Update value display
        onValueChange = [this] { updateValueLabel(); };
        updateValueLabel();
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getKnobBounds();
        auto centre = bounds.getCentre();
        auto radius = bounds.getWidth() / 2.0f;
        
        // Apply hover/active effects
        float scale = isMouseButtonDown() ? 1.03f : 1.0f;
        if (scale != 1.0f)
        {
            g.addTransform(juce::AffineTransform::scale(scale, scale, centre.x, centre.y));
        }
        
        // Background circle with gradient
        juce::ColourGradient bgGradient(
            Colors::bgTertiary.brighter(0.1f), centre.x, centre.y - radius,
            Colors::bgTertiary.darker(0.1f), centre.x, centre.y + radius,
            false
        );
        g.setGradientFill(bgGradient);
        g.fillEllipse(bounds);
        
        // Outer ring
        g.setColour(Colors::border);
        g.drawEllipse(bounds.reduced(1.0f), 2.0f);
        
        // Track (background arc)
        drawTrack(g, bounds, centre, radius);
        
        // Value arc
        drawValueArc(g, bounds, centre, radius);
        
        // Pointer
        drawPointer(g, centre, radius);
        
        // Center dot
        g.setColour(Colors::textPrimary);
        g.fillEllipse(centre.x - 2, centre.y - 2, 4, 4);
        
        // Hover glow
        if (isMouseOver() && !isMouseButtonDown())
        {
            juce::DropShadow glow(Colors::accent.withAlpha(0.25f), 8, {0, 0});
            glow.drawForRectangle(g, bounds.toNearestInt());
        }
        
        // Focus indicator
        if (hasKeyboardFocus(true))
        {
            g.setColour(Colors::borderFocus);
            g.drawEllipse(bounds.expanded(2.0f), 2.0f);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        auto knobBounds = getKnobBounds();
        
        // Value label (inside knob)
        valueLabel.setBounds(knobBounds.reduced(8).toNearestInt());
        
        // Parameter label (below knob)
        if (labelText.isNotEmpty())
        {
            auto labelBounds = bounds.removeFromBottom(20);
            paramLabel.setBounds(labelBounds);
        }
    }
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        isHovering = true;
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        isHovering = false;
        repaint();
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        juce::Slider::mouseDown(e);
        repaint();
    }
    
    void mouseUp(const juce::MouseEvent& e) override
    {
        juce::Slider::mouseUp(e);
        repaint();
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        // Fine mode with Shift
        if (e.mods.isShiftDown())
        {
            setMouseDragSensitivity(1000); // 10x finer
        }
        else
        {
            setMouseDragSensitivity(100);
        }
        
        juce::Slider::mouseDrag(e);
        
        // Snap to detents
        snapToDetents();
    }
    
    bool keyPressed(const juce::KeyPress& key) override
    {
        double step = 0.01;
        if (key.getModifiers().isShiftDown())
            step = 0.001; // Fine mode
        
        if (key.isKeyCode(juce::KeyPress::upKey) || key.isKeyCode(juce::KeyPress::rightKey))
        {
            setValue(juce::jlimit(0.0, 1.0, getValue() + step));
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::downKey) || key.isKeyCode(juce::KeyPress::leftKey))
        {
            setValue(juce::jlimit(0.0, 1.0, getValue() - step));
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::homeKey))
        {
            setValue(0.0);
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::endKey))
        {
            setValue(1.0);
            return true;
        }
        
        return juce::Slider::keyPressed(key);
    }
    
private:
    juce::Rectangle<float> getKnobBounds() const
    {
        auto bounds = getLocalBounds().toFloat();
        int size = static_cast<int>(knobSize);
        
        // Reserve space for label if present
        if (labelText.isNotEmpty())
            bounds.removeFromBottom(24);
        
        // Center the knob
        float x = (bounds.getWidth() - size) / 2.0f;
        float y = (bounds.getHeight() - size) / 2.0f;
        
        return {x, y, static_cast<float>(size), static_cast<float>(size)};
    }
    
    void drawTrack(juce::Graphics& g, const juce::Rectangle<float>& bounds, 
                   juce::Point<float> centre, float radius)
    {
        using namespace DesignTokens;
        
        float trackRadius = radius - 6.0f;
        float startAngle = juce::MathConstants<float>::pi * 1.25f;
        float endAngle = juce::MathConstants<float>::pi * 2.75f;
        
        juce::Path track;
        track.addCentredArc(centre.x, centre.y, trackRadius, trackRadius,
                           0.0f, startAngle, endAngle, true);
        
        g.setColour(Colors::bgSecondary);
        g.strokePath(track, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));
    }
    
    void drawValueArc(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                      juce::Point<float> centre, float radius)
    {
        using namespace DesignTokens;
        
        float trackRadius = radius - 6.0f;
        float startAngle = juce::MathConstants<float>::pi * 1.25f;
        float endAngle = juce::MathConstants<float>::pi * 2.75f;
        float valueAngle = startAngle + (endAngle - startAngle) * getValue();
        
        juce::Path valueArc;
        valueArc.addCentredArc(centre.x, centre.y, trackRadius, trackRadius,
                              0.0f, startAngle, valueAngle, true);
        
        // Gradient from accent to accentHover
        juce::ColourGradient gradient(
            Colors::accent, centre.x - trackRadius, centre.y,
            Colors::accentHover, centre.x + trackRadius, centre.y,
            false
        );
        g.setGradientFill(gradient);
        g.strokePath(valueArc, juce::PathStrokeType(4.0f, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
    }
    
    void drawPointer(juce::Graphics& g, juce::Point<float> centre, float radius)
    {
        using namespace DesignTokens;
        
        float startAngle = juce::MathConstants<float>::pi * 1.25f;
        float endAngle = juce::MathConstants<float>::pi * 2.75f;
        float angle = startAngle + (endAngle - startAngle) * getValue();
        
        float pointerLength = radius * 0.4f;
        float pointerX = centre.x + std::cos(angle - juce::MathConstants<float>::halfPi) * pointerLength;
        float pointerY = centre.y + std::sin(angle - juce::MathConstants<float>::halfPi) * pointerLength;
        
        g.setColour(Colors::textPrimary);
        g.drawLine(centre.x, centre.y, pointerX, pointerY, 1.0f);
    }
    
    void snapToDetents()
    {
        double value = getValue();
        double threshold = 0.02; // 2% snap threshold
        
        // Detents at 0%, 50%, 100%
        if (std::abs(value - 0.0) < threshold)
            setValue(0.0);
        else if (std::abs(value - 0.5) < threshold)
            setValue(0.5);
        else if (std::abs(value - 1.0) < threshold)
            setValue(1.0);
    }
    
    void updateValueLabel()
    {
        // Format value based on range
        double val = getValue() * (getMaximum() - getMinimum()) + getMinimum();
        juce::String text;
        
        if (std::abs(val) >= 100.0)
            text = juce::String(val, 0);
        else if (std::abs(val) >= 10.0)
            text = juce::String(val, 1);
        else
            text = juce::String(val, 2);
        
        valueLabel.setText(text, juce::dontSendNotification);
    }
    
    void applyTheme()
    {
        using namespace DesignTokens;
        
        valueLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
        paramLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    }
    
    Size knobSize;
    juce::String labelText;
    juce::Label valueLabel;
    juce::Label paramLabel;
    bool isHovering = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RotaryKnob)
};
