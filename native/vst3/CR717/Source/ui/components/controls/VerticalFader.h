#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../../DesignTokens.h"

/**
 * Professional vertical fader with graduations
 * Sizes: Narrow (24x140), Standard (28x180), Wide (32x220)
 * Features: Graduations every 6dB, center detent for pan, hover effects
 */
class VerticalFader : public juce::Slider
{
public:
    enum class Width
    {
        Narrow = 24,
        Standard = 28,
        Wide = 32
    };
    
    enum class Height
    {
        Narrow = 140,
        Standard = 180,
        Wide = 220
    };
    
    explicit VerticalFader(Width w = Width::Standard, Height h = Height::Standard,
                          const juce::String& label = {}, bool isPan = false)
        : faderWidth(w)
        , faderHeight(h)
        , labelText(label)
        , isPanMode(isPan)
    {
        setSliderStyle(juce::Slider::LinearVertical);
        setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        
        // Drag sensitivity
        setMouseDragSensitivity(100);
        
        // Center detent for pan mode
        if (isPanMode)
        {
            setRange(-1.0, 1.0, 0.01);
            setValue(0.0);
            setDoubleClickReturnValue(true, 0.0);
        }
        else
        {
            setRange(0.0, 1.0, 0.01);
            setValue(0.8);
            setDoubleClickReturnValue(true, 0.8);
        }
        
        // Apply theme
        applyTheme();
        
        // Value label
        valueLabel.setJustificationType(juce::Justification::centred);
        valueLabel.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), 11.0f, juce::Font::plain));
        valueLabel.setInterceptsMouseClicks(false, false);
        addAndMakeVisible(valueLabel);
        
        // Parameter label
        if (labelText.isNotEmpty())
        {
            paramLabel.setText(labelText, juce::dontSendNotification);
            paramLabel.setJustificationType(juce::Justification::centred);
            paramLabel.setFont(juce::Font(DesignTokens::Typography::xs));
            paramLabel.setInterceptsMouseClicks(false, false);
            addAndMakeVisible(paramLabel);
        }
        
        onValueChange = [this] { updateValueLabel(); };
        updateValueLabel();
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getFaderBounds();
        auto trackBounds = getTrackBounds(bounds);
        
        // Track background
        g.setColour(Colors::bgSecondary);
        g.fillRoundedRectangle(trackBounds, Radius::sm);
        
        // Track border
        g.setColour(Colors::border);
        g.drawRoundedRectangle(trackBounds, Radius::sm, 1.0f);
        
        // Graduations
        drawGraduations(g, trackBounds);
        
        // Fill (value indicator)
        drawFill(g, trackBounds);
        
        // Thumb
        drawThumb(g, bounds);
        
        // Hover glow
        if (isMouseOver() && !isMouseButtonDown())
        {
            g.setColour(Colors::accent.withAlpha(0.15f));
            g.fillRoundedRectangle(trackBounds.expanded(2.0f), Radius::sm);
        }
        
        // Focus indicator
        if (hasKeyboardFocus(true))
        {
            g.setColour(Colors::borderFocus);
            g.drawRoundedRectangle(bounds.expanded(2.0f), Radius::md, 2.0f);
        }
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        
        // Value label (top)
        valueLabel.setBounds(bounds.removeFromTop(16));
        
        // Parameter label (bottom)
        if (labelText.isNotEmpty())
        {
            paramLabel.setBounds(bounds.removeFromBottom(16));
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
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        // Fine mode with Shift
        if (e.mods.isShiftDown())
            setMouseDragSensitivity(1000);
        else
            setMouseDragSensitivity(100);
        
        juce::Slider::mouseDrag(e);
        
        // Snap to center detent in pan mode
        if (isPanMode)
        {
            double val = getValue();
            if (std::abs(val) < 0.05)
                setValue(0.0);
        }
    }
    
    bool keyPressed(const juce::KeyPress& key) override
    {
        double step = 0.01;
        if (key.getModifiers().isShiftDown())
            step = 0.001;
        
        double range = getMaximum() - getMinimum();
        
        if (key.isKeyCode(juce::KeyPress::upKey))
        {
            setValue(getValue() + step * range);
            return true;
        }
        else if (key.isKeyCode(juce::KeyPress::downKey))
        {
            setValue(getValue() - step * range);
            return true;
        }
        
        return juce::Slider::keyPressed(key);
    }
    
