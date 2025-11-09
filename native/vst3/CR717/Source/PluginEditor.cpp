/*
  Cherni CR-717 Plugin Editor
  
  Implements VST3 UI Best Practices (2025):
  - APVTS + Attachments for thread-safe parameter automation
  - Resizable with setResizeLimits (1100x720 - 1650x1080)
  - Throttled repaints at 30Hz for meters (not 60Hz)
  - Accessibility: titles and descriptions for all controls
  - No OpenGL (CPU rendering only, macOS compatible)
  - HiDPI ready via JUCE's built-in scaling
  
  See: backlog/open/40-ui-guide-complete-overhaul.md
*/

#include "PluginEditor.h"

using namespace DesignTokens;

CR717Editor::CR717Editor(CR717Processor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setLookAndFeel(&lookAndFeel);
    
    setSize(1200, 800);
    setResizable(true, true);
    setResizeLimits(1200, 800, 1800, 1200);
    
    // Preset selector (combo box for 36 presets)
    presetSelector.setColour(juce::ComboBox::backgroundColourId, Colors::bgSecondary);
    presetSelector.setColour(juce::ComboBox::textColourId, Colors::textPrimary);
    presetSelector.setColour(juce::ComboBox::outlineColourId, Colors::border);
    presetSelector.setColour(juce::ComboBox::arrowColourId, Colors::accent);
    
    const auto& presets = processor.getPresetManager().getPresets();
    for (size_t i = 0; i < presets.size(); ++i)
    {
        juce::String itemText = presets[i].style + " - " + presets[i].name;
        presetSelector.addItem(itemText, static_cast<int>(i + 1));
    }
    presetSelector.setSelectedId(1, juce::dontSendNotification);
    presetSelector.onChange = [this] {
        int index = presetSelector.getSelectedId() - 1;
        if (index >= 0)
            processor.setCurrentProgram(index);
    };
    addAndMakeVisible(presetSelector);
    
    // Preset navigation buttons
    prevPresetButton.setButtonText("<");
    nextPresetButton.setButtonText(">");
    for (auto* btn : {&prevPresetButton, &nextPresetButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        btn->setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        addAndMakeVisible(btn);
    }
    
    prevPresetButton.onClick = [this] {
        int current = presetSelector.getSelectedId() - 1;
        if (current > 0)
            presetSelector.setSelectedId(current, juce::sendNotification);
    };
    
    nextPresetButton.onClick = [this] {
        int current = presetSelector.getSelectedId() + 1;
        if (current <= presetSelector.getNumItems())
            presetSelector.setSelectedId(current, juce::sendNotification);
    };
    
    // Transport controls
    playButton.setButtonText("▶");
    stopButton.setButtonText("■");
    for (auto* btn : {&playButton, &stopButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::accent);
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        addAndMakeVisible(btn);
    }
    
    playButton.onClick = [this] { processor.startSequencer(); };
    stopButton.onClick = [this] { processor.stopSequencer(); };
    
    // BPM control
    bpmLabel.setText("BPM", juce::dontSendNotification);
    bpmLabel.setJustificationType(juce::Justification::centredRight);
    bpmLabel.setColour(juce::Label::textColourId, Colors::textSecondary);
    addAndMakeVisible(bpmLabel);
    
    bpmSlider.setRange(60, 240, 1);
    bpmSlider.setValue(120);
    bpmSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    bpmSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 50, 20);
    bpmSlider.onValueChange = [this] {
        processor.getSequencer().setBPM(bpmSlider.getValue());
    };
    addAndMakeVisible(bpmSlider);
    
    // Voice selector for sequencer
    const char* voiceNames[] = {"BD", "SD", "LT", "MT", "HT", "RS", "CP", "CH", "OH", "CY", "RD", "CB"};
    for (int i = 0; i < 12; ++i)
        voiceSelector.addItem(voiceNames[i], i + 1);
    voiceSelector.setSelectedId(1);
    voiceSelector.onChange = [this] {
        selectedVoice = voiceSelector.getSelectedId() - 1;
        updateStepButtons();
    };
    addAndMakeVisible(voiceSelector);
    
    // Step buttons
    for (int i = 0; i < 16; ++i)
    {
        stepButtons[i].setButtonText(juce::String(i + 1));
        stepButtons[i].setClickingTogglesState(true);
        stepButtons[i].setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        stepButtons[i].onClick = [this, i] {
            bool active = stepButtons[i].getToggleState();
            processor.getSequencer().setStep(selectedVoice, i, active);
        };
        addAndMakeVisible(stepButtons[i]);
    }
    
    // MIDI drag source
    addAndMakeVisible(midiDragSource);
    
    // Copy/Paste/Clear buttons
    copyButton.setButtonText("Copy");
    pasteButton.setButtonText("Paste");
    clearButton.setButtonText("Clear");
    
    for (auto* btn : {&copyButton, &pasteButton, &clearButton})
    {
        btn->setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        btn->setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        addAndMakeVisible(btn);
    }
    
    // Theme toggle
    themeButton.setButtonText("Theme");
    themeButton.setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
    themeButton.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
    addAndMakeVisible(themeButton);
    
    // Master level with meter
    masterLevelLabel.setText("Master", juce::dontSendNotification);
    masterLevelLabel.setJustificationType(juce::Justification::centred);
    masterLevelLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    masterLevelLabel.setFont(juce::Font(Typography::md));
    addAndMakeVisible(masterLevelLabel);
    
    addAndMakeVisible(masterLevelSlider);
    masterLevelAttachment = std::make_unique<SliderAttachment>(
        processor.getAPVTS(), ParamIDs::masterLevel, masterLevelSlider);
    
    addAndMakeVisible(masterMeter);
    
    // Setup all 12 voices
    setupVoiceControls(bdControls, "BD", Colors::instBD, ParamIDs::bdLevel, ParamIDs::bdTune, ParamIDs::bdDecay, ParamIDs::bdTone);
    setupVoiceControls(sdControls, "SD", Colors::instSD, ParamIDs::sdLevel, ParamIDs::sdTune, ParamIDs::sdDecay, ParamIDs::sdSnappy);
    setupVoiceControls(ltControls, "LT", Colors::instTom, ParamIDs::ltLevel, ParamIDs::ltTune, ParamIDs::ltDecay, nullptr);
    setupVoiceControls(mtControls, "MT", Colors::instTom, ParamIDs::mtLevel, ParamIDs::mtTune, ParamIDs::mtDecay, nullptr);
    setupVoiceControls(htControls, "HT", Colors::instTom, ParamIDs::htLevel, ParamIDs::htTune, ParamIDs::htDecay, nullptr);
    setupVoiceControls(rsControls, "RS", Colors::instRS, ParamIDs::rsLevel, ParamIDs::rsTune, nullptr, nullptr);
    setupVoiceControls(cpControls, "CP", Colors::instCP, ParamIDs::cpLevel, nullptr, nullptr, ParamIDs::cpTone);
    setupVoiceControls(chControls, "CH", Colors::instHH, ParamIDs::chLevel, nullptr, nullptr, ParamIDs::chTone);
    setupVoiceControls(ohControls, "OH", Colors::instHH, ParamIDs::ohLevel, nullptr, ParamIDs::ohDecay, ParamIDs::ohTone);
    setupVoiceControls(cyControls, "CY", Colors::instCY, ParamIDs::cyLevel, nullptr, ParamIDs::cyDecay, ParamIDs::cyTone);
    setupVoiceControls(rdControls, "RD", Colors::instCY, ParamIDs::rdLevel, nullptr, nullptr, ParamIDs::rdTone);
    setupVoiceControls(cbControls, "CB", Colors::instRS, ParamIDs::cbLevel, ParamIDs::cbTune, nullptr, nullptr);
    
    // Throttle UI updates to 30Hz (per guide recommendation)
    startTimerHz(30);
}

