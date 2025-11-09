#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui_tokens.hpp"

class LookAndFeelCR717 : public juce::LookAndFeel_V4
{
public:
    LookAndFeelCR717()
    {
        using namespace ui::tokens;
        
        setColour(juce::ResizableWindow::backgroundColourId, colors::BACKGROUND_PRIMARY);
        setColour(juce::TextButton::buttonColourId, colors::CONTROL_BUTTON);
        setColour(juce::TextButton::buttonOnColourId, colors::CONTROL_BUTTONACTIVE);
        setColour(juce::TextButton::textColourOffId, colors::TEXT_PRIMARY);
        setColour(juce::TextButton::textColourOnId, colors::TEXT_PRIMARY);
        setColour(juce::Label::textColourId, colors::TEXT_PRIMARY);
        setColour(juce::Slider::backgroundColourId, colors::CONTROL_KNOB);
        setColour(juce::Slider::thumbColourId, colors::CONTROL_FADERTHUMB);
        setColour(juce::Slider::trackColourId, colors::CONTROL_KNOBARC);
        setColour(juce::Slider::rotarySliderFillColourId, colors::CONTROL_KNOBARC);
        setColour(juce::Slider::rotarySliderOutlineColourId, colors::CONTROL_KNOB);
        setColour(juce::ComboBox::backgroundColourId, colors::BACKGROUND_SECONDARY);
        setColour(juce::ComboBox::textColourId, colors::TEXT_PRIMARY);
        setColour(juce::ComboBox::outlineColourId, colors::BORDER_DEFAULT);
    }
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override
    {
        using namespace ui::tokens;
        
        auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = juce::jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;
        
        // Background arc
        juce::Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                   arcRadius, arcRadius, 0.0f,
                                   rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(colors::CONTROL_KNOB);
        g.strokePath(backgroundArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        
        // Value arc
        if (slider.isEnabled())
        {
            juce::Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(), bounds.getCentreY(),
                                  arcRadius, arcRadius, 0.0f,
                                  rotaryStartAngle, toAngle, true);
            g.setColour(colors::CONTROL_KNOBARC);
            g.strokePath(valueArc, juce::PathStrokeType(lineW, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));
        }
        
        // Pointer
        juce::Path pointer;
        auto pointerLength = radius * 0.33f;
        auto pointerThickness = 2.0f;
        pointer.addRectangle(-pointerThickness * 0.5f, -radius, pointerThickness, pointerLength);
        pointer.applyTransform(juce::AffineTransform::rotation(toAngle).translated(bounds.getCentreX(), bounds.getCentreY()));
        g.setColour(slider.isEnabled() ? colors::TEXT_PRIMARY : colors::TEXT_DISABLED);
        g.fillPath(pointer);
    }
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        using namespace ui::tokens;
        
        if (slider.isBar())
        {
            g.setColour(slider.findColour(juce::Slider::trackColourId));
            g.fillRect(slider.isHorizontal() ? juce::Rectangle<float>(static_cast<float>(x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
                                             : juce::Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));
        }
        else
        {
            auto trackWidth = juce::jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);
            
            juce::Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
                                         slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));
            
            juce::Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
                                       slider.isHorizontal() ? startPoint.y : (float)y);
            
            juce::Path backgroundTrack;
            backgroundTrack.startNewSubPath(startPoint);
            backgroundTrack.lineTo(endPoint);
            g.setColour(colors::CONTROL_FADER);
            g.strokePath(backgroundTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});
            
            juce::Path valueTrack;
            juce::Point<float> minPoint, maxPoint, thumbPoint;
            
            if (slider.isHorizontal())
            {
                minPoint = startPoint;
                maxPoint = {sliderPos, startPoint.y};
            }
            else
            {
                minPoint = {startPoint.x, sliderPos};
                maxPoint = endPoint;
            }
            
            valueTrack.startNewSubPath(minPoint);
            valueTrack.lineTo(maxPoint);
            g.setColour(colors::CONTROL_FADERTHUMB);
            g.strokePath(valueTrack, {trackWidth, juce::PathStrokeType::curved, juce::PathStrokeType::rounded});
            
            // Thumb
            auto thumbWidth = trackWidth * 2.0f;
            g.setColour(colors::CONTROL_FADERTHUMB);
            g.fillEllipse(juce::Rectangle<float>(thumbWidth, thumbWidth).withCentre(slider.isHorizontal() ? maxPoint : minPoint));
        }
    }
    
    juce::Font getTextButtonFont(juce::TextButton&, int buttonHeight) override
    {
        return juce::Font(ui::tokens::typography::SIZE_BODY);
    }
    
    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(ui::tokens::typography::SIZE_LABEL);
    }
};
