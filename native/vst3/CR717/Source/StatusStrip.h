#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui_tokens.hpp"

class StatusStrip : public juce::Component
{
public:
    enum class Level { Info, Warning, Error };
    
    void showMessage(const juce::String& msg, Level level = Level::Info, int durationMs = 3000)
    {
        message = msg;
        messageLevel = level;
        startTimer(durationMs);
        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        if (message.isEmpty()) return;
        
        g.fillAll(ui::tokens::colors::BACKGROUND_SECONDARY);
        
        auto color = messageLevel == Level::Error ? ui::tokens::colors::ACCENT_ERROR :
                     messageLevel == Level::Warning ? ui::tokens::colors::ACCENT_WARNING :
                     ui::tokens::colors::TEXT_SECONDARY;
        
        g.setColour(color);
        g.setFont(ui::tokens::typography::SIZE_SMALL);
        g.drawText(message, getLocalBounds().reduced(8, 4), juce::Justification::centredLeft);
    }
    
private:
    void timerCallback() override
    {
        message.clear();
        stopTimer();
        repaint();
    }
    
    juce::String message;
    Level messageLevel = Level::Info;
};
