#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../../DesignTokens.h"
#include "../controls/RotaryKnob.h"
#include "../controls/VerticalFader.h"

/**
 * Professional voice strip with mixer controls and per-voice parameters
 * Width: ~84px at 1440x900, min 72px at 1100x720
 * Components: Label, Mute/Solo, Pan, Decay, Tone, Level fader
 * Elevation: L0 with accent bar and separators
 */
class VoiceStrip : public juce::Component
{
public:
    struct VoiceConfig
    {
        juce::String name;
        juce::String fullName;
        juce::Colour accentColor;
        bool hasDecay;
        bool hasTone;
        bool hasSnappy;
    };
    
    VoiceStrip(int voiceIndex, const VoiceConfig& config)
        : voice(voiceIndex)
        , cfg(config)
    {
        // Voice label
        nameLabel.setText(cfg.name, juce::dontSendNotification);
        nameLabel.setJustificationType(juce::Justification::centred);
        nameLabel.setFont(juce::Font(DesignTokens::Typography::sm, juce::Font::bold));
        nameLabel.setColour(juce::Label::textColourId, DesignTokens::Colors::textPrimary);
        nameLabel.setTooltip(cfg.fullName);
        addAndMakeVisible(nameLabel);
        
        // Mute button
        muteBtn.setButtonText("M");
        muteBtn.setTooltip("Mute " + cfg.fullName);
        muteBtn.setClickingTogglesState(true);
        muteBtn.onClick = [this] { onMuteChanged(); };
        addAndMakeVisible(muteBtn);
        
        // Solo button
        soloBtn.setButtonText("S");
        soloBtn.setTooltip("Solo " + cfg.fullName);
        soloBtn.setClickingTogglesState(true);
        soloBtn.onClick = [this] { onSoloChanged(); };
        addAndMakeVisible(soloBtn);
        
        // Pan knob
        panKnob = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Pan");
        panKnob->setRange(-1.0, 1.0, 0.01);
        panKnob->setValue(0.0);
        panKnob->setTooltip("Pan " + cfg.fullName);
        addAndMakeVisible(panKnob.get());
        
        // Decay knob (if applicable)
        if (cfg.hasDecay)
        {
            decayKnob = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Decay");
            decayKnob->setRange(0.0, 1.0, 0.01);
            decayKnob->setValue(0.5);
            decayKnob->setTooltip("Decay " + cfg.fullName);
            addAndMakeVisible(decayKnob.get());
        }
        
        // Tone/Snappy knob (if applicable)
        if (cfg.hasTone || cfg.hasSnappy)
        {
            toneKnob = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, 
                                                     cfg.hasSnappy ? "Snappy" : "Tone");
            toneKnob->setRange(0.0, 1.0, 0.01);
            toneKnob->setValue(0.5);
            toneKnob->setTooltip((cfg.hasSnappy ? "Snappy " : "Tone ") + cfg.fullName);
            addAndMakeVisible(toneKnob.get());
        }
        
        // Level fader
        levelFader = std::make_unique<VerticalFader>(
            VerticalFader::Width::Standard,
            VerticalFader::Height::Standard,
            "Level",
            false
        );
        levelFader->setRange(0.0, 1.0, 0.01);
        levelFader->setValue(0.8);
        levelFader->setTooltip("Level " + cfg.fullName);
        addAndMakeVisible(levelFader.get());
        
        // Apply theme
        applyTheme();
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds().toFloat();
        
        // Background
        g.setColour(Colors::bgSecondary);
        g.fillRoundedRectangle(bounds, Radius::sm);
        
        // Accent bar at top (2px)
        g.setColour(cfg.accentColor);
        g.fillRect(bounds.removeFromTop(2.0f));
        
        // Border
        g.setColour(Colors::border);
        g.drawRoundedRectangle(bounds.withTop(2.0f), Radius::sm, 1.0f);
        
        // Separator lines
        g.setColour(Colors::border.withAlpha(0.3f));
        
        // After header
        float y = headerHeight;
        g.drawLine(4, y, bounds.getWidth() - 4, y, 1.0f);
        