CR717Editor::~CR717Editor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void CR717Editor::setupVoiceControls(VoiceControls& vc, const juce::String& name, juce::Colour color,
                                          const char* levelID, const char* tuneID,
                                          const char* decayID, const char* toneID)
{
    vc.name = name;
    vc.color = color;
    
    vc.nameLabel.setText(name, juce::dontSendNotification);
    vc.nameLabel.setJustificationType(juce::Justification::centredLeft);
    vc.nameLabel.setColour(juce::Label::textColourId, Colors::textPrimary);
    vc.nameLabel.setFont(juce::Font(Typography::lg, juce::Font::bold));
    addAndMakeVisible(vc.nameLabel);
    
    auto setupSlider = [this, &name](CustomKnob& slider, const char* paramID, const juce::String& paramName,
                             std::unique_ptr<SliderAttachment>& attachment) {
        if (paramID == nullptr) return;
        slider.setTitle(name + " " + paramName); // Accessibility
        slider.setDescription(name + " " + paramName + " control");
        addAndMakeVisible(slider);
        attachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), paramID, slider);
    };
    
    setupSlider(vc.levelSlider, levelID, "Level", vc.levelAttachment);
    setupSlider(vc.tuneSlider, tuneID, "Tune", vc.tuneAttachment);
    setupSlider(vc.decaySlider, decayID, "Decay", vc.decayAttachment);
    setupSlider(vc.toneSlider, toneID, "Tone", vc.toneAttachment);
    
    addAndMakeVisible(vc.meter);
}

