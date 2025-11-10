#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "StepPad.h"
#include "../../../DesignTokens.h"

/**
 * Step sequencer pad grid
 * 16 visible steps (expandable to 32 with pager)
 * 12 voice rows (BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB)
 * 60fps playhead animation
 */
class PadGrid : public juce::Component, private juce::Timer
{
public:
    static constexpr int VISIBLE_STEPS = 16;
    static constexpr int TOTAL_STEPS = 32;
    static constexpr int VOICE_COUNT = 12;
    static constexpr int PAD_SIZE = 36;
    static constexpr int PAD_GAP = 4;
    
    PadGrid()
    {
        // Create pads
        for (int voice = 0; voice < VOICE_COUNT; ++voice)
        {
            for (int step = 0; step < VISIBLE_STEPS; ++step)
            {
                auto* pad = new StepPad(step + currentPage * VISIBLE_STEPS, voice);
                pad->onStateChanged = [this](int s, int v, StepPad::State state)
                {
                    if (onPadStateChanged)
                        onPadStateChanged(s, v, state);
                };
                pads.add(pad);
                addAndMakeVisible(pad);
            }
        }
        
        // Voice labels
        const juce::StringArray voiceNames = {"BD", "SD", "LT", "MT", "HT", "RS", 
                                              "CP", "CH", "OH", "CY", "RD", "CB"};
        for (int i = 0; i < VOICE_COUNT; ++i)
        {
            auto* label = new juce::Label({}, voiceNames[i]);
            label->setJustificationType(juce::Justification::centredRight);
            label->setFont(juce::Font(DesignTokens::Typography::sm, juce::Font::bold));
            label->setColour(juce::Label::textColourId, DesignTokens::Colors::textSecondary);
            voiceLabels.add(label);
            addAndMakeVisible(label);
        }
        
        // Step numbers
        for (int i = 0; i < VISIBLE_STEPS; ++i)
        {
            auto* label = new juce::Label({}, juce::String(i + 1));
            label->setJustificationType(juce::Justification::centred);
            label->setFont(juce::Font(DesignTokens::Typography::xs));
            label->setColour(juce::Label::textColourId, DesignTokens::Colors::textMuted);
            stepLabels.add(label);
            addAndMakeVisible(label);
        }
        
        // Page buttons
        prevPageBtn.setButtonText("◀");
        prevPageBtn.onClick = [this] { setPage(currentPage - 1); };
        addAndMakeVisible(prevPageBtn);
        
        nextPageBtn.setButtonText("▶");
        nextPageBtn.onClick = [this] { setPage(currentPage + 1); };
        addAndMakeVisible(nextPageBtn);
        
        pageLabel.setJustificationType(juce::Justification::centred);
        pageLabel.setFont(juce::Font(DesignTokens::Typography::sm));
        pageLabel.setColour(juce::Label::textColourId, DesignTokens::Colors::textPrimary);
        updatePageLabel();
        addAndMakeVisible(pageLabel);
        
        // Start playhead animation (60fps = ~16ms)
        startTimer(16);
    }
    
    ~PadGrid() override
    {
        stopTimer();
        pads.clear(true);
        voiceLabels.clear(true);
        stepLabels.clear(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        // Background
        g.fillAll(Colors::bgPrimary);
        
        // Draw playhead
        if (isPlaying && currentStep >= 0 && currentStep < VISIBLE_STEPS)
        {
            int x = labelWidth + currentStep * (PAD_SIZE + PAD_GAP) + PAD_GAP;
            int y = stepLabelHeight;
            int h = VOICE_COUNT * (PAD_SIZE + PAD_GAP);
            
            // Playhead bar with glow
            juce::Rectangle<float> playheadRect(x, y, 2, h);
            
            juce::DropShadow glow(Colors::accent.withAlpha(0.8f), 12, {0, 0});
            glow.drawForRectangle(g, playheadRect.toNearestInt());
            
            g.setColour(Colors::accent);
            g.fillRect(playheadRect);
        }
    }
    
    void resized() override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds();
        
        // Top: page controls
        auto topBar = bounds.removeFromTop(32);
        topBar.removeFromLeft(labelWidth);
        
        prevPageBtn.setBounds(topBar.removeFromLeft(40).reduced(Spacing::xs));
        pageLabel.setBounds(topBar.removeFromLeft(60));
        nextPageBtn.setBounds(topBar.removeFromLeft(40).reduced(Spacing::xs));
        
        // Step labels
        auto stepLabelRow = bounds.removeFromTop(stepLabelHeight);
        stepLabelRow.removeFromLeft(labelWidth);
        
        for (int i = 0; i < VISIBLE_STEPS; ++i)
        {
            stepLabels[i]->setBounds(stepLabelRow.removeFromLeft(PAD_SIZE + PAD_GAP)
                                                  .reduced(PAD_GAP / 2, 0));
        }
        
        // Grid
        for (int voice = 0; voice < VOICE_COUNT; ++voice)
        {
            auto row = bounds.removeFromTop(PAD_SIZE + PAD_GAP);
            
            // Voice label
            voiceLabels[voice]->setBounds(row.removeFromLeft(labelWidth).reduced(Spacing::sm, 0));
            
            // Pads
            for (int step = 0; step < VISIBLE_STEPS; ++step)
            {
                int padIndex = voice * VISIBLE_STEPS + step;
                pads[padIndex]->setBounds(row.removeFromLeft(PAD_SIZE + PAD_GAP)
                                             .reduced(PAD_GAP / 2));
            }
        }
    }
    