        // After buttons
        y += buttonRowHeight + Spacing::sm;
        g.drawLine(4, y, bounds.getWidth() - 4, y, 1.0f);
    }
    
    void resized() override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds();
        bounds.removeFromTop(2); // Accent bar
        bounds.reduce(Spacing::sm, Spacing::sm);
        
        // Header: Label
        auto header = bounds.removeFromTop(headerHeight - Spacing::sm);
        nameLabel.setBounds(header);
        
        bounds.removeFromTop(Spacing::xs);
        
        // Buttons row: Mute + Solo
        auto buttonRow = bounds.removeFromTop(buttonRowHeight);
        int btnWidth = (buttonRow.getWidth() - Spacing::xs) / 2;
        muteBtn.setBounds(buttonRow.removeFromLeft(btnWidth));
        buttonRow.removeFromLeft(Spacing::xs);
        soloBtn.setBounds(buttonRow);
        
        bounds.removeFromTop(Spacing::sm);
        
        // Knobs section
        auto knobSection = bounds.removeFromTop(knobSectionHeight);
        
        // Pan knob (always present)
        auto panBounds = knobSection.removeFromTop(40);
        panKnob->setBounds(panBounds.withSizeKeepingCentre(32, 40));
        
        knobSection.removeFromTop(Spacing::xs);
        
        // Decay knob
        if (decayKnob)
        {
            auto decayBounds = knobSection.removeFromTop(40);
            decayKnob->setBounds(decayBounds.withSizeKeepingCentre(32, 40));
            knobSection.removeFromTop(Spacing::xs);
        }
        
        // Tone/Snappy knob
        if (toneKnob)
        {
            auto toneBounds = knobSection.removeFromTop(40);
            toneKnob->setBounds(toneBounds.withSizeKeepingCentre(32, 40));
        }
        
        bounds.removeFromTop(Spacing::sm);
        
        // Level fader (fills remaining space)
        levelFader->setBounds(bounds.withSizeKeepingCentre(28, bounds.getHeight()));
    }
    
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())
        {
            showContextMenu();
        }
    }
    
    // APVTS attachment helpers
    void attachToParameters(juce::AudioProcessorValueTreeState& apvts,
                           const juce::String& levelParamID,
                           const juce::String& panParamID,
                           const juce::String& decayParamID = {},
                           const juce::String& toneParamID = {})
    {
        levelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, levelParamID, *levelFader
        );
        
        panAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, panParamID, *panKnob
        );
        
        if (decayKnob && decayParamID.isNotEmpty())
        {
            decayAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, decayParamID, *decayKnob
            );
        }
        
        if (toneKnob && toneParamID.isNotEmpty())
        {
            toneAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                apvts, toneParamID, *toneKnob
            );
        }
    }
    
    void setMuted(bool muted)
    {
        muteBtn.setToggleState(muted, juce::dontSendNotification);
        updateMuteSoloColors();
    }
    
    void setSoloed(bool soloed)
    {
        soloBtn.setToggleState(soloed, juce::dontSendNotification);
        updateMuteSoloColors();
    }
    
    bool isMuted() const { return muteBtn.getToggleState(); }
    bool isSoloed() const { return soloBtn.getToggleState(); }
    
    std::function<void(int voice, bool muted)> onMute;
    std::function<void(int voice, bool soloed)> onSolo;
    
private:
    void applyTheme()
    {
        using namespace DesignTokens;
        
        muteBtn.setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        muteBtn.setColour(juce::TextButton::buttonOnColourId, Colors::danger);
        muteBtn.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        muteBtn.setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
        
        soloBtn.setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        soloBtn.setColour(juce::TextButton::buttonOnColourId, Colors::warning);
        soloBtn.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        soloBtn.setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
    }
    
    void updateMuteSoloColors()
    {
        // Visual feedback for mute/solo state
        float alpha = (muteBtn.getToggleState() || soloBtn.getToggleState()) ? 0.5f : 1.0f;
        
        if (panKnob) panKnob->setAlpha(alpha);
        if (decayKnob) decayKnob->setAlpha(alpha);
        if (toneKnob) toneKnob->setAlpha(alpha);
        if (levelFader) levelFader->setAlpha(alpha);
    }
    
    void onMuteChanged()
    {
        updateMuteSoloColors();
        if (onMute)
            onMute(voice, muteBtn.getToggleState());
    }
    
    void onSoloChanged()
    {
        updateMuteSoloColors();
        if (onSolo)
            onSolo(voice, soloBtn.getToggleState());
    }
    
    void showContextMenu()
    {
        juce::PopupMenu menu;
        menu.addItem(1, "Reset to Default");
        menu.addSeparator();
        menu.addItem(2, "Copy Settings");
        menu.addItem(3, "Paste Settings", !copiedSettings.isVoid());
        
        menu.showMenuAsync(juce::PopupMenu::Options(),
            [this](int result)
            {
                if (result == 1) resetToDefault();
                else if (result == 2) copySettings();
                else if (result == 3) pasteSettings();
            }
        );
    }
    
    void resetToDefault()
    {
        levelFader->setValue(0.8);
        panKnob->setValue(0.0);
        if (decayKnob) decayKnob->setValue(0.5);
        if (toneKnob) toneKnob->setValue(0.5);
        muteBtn.setToggleState(false, juce::sendNotification);
        soloBtn.setToggleState(false, juce::sendNotification);
    }
    
    void copySettings()
    {
        copiedSettings = juce::var(new juce::DynamicObject());
        copiedSettings.getDynamicObject()->setProperty("level", levelFader->getValue());
        copiedSettings.getDynamicObject()->setProperty("pan", panKnob->getValue());
        if (decayKnob)
            copiedSettings.getDynamicObject()->setProperty("decay", decayKnob->getValue());
        if (toneKnob)
            copiedSettings.getDynamicObject()->setProperty("tone", toneKnob->getValue());
    }
    
    void pasteSettings()
    {
        if (copiedSettings.isVoid()) return;
        
        auto* obj = copiedSettings.getDynamicObject();
        if (!obj) return;
        
        if (obj->hasProperty("level"))
            levelFader->setValue(obj->getProperty("level"));
        if (obj->hasProperty("pan"))
            panKnob->setValue(obj->getProperty("pan"));
        if (decayKnob && obj->hasProperty("decay"))
            decayKnob->setValue(obj->getProperty("decay"));
        if (toneKnob && obj->hasProperty("tone"))
            toneKnob->setValue(obj->getProperty("tone"));
    }
    
    int voice;
    VoiceConfig cfg;
    
    juce::Label nameLabel;
    juce::TextButton muteBtn;
    juce::TextButton soloBtn;
    
    std::unique_ptr<RotaryKnob> panKnob;
    std::unique_ptr<RotaryKnob> decayKnob;
    std::unique_ptr<RotaryKnob> toneKnob;
    std::unique_ptr<VerticalFader> levelFader;
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> levelAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> panAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> decayAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> toneAttachment;
    
    static constexpr int headerHeight = 24;
    static constexpr int buttonRowHeight = 20;
    static constexpr int knobSectionHeight = 140;
    
    static inline juce::var copiedSettings;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceStrip)
};

