#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../../DesignTokens.h"

/**
 * Individual step sequencer pad
 * Size: 36x36px with 4px gaps, 4px corner radius
 * States: Off, On, Accent, Active (playing), Hover
 * Interactions: Click toggle, Alt-click accent, drag paint
 */
class StepPad : public juce::Component
{
public:
    enum class State
    {
        Off,      // #2a2f36
        On,       // #3c434b
        Accent    // accent color (bright)
    };
    
    StepPad(int stepIndex, int voiceIndex)
        : step(stepIndex)
        , voice(voiceIndex)
    {
        setSize(36, 36);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds().toFloat().reduced(2.0f); // 4px gap = 2px per side
        
        // Determine color based on state
        juce::Colour padColor;
        switch (currentState)
        {
            case State::Off:
                padColor = juce::Colour(0xff2a2f36);
                break;
            case State::On:
                padColor = juce::Colour(0xff3c434b);
                break;
            case State::Accent:
                padColor = Colors::accent;
                break;
        }
        
        // Active (playing) highlight
        if (isActive)
        {
            padColor = padColor.brighter(0.3f);
            
            // Glow effect
            juce::DropShadow glow(Colors::accent.withAlpha(0.6f), 8, {0, 0});
            glow.drawForRoundedRectangle(g, bounds.toNearestInt(), Radius::md);
        }
        
        // Hover effect
        if (isMouseOver() && !isActive)
        {
            padColor = padColor.brighter(0.15f);
        }
        
        // Fill pad
        g.setColour(padColor);
        g.fillRoundedRectangle(bounds, Radius::md);
        
        // Border
        if (currentState != State::Off || isMouseOver())
        {
            g.setColour(padColor.brighter(0.2f));
            g.drawRoundedRectangle(bounds, Radius::md, 1.0f);
        }
        
        // Focus indicator
        if (hasKeyboardFocus(true))
        {
            g.setColour(Colors::borderFocus);
            g.drawRoundedRectangle(bounds.expanded(1.0f), Radius::md, 2.0f);
        }
        
        // Accent indicator (small dot in corner)
        if (currentState == State::Accent)
        {
            g.setColour(juce::Colours::white.withAlpha(0.8f));
            g.fillEllipse(bounds.getRight() - 8, bounds.getY() + 2, 4, 4);
        }
    }
    
    void mouseEnter(const juce::MouseEvent&) override
    {
        repaint();
    }
    
    void mouseExit(const juce::MouseEvent&) override
    {
        repaint();
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isAltDown())
        {
            // Alt-click: toggle accent
            if (currentState == State::Accent)
                setState(State::Off);
            else
                setState(State::Accent);
        }
        else if (e.mods.isShiftDown())
        {
            // Shift-click: erase
            setState(State::Off);
        }
        else
        {
            // Normal click: toggle on/off
            if (currentState == State::Off)
                setState(State::On);
            else if (currentState == State::On)
                setState(State::Off);
            else
                setState(State::Off);
        }
        
        if (onStateChanged)
            onStateChanged(step, voice, currentState);
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        // Paint mode: apply current state to hovered pads
        auto* padUnderMouse = dynamic_cast<StepPad*>(
            getParentComponent()->getComponentAt(e.getEventRelativeTo(getParentComponent()).getPosition())
        );
        
        if (padUnderMouse && padUnderMouse != this)
        {
            if (e.mods.isShiftDown())
                padUnderMouse->setState(State::Off);
            else if (e.mods.isAltDown())
                padUnderMouse->setState(State::Accent);
            else
                padUnderMouse->setState(State::On);
            
            if (onStateChanged)
                onStateChanged(padUnderMouse->step, padUnderMouse->voice, padUnderMouse->currentState);
        }
    }
    
    bool keyPressed(const juce::KeyPress& key) override
    {
        if (key.isKeyCode(juce::KeyPress::spaceKey) || key.isKeyCode(juce::KeyPress::returnKey))
        {
            // Toggle state
            if (currentState == State::Off)
                setState(State::On);
            else
                setState(State::Off);
            
            if (onStateChanged)
                onStateChanged(step, voice, currentState);
            
            return true;
        }
        
        return false;
    }
    
    void setState(State newState)
    {
        if (currentState != newState)
        {
            currentState = newState;
            repaint();
        }
    }
    
    State getState() const { return currentState; }
    
    void setActive(bool active)
    {
        if (isActive != active)
        {
            isActive = active;
            repaint();
        }
    }
    
    bool getActive() const { return isActive; }
    
    int getStepIndex() const { return step; }
    int getVoiceIndex() const { return voice; }
    
    std::function<void(int step, int voice, State state)> onStateChanged;
    
private:
    int step;
    int voice;
    State currentState = State::Off;
    bool isActive = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StepPad)
};