void CR717Editor::paint(juce::Graphics& g)
{
    g.fillAll(Colors::bgPrimary);
    
    auto bounds = getLocalBounds();
    
    // Header
    auto headerArea = bounds.removeFromTop(60);
    paintHeader(g, headerArea);
    
    // Pattern bank
    auto patternArea = bounds.removeFromTop(50);
    paintPatternBank(g, patternArea);
    
    // Footer
    auto footerArea = bounds.removeFromBottom(30);
    paintFooter(g, footerArea);
    
    // Voice section
    paintVoiceSection(g, bounds);
}

void CR717Editor::paintHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textPrimary);
    g.setFont(juce::Font(Typography::xl, juce::Font::bold));
    g.drawText("Cherni CR-717", area.reduced(Spacing::md, 0), juce::Justification::centredLeft);
}

void CR717Editor::paintPatternBank(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgElevated);
    g.fillRoundedRectangle(area.reduced(Spacing::sm).toFloat(), Radius::md);
}

void CR717Editor::paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area)
{
    // Draw voice panels with color stripes
    auto drawVoicePanel = [&](juce::Rectangle<int> r, juce::Colour color) {
        g.setColour(Colors::bgSecondary);
        g.fillRoundedRectangle(r.toFloat(), Radius::lg);
        
        // Color stripe on left
        g.setColour(color);
        g.fillRoundedRectangle(r.removeFromLeft(3).toFloat(), Radius::sm);
    };
    
    int voiceY = area.getY() + Spacing::md;
    int voiceHeight = 140;
    int voiceWidth = (area.getWidth() - Spacing::md * 5 - 120) / 4; // 4 columns
    
    VoiceControls* allVoices[] = {&bdControls, &sdControls, &ltControls, &mtControls,
                                   &htControls, &rsControls, &cpControls, &chControls,
                                   &ohControls, &cyControls, &rdControls, &cbControls};
    
    for (int i = 0; i < 12; ++i)
    {
        int col = i % 4;
        int row = i / 4;
        int x = area.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = voiceY + row * (voiceHeight + Spacing::md);
        
        drawVoicePanel(juce::Rectangle<int>(x, y, voiceWidth, voiceHeight), allVoices[i]->color);
    }
}

