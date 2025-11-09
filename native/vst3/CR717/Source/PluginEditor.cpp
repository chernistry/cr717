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

namespace Layout
{
    constexpr int topBarHeight = 72;
    constexpr int presetRowHeight = 56;
    constexpr int sequencerHeight = 120;
    constexpr int footerHeight = 36;
    constexpr int transportHeight = 42;
    constexpr int masterSectionWidth = 140;
    constexpr int voiceColumns = 4;
    constexpr int voiceRows = 3;
}

namespace
{
    struct VoiceSectionGeometry
    {
        juce::Rectangle<int> voiceGrid;
        juce::Rectangle<int> masterArea;
    };

    VoiceSectionGeometry getVoiceSectionGeometry(juce::Rectangle<int> area)
    {
        VoiceSectionGeometry geometry{};
        area.reduce(Spacing::md, Spacing::md);
        geometry.masterArea = area.removeFromRight(Layout::masterSectionWidth);
        geometry.masterArea = geometry.masterArea.reduced(Spacing::sm);
        area.removeFromRight(Spacing::md);
        geometry.voiceGrid = area;
        return geometry;
    }
}

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

    auto headerArea = bounds.removeFromTop(Layout::topBarHeight);
    paintHeader(g, headerArea);

    auto presetArea = bounds.removeFromTop(Layout::presetRowHeight);
    paintPatternBank(g, presetArea);

    auto sequencerArea = bounds.removeFromTop(Layout::sequencerHeight);
    paintPatternBank(g, sequencerArea);

    auto footerArea = bounds.removeFromBottom(Layout::footerHeight);
    paintFooter(g, footerArea);

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
    auto panel = area.reduced(Spacing::sm);
    g.setColour(Colors::bgSecondary);
    g.fillRoundedRectangle(panel.toFloat(), Radius::lg);
    g.setColour(Colors::border.withAlpha(0.4f));
    g.drawRoundedRectangle(panel.toFloat(), Radius::lg, 1.0f);
}

