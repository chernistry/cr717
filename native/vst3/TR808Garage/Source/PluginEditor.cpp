#include "PluginEditor.h"

TR808GarageEditor::TR808GarageEditor(TR808GarageProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(800, 600);
    
    // Master level
    masterLevelLabel.setText("Master", juce::dontSendNotification);
    masterLevelLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(masterLevelLabel);
    
    masterLevelSlider.setSliderStyle(juce::Slider::LinearVertical);
    masterLevelSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    addAndMakeVisible(masterLevelSlider);
    masterLevelAttachment = std::make_unique<SliderAttachment>(
        processor.getAPVTS(), ParamIDs::masterLevel, masterLevelSlider);
    
    // Voice controls
    setupVoiceControls(bdControls, "BD", ParamIDs::bdLevel, ParamIDs::bdTune, 
                      ParamIDs::bdDecay, ParamIDs::bdTone);
    setupVoiceControls(sdControls, "SD", ParamIDs::sdLevel, ParamIDs::sdTune, 
                      ParamIDs::sdDecay, ParamIDs::sdSnappy);
    setupVoiceControls(chControls, "CH", ParamIDs::chLevel, nullptr, 
                      nullptr, ParamIDs::chTone);
    setupVoiceControls(ohControls, "OH", ParamIDs::ohLevel, nullptr, 
                      ParamIDs::ohDecay, ParamIDs::ohTone);
    setupVoiceControls(cpControls, "CP", ParamIDs::cpLevel, nullptr, 
                      nullptr, ParamIDs::cpTone);
    setupVoiceControls(rsControls, "RS", ParamIDs::rsLevel, ParamIDs::rsTune, 
                      nullptr, nullptr);
    
    startTimerHz(30);
}

TR808GarageEditor::~TR808GarageEditor()
{
    stopTimer();
}

void TR808GarageEditor::setupVoiceControls(VoiceControls& vc, const juce::String& name,
                                          const char* levelID, const char* tuneID,
                                          const char* decayID, const char* toneID)
{
    vc.nameLabel.setText(name, juce::dontSendNotification);
    vc.nameLabel.setJustificationType(juce::Justification::centred);
    vc.nameLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(vc.nameLabel);
    
    auto setupSlider = [this](juce::Slider& slider, const char* paramID, 
                             std::unique_ptr<SliderAttachment>& attachment) {
        if (paramID == nullptr) return;
        
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 16);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xff00a8ff));
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        addAndMakeVisible(slider);
        attachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), paramID, slider);
    };
    
    setupSlider(vc.levelSlider, levelID, vc.levelAttachment);
    setupSlider(vc.tuneSlider, tuneID, vc.tuneAttachment);
    setupSlider(vc.decaySlider, decayID, vc.decayAttachment);
    setupSlider(vc.toneSlider, toneID, vc.toneAttachment);
}

void TR808GarageEditor::paint(juce::Graphics& g)
{
    // Dark theme background
    g.fillAll(juce::Colour(0xff1a1a1a));
    
    // Header
    g.setColour(juce::Colour(0xff2a2a2a));
    g.fillRect(0, 0, getWidth(), 60);
    
    g.setColour(juce::Colours::white);
    g.setFont(24.0f);
    g.drawText("TR-808 Garage", 20, 15, 300, 30, juce::Justification::left);
    
    // Voice sections
    g.setColour(juce::Colour(0xff2a2a2a));
    int voiceY = 80;
    int voiceHeight = 180;
    for (int i = 0; i < 6; ++i)
    {
        int x = 20 + (i % 3) * 250;
        int y = voiceY + (i / 3) * (voiceHeight + 20);
        g.fillRoundedRectangle(x, y, 230, voiceHeight, 8.0f);
    }
}

void TR808GarageEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Master section (top right)
    masterLevelLabel.setBounds(getWidth() - 100, 70, 80, 20);
    masterLevelSlider.setBounds(getWidth() - 100, 95, 80, 120);
    
    // Voice controls in 3x2 grid
    auto layoutVoice = [](VoiceControls& vc, int x, int y) {
        vc.nameLabel.setBounds(x, y, 230, 25);
        
        int knobY = y + 30;
        int knobSpacing = 55;
        int knobX = x + 10;
        
        if (vc.levelAttachment) vc.levelSlider.setBounds(knobX, knobY, 50, 70);
        if (vc.tuneAttachment) vc.tuneSlider.setBounds(knobX + knobSpacing, knobY, 50, 70);
        if (vc.decayAttachment) vc.decaySlider.setBounds(knobX + knobSpacing * 2, knobY, 50, 70);
        if (vc.toneAttachment) vc.toneSlider.setBounds(knobX + knobSpacing * 3, knobY, 50, 70);
    };
    
    int voiceY = 80;
    int voiceHeight = 180;
    
    layoutVoice(bdControls, 20, voiceY);
    layoutVoice(sdControls, 270, voiceY);
    layoutVoice(chControls, 520, voiceY);
    
    layoutVoice(ohControls, 20, voiceY + voiceHeight + 20);
    layoutVoice(cpControls, 270, voiceY + voiceHeight + 20);
    layoutVoice(rsControls, 520, voiceY + voiceHeight + 20);
}

void TR808GarageEditor::timerCallback()
{
    // Future: update meters, voice activity indicators
}