void CR717Editor::paintFooter(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textMuted);
    g.setFont(juce::Font(Typography::xs));
    
    int currentPreset = processor.getCurrentProgram();
    juce::String status = "Preset " + juce::String(currentPreset + 1) + " — " + 
                         processor.getProgramName(currentPreset);
    g.drawText(status, area.reduced(Spacing::sm, 0), juce::Justification::centredLeft);
}

void CR717Editor::resized()
{
    auto bounds = getLocalBounds();
    const int pad = ui::tokens::spacing::MD;
    
    UiGrid grid(getWidth());
    
    // Header (60px)
    auto headerArea = bounds.removeFromTop(60);
    
    // Preset browser (50px) - spans 8 columns
    auto presetArea = bounds.removeFromTop(50).reduced(pad, ui::tokens::spacing::SM);
    prevPresetButton.setBounds(presetArea.removeFromLeft(40));
    presetArea.removeFromLeft(ui::tokens::spacing::SM);
    presetSelector.setBounds(presetArea.removeFromLeft(300));
    presetArea.removeFromLeft(ui::tokens::spacing::SM);
    nextPresetButton.setBounds(presetArea.removeFromLeft(40));
    presetArea.removeFromLeft(ui::tokens::spacing::LG);
    copyButton.setBounds(presetArea.removeFromLeft(60));
    pasteButton.setBounds(presetArea.removeFromLeft(60).translated(ui::tokens::spacing::SM, 0));
    clearButton.setBounds(presetArea.removeFromLeft(60).translated(ui::tokens::spacing::SM * 2, 0));
    themeButton.setBounds(getWidth() - 80 - pad, 10, 80, 30);
    
    // Sequencer section (100px)
    auto seqArea = bounds.removeFromTop(100).reduced(pad, ui::tokens::spacing::SM);
    
    // Transport row
    auto transportRow = seqArea.removeFromTop(40);
    playButton.setBounds(transportRow.removeFromLeft(50));
    transportRow.removeFromLeft(ui::tokens::spacing::SM);
    stopButton.setBounds(transportRow.removeFromLeft(50));
    transportRow.removeFromLeft(ui::tokens::spacing::LG);
    bpmLabel.setBounds(transportRow.removeFromLeft(40));
    transportRow.removeFromLeft(ui::tokens::spacing::SM);
    bpmSlider.setBounds(transportRow.removeFromLeft(200));
    
    // MIDI drag (right side)
    midiDragSource.setBounds(getWidth() - 130 - pad, 60 + 50 + 10, 120, 35);
    
    // Step grid row
    seqArea.removeFromTop(ui::tokens::spacing::SM);
    auto stepRow = seqArea.removeFromTop(40);
    voiceSelector.setBounds(stepRow.removeFromLeft(100));
    stepRow.removeFromLeft(pad);
    
    // 16 step buttons
    int stepWidth = (stepRow.getWidth() - 15 * 4) / 16;
    for (int i = 0; i < 16; ++i)
    {
        stepButtons[i].setBounds(stepRow.removeFromLeft(stepWidth));
        if (i < 15) stepRow.removeFromLeft(4);
    }
    
    // Footer (30px)
    auto footerArea = bounds.removeFromBottom(30);
    
    // Main content area - Voice grid
    bounds.reduce(pad, pad);
    
    // Master controls (right side, 120px wide)
    auto masterArea = bounds.removeFromRight(120);
    masterArea.removeFromTop(pad);
    masterLevelLabel.setBounds(masterArea.removeFromTop(25));
    masterArea.removeFromTop(ui::tokens::spacing::SM);
    auto meterAndSlider = masterArea.removeFromTop(200);
    masterMeter.setBounds(meterAndSlider.removeFromLeft(20));
    meterAndSlider.removeFromLeft(ui::tokens::spacing::SM);
    masterLevelSlider.setBounds(meterAndSlider);
    
    bounds.removeFromRight(pad);
    
    // Voice grid (4x3)
    int voiceWidth = (bounds.getWidth() - 3 * pad) / 4;
    int voiceHeight = (bounds.getHeight() - 2 * pad) / 3;
    
    auto layoutVoice = [&](VoiceControls& vc, int col, int row) {
        int x = bounds.getX() + col * (voiceWidth + pad);
        int y = bounds.getY() + row * (voiceHeight + pad);
        
        vc.nameLabel.setBounds(x + 10, y + 10, voiceWidth - 20, 25);
        vc.meter.setBounds(x + 5, y + 45, 10, voiceHeight - 60);
        
        int knobY = y + 45;
        int knobSize = 50;
        int knobSpacing = (voiceWidth - 20 - knobSize * 4) / 3;
        int knobX = x + 20;
        
        if (vc.levelAttachment) vc.levelSlider.setBounds(knobX, knobY, knobSize, knobSize + 20);
        knobX += knobSize + knobSpacing;
        if (vc.tuneAttachment) vc.tuneSlider.setBounds(knobX, knobY, knobSize, knobSize + 20);
        knobX += knobSize + knobSpacing;
        if (vc.decayAttachment) vc.decaySlider.setBounds(knobX, knobY, knobSize, knobSize + 20);
        knobX += knobSize + knobSpacing;
        if (vc.toneAttachment) vc.toneSlider.setBounds(knobX, knobY, knobSize, knobSize + 20);
    };
    
    // Layout all 12 voices in 4x3 grid
    layoutVoice(bdControls, 0, 0);
    layoutVoice(sdControls, 1, 0);
    layoutVoice(ltControls, 2, 0);
    layoutVoice(mtControls, 3, 0);
    
    layoutVoice(htControls, 0, 1);
    layoutVoice(rsControls, 1, 1);
    layoutVoice(cpControls, 2, 1);
    layoutVoice(chControls, 3, 1);
    
    layoutVoice(ohControls, 0, 2);
    layoutVoice(cyControls, 1, 2);
    layoutVoice(rdControls, 2, 2);
    layoutVoice(cbControls, 3, 2);
    layoutVoice(bdControls, 0, 0);
    layoutVoice(sdControls, 1, 0);
    layoutVoice(ltControls, 2, 0);
    layoutVoice(mtControls, 3, 0);
    
    layoutVoice(htControls, 0, 1);
    layoutVoice(rsControls, 1, 1);
    layoutVoice(cpControls, 2, 1);
    layoutVoice(chControls, 3, 1);
    
    layoutVoice(ohControls, 0, 2);
    layoutVoice(cyControls, 1, 2);
    layoutVoice(rdControls, 2, 2);
    layoutVoice(cbControls, 3, 2);
}

