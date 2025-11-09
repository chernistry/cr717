#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class TR808GarageEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit TR808GarageEditor(TR808GarageProcessor&);
    ~TR808GarageEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;

    TR808GarageProcessor& processor;

    // Parameter attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    
    // Master
    juce::Slider masterLevelSlider;
    std::unique_ptr<SliderAttachment> masterLevelAttachment;
    juce::Label masterLevelLabel;
    
    // Voice controls (6 voices x 4 params max = 24 sliders)
    struct VoiceControls {
        juce::Label nameLabel;
        juce::Slider levelSlider, tuneSlider, decaySlider, toneSlider;
        std::unique_ptr<SliderAttachment> levelAttachment, tuneAttachment, decayAttachment, toneAttachment;
    };
    
    VoiceControls bdControls, sdControls, chControls, ohControls, cpControls, rsControls;

    void setupVoiceControls(VoiceControls& vc, const juce::String& name,
                           const char* levelID, const char* tuneID, 
                           const char* decayID, const char* toneID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TR808GarageEditor)
};