private:
    juce::Rectangle<float> getFaderBounds() const
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Remove space for labels
        bounds.removeFromTop(20);
        if (labelText.isNotEmpty())
            bounds.removeFromBottom(20);
        
        return bounds;
    }
    
    juce::Rectangle<float> getTrackBounds(const juce::Rectangle<float>& faderBounds) const
    {
        int w = static_cast<int>(faderWidth);
        float trackWidth = w * 0.3f; // Track is 30% of fader width
        float x = faderBounds.getCentreX() - trackWidth / 2.0f;
        
        return {x, faderBounds.getY(), trackWidth, faderBounds.getHeight()};
    }
    
    void drawGraduations(juce::Graphics& g, const juce::Rectangle<float>& trackBounds)
    {
        using namespace DesignTokens;
        
        if (isPanMode)
        {
            // Center line for pan
            float centerY = trackBounds.getCentreY();
            g.setColour(Colors::textMuted);
            g.drawLine(trackBounds.getX() - 4, centerY, 
                      trackBounds.getRight() + 4, centerY, 1.0f);
        }
        else
        {
            // Graduations every 6dB (assuming 0-1 range maps to dB)
            g.setColour(Colors::textMuted);
            
            for (float pos = 0.0f; pos <= 1.0f; pos += 0.1f) // Every 10%
            {
                float y = trackBounds.getBottom() - pos * trackBounds.getHeight();
                float lineLength = (std::abs(pos - 0.8f) < 0.01f) ? 6.0f : 3.0f; // Bold at 0dB (80%)
                
                g.drawLine(trackBounds.getRight() + 2, y,
                          trackBounds.getRight() + 2 + lineLength, y, 1.0f);
            }
        }
    }
    
    void drawFill(juce::Graphics& g, const juce::Rectangle<float>& trackBounds)
    {
        using namespace DesignTokens;
        
        double normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        
        if (isPanMode)
        {
            // Pan mode: fill from center
            float centerY = trackBounds.getCentreY();
            float valueY = trackBounds.getBottom() - normalizedValue * trackBounds.getHeight();
            
            auto fillBounds = trackBounds.withTop(std::min(centerY, valueY))
                                        .withBottom(std::max(centerY, valueY));
            
            g.setColour(Colors::accent.withAlpha(0.5f));
            g.fillRoundedRectangle(fillBounds.reduced(1.0f), Radius::sm);
        }
        else
        {
            // Normal mode: fill from bottom
            float fillHeight = normalizedValue * trackBounds.getHeight();
            auto fillBounds = trackBounds.withTop(trackBounds.getBottom() - fillHeight);
            
            juce::ColourGradient gradient(
                Colors::accent, fillBounds.getCentreX(), fillBounds.getBottom(),
                Colors::accentHover, fillBounds.getCentreX(), fillBounds.getY(),
                false
            );
            g.setGradientFill(gradient);
            g.fillRoundedRectangle(fillBounds.reduced(1.0f), Radius::sm);
        }
    }
    
    void drawThumb(juce::Graphics& g, const juce::Rectangle<float>& faderBounds)
    {
        using namespace DesignTokens;
        
        double normalizedValue = (getValue() - getMinimum()) / (getMaximum() - getMinimum());
        float thumbY = faderBounds.getBottom() - normalizedValue * faderBounds.getHeight();
        
        int w = static_cast<int>(faderWidth);
        juce::Rectangle<float> thumbBounds(
            faderBounds.getCentreX() - w / 2.0f,
            thumbY - 5.0f,
            static_cast<float>(w),
            10.0f
        );
        
        // Thumb shadow
        juce::DropShadow shadow(juce::Colours::black.withAlpha(0.3f), 4, {0, 1});
        shadow.drawForRectangle(g, thumbBounds.toNearestInt());
        
        // Thumb gradient
        juce::ColourGradient thumbGradient(
            Colors::bgElevated.brighter(0.2f), thumbBounds.getCentreX(), thumbBounds.getY(),
            Colors::bgElevated.darker(0.1f), thumbBounds.getCentreX(), thumbBounds.getBottom(),
            false
        );
        g.setGradientFill(thumbGradient);
        g.fillRoundedRectangle(thumbBounds, Radius::md);
        
        // Thumb border
        g.setColour(isMouseButtonDown() ? Colors::accent : Colors::border);
        g.drawRoundedRectangle(thumbBounds, Radius::md, 1.0f);
        
        // Grip lines
        g.setColour(Colors::textMuted);
        float gripY = thumbBounds.getCentreY();
        for (int i = -1; i <= 1; ++i)
        {
            g.drawLine(thumbBounds.getX() + 6, gripY + i * 2,
                      thumbBounds.getRight() - 6, gripY + i * 2, 0.5f);
        }
    }
    
    void updateValueLabel()
    {
        double val = getValue();
        juce::String text;
        
        if (isPanMode)
        {
            if (val < -0.01)
                text = "L" + juce::String(std::abs(val) * 100.0, 0);
            else if (val > 0.01)
                text = "R" + juce::String(val * 100.0, 0);
            else
                text = "C";
        }
        else
        {
            // Convert to dB (assuming 0-1 maps to -inf to 0dB)
            double db = val > 0.001 ? 20.0 * std::log10(val) : -60.0;
            text = juce::String(db, 1) + "dB";
        }
        
        valueLabel.setText(text, juce::dontSendNotification);
    }
    
    void applyTheme()
    {
        using namespace DesignTokens;
        
        valueLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
        paramLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    }
    
    Width faderWidth;
    Height faderHeight;
    juce::String labelText;
    bool isPanMode;
    juce::Label valueLabel;
    juce::Label paramLabel;
    bool isHovering = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VerticalFader)
};