void CR717Editor::timerCallback()
{
    // Update preset selection if changed externally
    int currentProgram = processor.getCurrentProgram();
    if (currentProgram + 1 != presetSelector.getSelectedId())
    {
        presetSelector.setSelectedId(currentProgram + 1, juce::dontSendNotification);
        updateStepButtons(); // Update sequencer UI when preset changes
        bpmSlider.setValue(processor.getSequencer().getBPM(), juce::dontSendNotification);
        repaint();
    }
    
    // Update step button highlighting for current step
    if (processor.getSequencer().getPlaying())
    {
        int currentStep = processor.getSequencer().getCurrentStep();
        for (int i = 0; i < 16; ++i)
        {
            bool isCurrentStep = (i == currentStep);
            bool isActive = stepButtons[i].getToggleState();
            
            if (isCurrentStep && isActive)
                stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::success);
            else if (isActive)
                stepButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        }
    }
    
    // Update MIDI drag source with current pattern
    midiDragSource.setMidiFile(processor.getSequencer().generateMidiFile());
    
    // Update meters (placeholder - would need actual level data from processor)
    masterMeter.setLevel(0.5f);
}

void CR717Editor::updateStepButtons()
{
    for (int i = 0; i < 16; ++i)
    {
        bool active = processor.getSequencer().getStep(selectedVoice, i);
        stepButtons[i].setToggleState(active, juce::dontSendNotification);
    }
}
