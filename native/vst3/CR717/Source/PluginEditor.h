#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "DesignTokens.h"
#include "LookAndFeelCR717.h"
#include "UiGrid.h"
#include "CustomKnob.h"
#include "CustomFader.h"
#include "LevelMeter.h"
#include "MidiDragSource.h"

class CR717Editor : public juce::AudioProcessorEditor, 
                          private juce::Timer,
                          public juce::DragAndDropContainer
{
public:
    explicit CR717Editor(CR717Processor&);
    ~CR717Editor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void paintHeader(juce::Graphics& g, juce::Rectangle<int> area);
    void paintPatternBank(juce::Graphics& g, juce::Rectangle<int> area);
    void paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area);
    void paintFooter(juce::Graphics& g, juce::Rectangle<int> area);

    CR717Processor& processor;

    // Preset browser
    juce::ComboBox presetSelector;
    juce::TextButton prevPresetButton, nextPresetButton;
    juce::TextButton copyButton, pasteButton, clearButton;
    
    // Sequencer transport
    juce::TextButton playButton, stopButton;
    juce::Label bpmLabel;
    juce::Slider bpmSlider;
    
    // Sequencer grid
    juce::ComboBox voiceSelector;
    juce::TextButton stepButtons[16];
    MidiDragSource midiDragSource;
    int selectedVoice = 0;
    
    // Theme toggle
    juce::TextButton themeButton;
    
    // Parameter attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    // Master with meter
    CustomFader masterLevelSlider;
    std::unique_ptr<SliderAttachment> masterLevelAttachment;
    juce::Label masterLevelLabel;
    LevelMeter masterMeter;
    
    // Voice controls with custom knobs
    struct VoiceControls {
        juce::String name;
        juce::Colour color;
        juce::Label nameLabel;
        CustomKnob levelSlider, tuneSlider, decaySlider, toneSlider;
        std::unique_ptr<SliderAttachment> levelAttachment, tuneAttachment, decayAttachment, toneAttachment;
        LevelMeter meter;
    };
    
    VoiceControls bdControls, sdControls, ltControls, mtControls, htControls, rsControls,
                  cpControls, chControls, ohControls, cyControls, rdControls, cbControls;

    void setupVoiceControls(VoiceControls& vc, const juce::String& name, juce::Colour color,
                           const char* levelID, const char* tuneID, 
                           const char* decayID, const char* toneID);
    
    void updateStepButtons();
    
    LookAndFeelCR717 lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CR717Editor)
};
