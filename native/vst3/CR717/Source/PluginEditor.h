#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "DesignTokens.h"
#include "LookAndFeelCR717.h"
#include "ui/layout/GridLayout.h"
#include "ui/components/HeaderPanel.h"
#include "ui/components/sequencer/PadGrid.h"
#include "ui/components/voices/VoiceStrip.h"
#include "ui/components/master/MasterPanel.h"

class CR717Editor : public juce::AudioProcessorEditor, 
                    private juce::Timer
{
public:
    explicit CR717Editor(CR717Processor&);
    ~CR717Editor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateMeters();
    void updatePlayhead();
    void loadPatternFromProcessor();
    void savePadToProcessor(int step, int voice, StepPad::State state);
    void handlePresetChange(int index);

    CR717Processor& processor;
    
    // Layout system
    std::unique_ptr<GridLayout> gridLayout;
    
    // UI Components
    std::unique_ptr<HeaderPanel> header;
    std::unique_ptr<PadGrid> sequencer;
    std::unique_ptr<VoiceStripPanel> voiceStrips;
    std::unique_ptr<MasterPanel> masterPanel;
    
    // APVTS attachments
    using SliderAttachment   = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<SliderAttachment> masterLevelAttachment;
    std::unique_ptr<SliderAttachment> reverbSizeAttachment, reverbDampAttachment, reverbWetAttachment,
                                      reverbPreDelayAttachment, reverbDiffusionAttachment;
    std::unique_ptr<SliderAttachment> delayTimeAttachment, delayFeedbackAttachment, delayWetAttachment,
                                      delayModRateAttachment, delayModDepthAttachment;
    std::unique_ptr<ComboBoxAttachment> delayModeAttachment;
    
    LookAndFeelCR717 lookAndFeel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CR717Editor)
};