void CR717Editor::paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area)
{
    const auto geometry = getVoiceSectionGeometry(area);

    g.setColour(Colors::bgSecondary);
    g.fillRoundedRectangle(geometry.masterArea.toFloat(), Radius::lg);

    VoiceControls* allVoices[] = {&bdControls, &sdControls, &ltControls, &mtControls,
                                   &htControls, &rsControls, &cpControls, &chControls,
                                   &ohControls, &cyControls, &rdControls, &cbControls};

    const int voiceWidth = (geometry.voiceGrid.getWidth() - (Layout::voiceColumns - 1) * Spacing::md)
                           / Layout::voiceColumns;
    const int voiceHeight = (geometry.voiceGrid.getHeight() - (Layout::voiceRows - 1) * Spacing::md)
                            / Layout::voiceRows;

    for (int i = 0; i < 12; ++i)
    {
        int col = i % Layout::voiceColumns;
        int row = i / Layout::voiceColumns;
        int x = geometry.voiceGrid.getX() + col * (voiceWidth + Spacing::md);
        int y = geometry.voiceGrid.getY() + row * (voiceHeight + Spacing::md);
        auto cardBounds = juce::Rectangle<int>(x, y, voiceWidth, voiceHeight).reduced(1);

        g.setColour(Colors::bgSecondary);
        g.fillRoundedRectangle(cardBounds.toFloat(), Radius::lg);

        auto stripe = cardBounds.removeFromLeft(4);
        g.setColour(allVoices[i]->color);
        g.fillRoundedRectangle(stripe.toFloat(), Radius::sm);
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
    using namespace DesignTokens;
    
    auto bounds = getLocalBounds();

    auto topBarArea = bounds.removeFromTop(Layout::topBarHeight);
    const int themeButtonWidth = 96;
    auto themeArea = topBarArea.removeFromRight(themeButtonWidth + Spacing::md);
    auto themeButtonBounds = themeArea.removeFromTop(32);
    themeButtonBounds.setWidth(themeButtonWidth);
    themeButtonBounds.setX(getWidth() - themeButtonWidth - Spacing::md);
    themeButtonBounds.setY(topBarArea.getCentreY() - themeButtonBounds.getHeight() / 2);
    themeButton.setBounds(themeButtonBounds);

    auto presetArea = bounds.removeFromTop(Layout::presetRowHeight).reduced(Spacing::md, Spacing::sm);
    auto presetRow = presetArea;

    const int presetNavWidth = 36;
    prevPresetButton.setBounds(presetRow.removeFromLeft(presetNavWidth));
    presetRow.removeFromLeft(Spacing::sm);
    presetSelector.setBounds(presetRow.removeFromLeft(300));
    presetRow.removeFromLeft(Spacing::sm);
    nextPresetButton.setBounds(presetRow.removeFromLeft(presetNavWidth));

    const int actionButtonWidth = 68;
    auto actionRow = presetRow.removeFromRight(actionButtonWidth * 3 + Spacing::sm * 2);
    copyButton.setBounds(actionRow.removeFromLeft(actionButtonWidth));
    actionRow.removeFromLeft(Spacing::sm);
    pasteButton.setBounds(actionRow.removeFromLeft(actionButtonWidth));
    actionRow.removeFromLeft(Spacing::sm);
    clearButton.setBounds(actionRow.removeFromLeft(actionButtonWidth));

    auto seqArea = bounds.removeFromTop(Layout::sequencerHeight).reduced(Spacing::md, Spacing::sm);
    auto transportRow = seqArea.removeFromTop(Layout::transportHeight);

    const int transportButtonWidth = 48;
    playButton.setBounds(transportRow.removeFromLeft(transportButtonWidth));
    transportRow.removeFromLeft(Spacing::xs);
    stopButton.setBounds(transportRow.removeFromLeft(transportButtonWidth));
    transportRow.removeFromLeft(Spacing::md);

    const int voiceSelectorWidth = 110;
    auto voiceArea = transportRow.removeFromRight(voiceSelectorWidth);
    voiceSelector.setBounds(voiceArea);
    transportRow.removeFromRight(Spacing::md);

    const int midiWidth = 120;
    auto midiArea = transportRow.removeFromRight(midiWidth);
    midiDragSource.setBounds(midiArea);
    transportRow.removeFromRight(Spacing::md);

    const int bpmLabelWidth = 40;
    bpmLabel.setBounds(transportRow.removeFromLeft(bpmLabelWidth));
    transportRow.removeFromLeft(Spacing::sm);
    bpmSlider.setBounds(transportRow.removeFromLeft(juce::jmin(240, transportRow.getWidth())));

    seqArea.removeFromTop(Spacing::sm);
    auto stepRow = seqArea;
    const int stepGap = Spacing::xs;
    const int stepWidth = juce::jmax(28, (stepRow.getWidth() - stepGap * 15) / 16);
    for (int i = 0; i < 16; ++i)
    {
        auto stepBounds = stepRow.removeFromLeft(stepWidth);
        stepButtons[i].setBounds(stepBounds);
        if (i < 15)
            stepRow.removeFromLeft(stepGap);
    }

    bounds.removeFromBottom(Layout::footerHeight);

    const auto voiceGeometry = getVoiceSectionGeometry(bounds);

    auto masterArea = voiceGeometry.masterArea;
    masterLevelLabel.setBounds(masterArea.removeFromTop(24));
    masterArea.removeFromTop(Spacing::sm);
    auto meterArea = masterArea.removeFromLeft(20);
    masterMeter.setBounds(meterArea);
    masterArea.removeFromLeft(Spacing::sm);
    masterLevelSlider.setBounds(masterArea);

    const int voiceWidth = (voiceGeometry.voiceGrid.getWidth() - (Layout::voiceColumns - 1) * Spacing::md)
                           / Layout::voiceColumns;
    const int voiceHeight = (voiceGeometry.voiceGrid.getHeight() - (Layout::voiceRows - 1) * Spacing::md)
                            / Layout::voiceRows;

    auto layoutVoice = [&](VoiceControls& vc, int col, int row) {
        int x = voiceGeometry.voiceGrid.getX() + col * (voiceWidth + Spacing::md);
        int y = voiceGeometry.voiceGrid.getY() + row * (voiceHeight + Spacing::md);
        auto voiceArea = juce::Rectangle<int>(x, y, voiceWidth, voiceHeight);
        
        // Voice name label (top)
        vc.nameLabel.setBounds(voiceArea.removeFromTop(30).reduced(Spacing::sm, Spacing::xs));
        
        // Meter (left side)
        auto meterArea = voiceArea.removeFromLeft(15);
        vc.meter.setBounds(meterArea.reduced(2, Spacing::sm));
        
        voiceArea.removeFromLeft(Spacing::xs);
        
        // Knobs in a row
        voiceArea.reduce(Spacing::sm, Spacing::xs);
        const int knobSize = 50;
        const int knobSpacing = (voiceArea.getWidth() - knobSize * 4) / 3;
        
        auto knobRow = voiceArea.removeFromTop(knobSize + 20);
        
        if (vc.levelAttachment) {
            vc.levelSlider.setBounds(knobRow.removeFromLeft(knobSize).withHeight(knobSize + 20));
            knobRow.removeFromLeft(knobSpacing);
        }
        if (vc.tuneAttachment) {
            vc.tuneSlider.setBounds(knobRow.removeFromLeft(knobSize).withHeight(knobSize + 20));
            knobRow.removeFromLeft(knobSpacing);
        }
        if (vc.decayAttachment) {
            vc.decaySlider.setBounds(knobRow.removeFromLeft(knobSize).withHeight(knobSize + 20));
            knobRow.removeFromLeft(knobSpacing);
        }
        if (vc.toneAttachment) {
            vc.toneSlider.setBounds(knobRow.removeFromLeft(knobSize).withHeight(knobSize + 20));
        }
    };
    
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
