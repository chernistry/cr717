#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../DesignTokens.h"

/**
 * Header panel with transport controls, BPM, and preset selector
 * Elevation: L1 (subtle shadow)
 */
class HeaderPanel : public juce::Component
{
public:
    HeaderPanel()
    {
        // Play/Stop button
        playButton.setButtonText("▶");
        playButton.setTooltip("Play/Stop (Space)");
        playButton.onClick = [this] { onPlayStop(); };
        addAndMakeVisible(playButton);
        
        // BPM label and slider
        bpmLabel.setText("BPM", juce::dontSendNotification);
        bpmLabel.setJustificationType(juce::Justification::centredRight);
        addAndMakeVisible(bpmLabel);
        
        bpmSlider.setRange(60.0, 240.0, 1.0);
        bpmSlider.setValue(120.0);
        bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 24);
        bpmSlider.setTooltip("Tempo (60-240 BPM)");
        bpmSlider.onValueChange = [this]
        {
            if (onBPMChange)
                onBPMChange(bpmSlider.getValue());
        };
        addAndMakeVisible(bpmSlider);
        
        // Preset selector
        presetCombo.addItem("Init", 1);
        presetCombo.addItem("UK Garage", 2);
        presetCombo.addItem("DnB", 3);
        presetCombo.addItem("Techno", 4);
        presetCombo.setSelectedId(1);
        presetCombo.setTooltip("Select preset");
        presetCombo.onChange = [this]
        {
            if (onPresetChange)
                onPresetChange(presetCombo.getSelectedId() - 1);
        };
        addAndMakeVisible(presetCombo);
        
        // Pattern selector
        patternLabel.setText("Pattern", juce::dontSendNotification);
        addAndMakeVisible(patternLabel);
        
        for (int i = 0; i < 8; ++i)
        {
            auto* btn = new juce::TextButton(juce::String(char('A' + i)));
            btn->setTooltip("Pattern " + juce::String(char('A' + i)));
            btn->setClickingTogglesState(true);
            btn->setRadioGroupId(1);
            btn->onClick = [this, i]
            {
                if (onPatternChange)
                    onPatternChange(i);
            };
            patternButtons.add(btn);
            addAndMakeVisible(btn);
        }
        patternButtons[0]->setToggleState(true, juce::dontSendNotification);
        
        // Apply theme
        applyTheme();
    }
    
    ~HeaderPanel() override
    {
        patternButtons.clear(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        // Background with subtle gradient
        auto bounds = getLocalBounds().toFloat();
        juce::ColourGradient gradient(
            Colors::bgSecondary, 0, 0,
            Colors::bgPrimary, 0, bounds.getHeight(),
            false
        );
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, Radius::md);
        
        // Bottom border
        g.setColour(Colors::border);
        g.drawLine(0, bounds.getHeight(), bounds.getWidth(), bounds.getHeight(), 1.0f);
        
        // Subtle shadow (L1 elevation)
        juce::DropShadow shadow(Colors::bgPrimary.withAlpha(0.5f), 4, {0, 2});
        shadow.drawForRectangle(g, getLocalBounds());
    }
    
    void resized() override
    {
        using namespace DesignTokens;
        auto bounds = getLocalBounds().reduced(Spacing::md);
        
        // Left section: Play button
        auto leftSection = bounds.removeFromLeft(80);
        playButton.setBounds(leftSection.removeFromLeft(64).reduced(Spacing::xs));
        
        bounds.removeFromLeft(Spacing::lg);
        
        // BPM section
        auto bpmSection = bounds.removeFromLeft(200);
        bpmLabel.setBounds(bpmSection.removeFromLeft(50));
        bpmSection.removeFromLeft(Spacing::sm);
        bpmSlider.setBounds(bpmSection);
        
        bounds.removeFromLeft(Spacing::xl);
        
        // Pattern section
        auto patternSection = bounds.removeFromLeft(300);
        patternLabel.setBounds(patternSection.removeFromLeft(60));
        patternSection.removeFromLeft(Spacing::sm);
        
        int btnWidth = (patternSection.getWidth() - 7 * Spacing::xs) / 8;
        for (auto* btn : patternButtons)
        {
            btn->setBounds(patternSection.removeFromLeft(btnWidth));
            patternSection.removeFromLeft(Spacing::xs);
        }
        
        // Right section: Preset selector
        presetCombo.setBounds(bounds.removeFromRight(200));
    }
    
    std::function<void()> onPlayStop;
    std::function<void(double)> onBPMChange;
    std::function<void(int)> onPresetChange;
    std::function<void(int)> onPatternChange;
    
    // Populate preset combo with display names
    void setPresetList(const juce::StringArray& names)
    {
        presetCombo.clear(juce::dontSendNotification);
        for (int i = 0; i < names.size(); ++i)
            presetCombo.addItem(names[i], i + 1);
        if (names.size() > 0)
            presetCombo.setSelectedId(1, juce::dontSendNotification);
    }
    
    void setSelectedPresetIndex(int index)
    {
        presetCombo.setSelectedId(index + 1, juce::dontSendNotification);
    }
    
    void setBPM(double bpm)
    {
        bpmSlider.setValue(bpm, juce::dontSendNotification);
    }

private:
    void applyTheme()
    {
        using namespace DesignTokens;
        
        playButton.setColour(juce::TextButton::buttonColourId, Colors::accent);
        playButton.setColour(juce::TextButton::buttonOnColourId, Colors::accentActive);
        playButton.setColour(juce::TextButton::textColourOffId, Colors::textPrimary);
        
        bpmLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
        
        presetCombo.setColour(juce::ComboBox::backgroundColourId, Colors::bgTertiary);
        presetCombo.setColour(juce::ComboBox::textColourId, Colors::textPrimary);
        presetCombo.setColour(juce::ComboBox::outlineColourId, Colors::border);
        
        patternLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
        
        for (auto* btn : patternButtons)
        {
            btn->setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
            btn->setColour(juce::TextButton::buttonOnColourId, Colors::accent);
            btn->setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
            btn->setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        }
    }
    
    juce::TextButton playButton;
    juce::Label bpmLabel;
    juce::Slider bpmSlider;
    juce::ComboBox presetCombo;
    juce::Label patternLabel;
    juce::OwnedArray<juce::TextButton> patternButtons;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HeaderPanel)
};