    void setPlaying(bool playing)
    {
        isPlaying = playing;
        repaint();
    }
    
    void setCurrentStep(int step)
    {
        // Clear previous step highlight
        if (currentStep >= 0 && currentStep < VISIBLE_STEPS)
        {
            for (int voice = 0; voice < VOICE_COUNT; ++voice)
            {
                int padIndex = voice * VISIBLE_STEPS + currentStep;
                pads[padIndex]->setActive(false);
            }
        }
        
        currentStep = step % VISIBLE_STEPS;
        
        // Highlight current step
        if (currentStep >= 0 && currentStep < VISIBLE_STEPS)
        {
            for (int voice = 0; voice < VOICE_COUNT; ++voice)
            {
                int padIndex = voice * VISIBLE_STEPS + currentStep;
                pads[padIndex]->setActive(true);
            }
        }
        
        repaint();
    }
    
    void setPage(int page)
    {
        currentPage = juce::jlimit(0, 1, page); // 0 = steps 1-16, 1 = steps 17-32
        updatePageLabel();
        
        // Update pad indices
        for (int voice = 0; voice < VOICE_COUNT; ++voice)
        {
            for (int step = 0; step < VISIBLE_STEPS; ++step)
            {
                int padIndex = voice * VISIBLE_STEPS + step;
                // Update pad's internal step index
                if (padIndex >= 0 && padIndex < pads.size())
                {
                    int absoluteStep = step + currentPage * VISIBLE_STEPS;
                    pads[padIndex]->setStepIndex(absoluteStep);
                    if (onRequestPadState)
                    {
                        auto st = onRequestPadState(absoluteStep, voice);
                        pads[padIndex]->setState(st);
                    }
                }
            }
        }
        
        repaint();
    }
    
    void setPadState(int step, int voice, StepPad::State state)
    {
        int visibleStep = step % VISIBLE_STEPS;
        int padIndex = voice * VISIBLE_STEPS + visibleStep;
        
        if (padIndex >= 0 && padIndex < pads.size())
        {
            pads[padIndex]->setState(state);
        }
    }
    
    StepPad::State getPadState(int step, int voice) const
    {
        int visibleStep = step % VISIBLE_STEPS;
        int padIndex = voice * VISIBLE_STEPS + visibleStep;
        
        if (padIndex >= 0 && padIndex < pads.size())
            return pads[padIndex]->getState();
        
        return StepPad::State::Off;
    }
    
    std::function<void(int step, int voice, StepPad::State state)> onPadStateChanged;
    std::function<StepPad::State(int step, int voice)> onRequestPadState;
    
private:
    void timerCallback() override
    {
        // Smooth playhead animation at 60fps
        if (isPlaying)
        {
            repaint(); // Only repaint playhead area in production
        }
    }
    
    void updatePageLabel()
    {
        pageLabel.setText(juce::String(currentPage + 1) + "/2", juce::dontSendNotification);
        prevPageBtn.setEnabled(currentPage > 0);
        nextPageBtn.setEnabled(currentPage < 1);
    }
    
    juce::OwnedArray<StepPad> pads;
    juce::OwnedArray<juce::Label> voiceLabels;
    juce::OwnedArray<juce::Label> stepLabels;
    
    juce::TextButton prevPageBtn;
    juce::TextButton nextPageBtn;
    juce::Label pageLabel;
    
    int currentStep = -1;
    int currentPage = 0;
    bool isPlaying = false;
    
    static constexpr int labelWidth = 40;
    static constexpr int stepLabelHeight = 20;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PadGrid)
};
