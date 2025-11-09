#include "PluginEditor.h"

using namespace DesignTokens;

TR808GarageEditor::TR808GarageEditor(TR808GarageProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(1100, 720);
    setResizable(true, true);
    setResizeLimits(1100, 720, 1650, 1080);
    
    // Pattern bank buttons
    const char* patternNames[] = {"A", "B", "C", "D", "E", "F", "G", "H"};
    for (int i = 0; i < 8; ++i)
    {
        patternButtons[i].setButtonText(patternNames[i]);
        patternButtons[i].setColour(juce::TextButton::buttonColourId, Colors::bgSecondary);
        patternButtons[i].setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        patternButtons[i].setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        patternButtons[i].setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        patternButtons[i].setClickingTogglesState(true);
        patternButtons[i].setRadioGroupId(1001);
        patternButtons[i].onClick = [this, i] {
            selectedPattern = i;
            processor.setCurrentProgram(i);
            for (int j = 0; j < 8; ++j)
                patternButtons[j].setToggleState(j == i, juce::dontSendNotification);
            repaint();
        };
        addAndMakeVisible(patternButtons[i]);
    }
    patternButtons[0].setToggleState(true, juce::dontSendNotification);
    
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
    
    startTimerHz(60);
}

TR808GarageEditor::~TR808GarageEditor()
{
    stopTimer();
}

void TR808GarageEditor::setupVoiceControls(VoiceControls& vc, const juce::String& name, juce::Colour color,
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
    
    auto setupSlider = [this](CustomKnob& slider, const char* paramID, 
                             std::unique_ptr<SliderAttachment>& attachment) {
        if (paramID == nullptr) return;
        addAndMakeVisible(slider);
        attachment = std::make_unique<SliderAttachment>(processor.getAPVTS(), paramID, slider);
    };
    
    setupSlider(vc.levelSlider, levelID, vc.levelAttachment);
    setupSlider(vc.tuneSlider, tuneID, vc.tuneAttachment);
    setupSlider(vc.decaySlider, decayID, vc.decayAttachment);
    setupSlider(vc.toneSlider, toneID, vc.toneAttachment);
    
    addAndMakeVisible(vc.meter);
}

void TR808GarageEditor::paint(juce::Graphics& g)
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

void TR808GarageEditor::paintHeader(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textPrimary);
    g.setFont(juce::Font(Typography::xl, juce::Font::bold));
    g.drawText("TR-808 Garage", area.reduced(Spacing::md, 0), juce::Justification::centredLeft);
}

void TR808GarageEditor::paintPatternBank(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgElevated);
    g.fillRoundedRectangle(area.reduced(Spacing::sm).toFloat(), Radius::md);
}

void TR808GarageEditor::paintVoiceSection(juce::Graphics& g, juce::Rectangle<int> area)
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

void TR808GarageEditor::paintFooter(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(Colors::bgSecondary);
    g.fillRect(area);
    
    g.setColour(Colors::textMuted);
    g.setFont(juce::Font(Typography::xs));
    
    juce::String status = "Pattern " + juce::String(char('A' + selectedPattern)) + " — " + 
                         processor.getProgramName(selectedPattern);
    g.drawText(status, area.reduced(Spacing::sm, 0), juce::Justification::centredLeft);
}

void TR808GarageEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Header
    auto headerArea = bounds.removeFromTop(60);
    
    // Pattern bank
    auto patternArea = bounds.removeFromTop(50).reduced(Spacing::md, Spacing::sm);
    
    int btnWidth = 40;
    int btnSpacing = Spacing::sm;
    
    for (int i = 0; i < 8; ++i)
    {
        patternButtons[i].setBounds(patternArea.removeFromLeft(btnWidth));
        patternArea.removeFromLeft(btnSpacing);
    }
    
    patternArea.removeFromLeft(Spacing::lg);
    copyButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(btnSpacing);
    pasteButton.setBounds(patternArea.removeFromLeft(60));
    patternArea.removeFromLeft(btnSpacing);
    clearButton.setBounds(patternArea.removeFromLeft(60));
    
    // Theme button (top right)
    themeButton.setBounds(getWidth() - 80 - Spacing::md, Spacing::md, 80, 30);
    
    // Footer
    auto footerArea = bounds.removeFromBottom(30);
    
    // Master (right side)
    int masterX = getWidth() - 100 - Spacing::md;
    masterLevelLabel.setBounds(masterX, bounds.getY() + Spacing::md, 100, 20);
    masterLevelSlider.setBounds(masterX + 20, bounds.getY() + Spacing::md + 25, 60, 150);
    masterMeter.setBounds(masterX, bounds.getY() + Spacing::md + 25, 15, 150);
    
    // Voice controls in 4x3 grid
    auto layoutVoice = [&](VoiceControls& vc, int col, int row) {
        int voiceWidth = (bounds.getWidth() - Spacing::md * 5 - 120) / 4;
        int voiceHeight = 140;
        int x = bounds.getX() + Spacing::md + col * (voiceWidth + Spacing::md);
        int y = bounds.getY() + Spacing::md + row * (voiceHeight + Spacing::md);
        
        vc.nameLabel.setBounds(x + 10, y + Spacing::sm, voiceWidth - 20, 25);
        vc.meter.setBounds(x + 5, y + 40, 8, 90);
        
        int knobY = y + 40;
        int knobSpacing = 50;
        int knobX = x + 20;
        
        if (vc.levelAttachment) vc.levelSlider.setBounds(knobX, knobY, 45, 70);
        if (vc.tuneAttachment) vc.tuneSlider.setBounds(knobX + knobSpacing, knobY, 45, 70);
        if (vc.decayAttachment) vc.decaySlider.setBounds(knobX + knobSpacing * 2, knobY, 45, 70);
        if (vc.toneAttachment) vc.toneSlider.setBounds(knobX + knobSpacing * 3, knobY, 45, 70);
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
}

void TR808GarageEditor::timerCallback()
{
    // Update pattern selection if changed externally
    int currentProgram = processor.getCurrentProgram();
    if (currentProgram != selectedPattern)
    {
        selectedPattern = currentProgram;
        repaint();
    }
    
    // Update meters (placeholder - would need actual level data from processor)
    masterMeter.setLevel(0.5f);
}