/**
 * Container for all 12 voice strips
 */
class VoiceStripPanel : public juce::Component
{
public:
    VoiceStripPanel()
    {
        // Define voice configurations
        const std::array<VoiceStrip::VoiceConfig, 12> configs = {{
            {"BD", "Bass Drum", DesignTokens::Colors::instBD, true, true, false},
            {"SD", "Snare Drum", DesignTokens::Colors::instSD, true, false, true},
            {"LT", "Low Tom", DesignTokens::Colors::instTom, true, false, false},
            {"MT", "Mid Tom", DesignTokens::Colors::instTom, true, false, false},
            {"HT", "High Tom", DesignTokens::Colors::instTom, true, false, false},
            {"RS", "Rim Shot", DesignTokens::Colors::instRS, false, false, false},
            {"CP", "Clap", DesignTokens::Colors::instCP, false, true, false},
            {"CH", "Closed Hat", DesignTokens::Colors::instHH, false, true, false},
            {"OH", "Open Hat", DesignTokens::Colors::instHH, true, true, false},
            {"CY", "Cymbal", DesignTokens::Colors::instCY, true, true, false},
            {"RD", "Ride", DesignTokens::Colors::instCY, false, true, false},
            {"CB", "Cowbell", DesignTokens::Colors::instRS, false, false, false}
        }};
        
        // Create strips
        for (int i = 0; i < 12; ++i)
        {
            auto* strip = new VoiceStrip(i, configs[i]);
            strip->onMute = [this](int v, bool m) { handleMute(v, m); };
            strip->onSolo = [this](int v, bool s) { handleSolo(v, s); };
            strips.add(strip);
            addAndMakeVisible(strip);
        }
    }
    
    ~VoiceStripPanel() override
    {
        strips.clear(true);
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(DesignTokens::Colors::bgPrimary);
    }
    
    void resized() override
    {
        auto bounds = getLocalBounds();
        int stripWidth = bounds.getWidth() / 12;
        
        for (auto* strip : strips)
        {
            strip->setBounds(bounds.removeFromLeft(stripWidth).reduced(2));
        }
    }
    
    VoiceStrip* getStrip(int index)
    {
        return strips[index];
    }
    
private:
    void handleMute(int voice, bool muted)
    {
        // Mute logic
        if (onVoiceMuteChanged)
            onVoiceMuteChanged(voice, muted);
    }
    
    void handleSolo(int voice, bool soloed)
    {
        // Solo logic: if any voice is soloed, mute all others
        bool anySoloed = false;
        for (auto* strip : strips)
        {
            if (strip->isSoloed())
            {
                anySoloed = true;
                break;
            }
        }
        
        // Update visual state
        for (int i = 0; i < strips.size(); ++i)
        {
            if (anySoloed && !strips[i]->isSoloed())
            {
                // Visually dim non-soloed strips
                strips[i]->setAlpha(0.5f);
            }
            else
            {
                strips[i]->setAlpha(1.0f);
            }
        }
        
        if (onVoiceSoloChanged)
            onVoiceSoloChanged(voice, soloed);
    }
    
    juce::OwnedArray<VoiceStrip> strips;
    
public:
    std::function<void(int voice, bool muted)> onVoiceMuteChanged;
    std::function<void(int voice, bool soloed)> onVoiceSoloChanged;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(VoiceStripPanel)
};
