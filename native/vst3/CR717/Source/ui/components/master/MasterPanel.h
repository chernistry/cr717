#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "../../../DesignTokens.h"
#include "../controls/RotaryKnob.h"

/**
 * Stereo peak/RMS meter with hold
 * Updates at 30-60Hz, smooth animation
 */
class StereoMeter : public juce::Component, private juce::Timer
{
public:
    StereoMeter()
    {
        startTimer(33); // ~30Hz
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds().toFloat();
        float channelWidth = (bounds.getWidth() - 4) / 2.0f;
        
        // Background
        g.setColour(Colors::bgPrimary);
        g.fillRoundedRectangle(bounds, Radius::sm);
        
        // Draw meters for L and R
        for (int ch = 0; ch < 2; ++ch)
        {
            float x = ch * (channelWidth + 4);
            juce::Rectangle<float> meterBounds(x, 0, channelWidth, bounds.getHeight());
            
            drawMeter(g, meterBounds, peakLevels[ch], rmsLevels[ch], peakHolds[ch]);
        }
        
        // Border
        g.setColour(Colors::border);
        g.drawRoundedRectangle(bounds, Radius::sm, 1.0f);
        
        // Labels
        g.setColour(Colors::textMuted);
        g.setFont(juce::Font(DesignTokens::Typography::xs));
        g.drawText("L", 0, 2, channelWidth, 12, juce::Justification::centred);
        g.drawText("R", channelWidth + 4, 2, channelWidth, 12, juce::Justification::centred);
    }
    
    void setPeakLevel(int channel, float level)
    {
        if (channel >= 0 && channel < 2)
        {
            peakLevels[channel] = juce::jlimit(0.0f, 1.0f, level);
            
            // Update peak hold
            if (level > peakHolds[channel])
            {
                peakHolds[channel] = level;
                peakHoldTimes[channel] = 0.0f;
            }
        }
    }
    
    void setRMSLevel(int channel, float level)
    {
        if (channel >= 0 && channel < 2)
            rmsLevels[channel] = juce::jlimit(0.0f, 1.0f, level);
    }
    
private:
    void timerCallback() override
    {
        // Decay peak holds
        for (int ch = 0; ch < 2; ++ch)
        {
            peakHoldTimes[ch] += 0.033f; // ~33ms
            
            if (peakHoldTimes[ch] > 1.0f) // Hold for 1 second
            {
                peakHolds[ch] *= 0.95f; // Decay
            }
        }
        
        repaint();
    }
    
    void drawMeter(juce::Graphics& g, const juce::Rectangle<float>& bounds,
                   float peak, float rms, float hold)
    {
        using namespace DesignTokens;
        
        auto meterArea = bounds.reduced(2.0f, 16.0f);
        
        // RMS bar (darker)
        float rmsHeight = rms * meterArea.getHeight();
        auto rmsRect = meterArea.withTop(meterArea.getBottom() - rmsHeight);
        g.setColour(Colors::accent.withAlpha(0.3f));
        g.fillRect(rmsRect);
        
        // Peak bar
        float peakHeight = peak * meterArea.getHeight();
        auto peakRect = meterArea.withTop(meterArea.getBottom() - peakHeight);
        
        // Gradient: green -> yellow -> red
        juce::ColourGradient gradient(
            Colors::success, peakRect.getCentreX(), peakRect.getBottom(),
            Colors::danger, peakRect.getCentreX(), peakRect.getY(),
            false
        );
        gradient.addColour(0.7, Colors::warning);
        g.setGradientFill(gradient);
        g.fillRect(peakRect);
        
        // Peak hold cap
        if (hold > 0.01f)
        {
            float holdY = meterArea.getBottom() - hold * meterArea.getHeight();
            g.setColour(Colors::textPrimary);
            g.fillRect((float)meterArea.getX(), holdY - 1.0f, (float)meterArea.getWidth(), 2.0f);
        }
        
        // dB markers
        g.setColour(Colors::textMuted);
        g.setFont(juce::Font(8.0f));
        
        const float dbMarks[] = {0.0f, -6.0f, -12.0f, -18.0f, -24.0f};
        for (float db : dbMarks)
        {
            float level = juce::Decibels::decibelsToGain(db);
            float y = meterArea.getBottom() - level * meterArea.getHeight();
            g.drawLine(meterArea.getRight() + 2, y, meterArea.getRight() + 4, y, 1.0f);
        }
    }
    
    float peakLevels[2] = {0.0f, 0.0f};
    float rmsLevels[2] = {0.0f, 0.0f};
    float peakHolds[2] = {0.0f, 0.0f};
    float peakHoldTimes[2] = {0.0f, 0.0f};
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StereoMeter)
};

/**
 * Master & FX panel (right rail, 3 columns)
 * Elevation: L2 with shadow
 */
class MasterPanel : public juce::Component
{
public:
    MasterPanel()
    {
        // Section label
        titleLabel.setText("MASTER", juce::dontSendNotification);
        titleLabel.setJustificationType(juce::Justification::centred);
        titleLabel.setFont(juce::Font(DesignTokens::Typography::md, juce::Font::bold));
        titleLabel.setColour(juce::Label::textColourId, DesignTokens::Colors::textPrimary);
        addAndMakeVisible(titleLabel);
        
        // Meters
        addAndMakeVisible(meters);
        
        // Clip indicator
        clipIndicator.setButtonText("CLIP");
        clipIndicator.setEnabled(false);
        clipIndicator.setColour(juce::TextButton::buttonColourId, DesignTokens::Colors::danger.withAlpha(0.3f));
        clipIndicator.setColour(juce::TextButton::textColourOffId, DesignTokens::Colors::danger);
        addAndMakeVisible(clipIndicator);
        
        // Output gain
        outputGain = std::make_unique<RotaryKnob>(RotaryKnob::Size::Medium, "Output");
        outputGain->setRange(-12.0, 12.0, 0.1);
        outputGain->setValue(0.0);
        outputGain->setTooltip("Master Output Gain");
        addAndMakeVisible(outputGain.get());
        
        // FX Section
        fxLabel.setText("FX RETURNS", juce::dontSendNotification);
        fxLabel.setJustificationType(juce::Justification::centred);
        fxLabel.setFont(juce::Font(DesignTokens::Typography::sm, juce::Font::bold));
        fxLabel.setColour(juce::Label::textColourId, DesignTokens::Colors::textSecondary);
        addAndMakeVisible(fxLabel);
        
        // Reverb controls
        reverbSize = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Size");
        reverbSize->setRange(0.0, 1.0, 0.01);
        reverbSize->setValue(0.5);
        addAndMakeVisible(reverbSize.get());
        
        reverbDamp = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Damp");
        reverbDamp->setRange(0.0, 1.0, 0.01);
        reverbDamp->setValue(0.5);
        addAndMakeVisible(reverbDamp.get());
        
        reverbMix = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Mix");
        reverbMix->setRange(0.0, 1.0, 0.01);
        reverbMix->setValue(0.3);
        addAndMakeVisible(reverbMix.get());
        
        reverbPreDelay = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "PreDly");
        reverbPreDelay->setRange(0.0, 100.0, 1.0);
        reverbPreDelay->setValue(20.0);
        addAndMakeVisible(reverbPreDelay.get());
        
        reverbDiffusion = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Diff");
        reverbDiffusion->setRange(0.0, 1.0, 0.01);
        reverbDiffusion->setValue(0.7);
        addAndMakeVisible(reverbDiffusion.get());
        
        // Delay controls
        delayTime = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Time");
        delayTime->setRange(0.125, 1.0, 0.125);
        delayTime->setValue(0.25);
        addAndMakeVisible(delayTime.get());
        
        delayFeedback = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Fdbk");
        delayFeedback->setRange(0.0, 0.9, 0.01);
        delayFeedback->setValue(0.4);
        addAndMakeVisible(delayFeedback.get());
        
        delayMix = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "Mix");
        delayMix->setRange(0.0, 1.0, 0.01);
        delayMix->setValue(0.3);
        addAndMakeVisible(delayMix.get());
        
        delayMode.addItem("Mono", 1);
        delayMode.addItem("Ping-Pong", 2);
        delayMode.addItem("Stereo", 3);
        delayMode.setSelectedId(3, juce::dontSendNotification);
        addAndMakeVisible(delayMode);

        delayModRate = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "ModRt");
        delayModRate->setRange(0.1, 5.0, 0.01);
        delayModRate->setValue(0.0);
        addAndMakeVisible(delayModRate.get());
        
        delayModDepth = std::make_unique<RotaryKnob>(RotaryKnob::Size::Small, "ModDp");
        delayModDepth->setRange(0.0, 10.0, 0.1);
        delayModDepth->setValue(0.0);
        addAndMakeVisible(delayModDepth.get());
        
        // Sync toggle
        delaySyncBtn.setButtonText("Sync");
        delaySyncBtn.setClickingTogglesState(true);
        delaySyncBtn.setToggleState(true, juce::dontSendNotification);
        delaySyncBtn.setTooltip("Sync delay to tempo");
        addAndMakeVisible(delaySyncBtn);
        
        applyTheme();
    }
    
    void paint(juce::Graphics& g) override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds().toFloat();
        
        // Background with gradient (L2 elevation)
        juce::ColourGradient gradient(
            Colors::bgElevated, bounds.getCentreX(), bounds.getY(),
            Colors::bgSecondary, bounds.getCentreX(), bounds.getBottom(),
            false
        );
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(bounds, Radius::md);
        
        // Shadow (L2)
        juce::DropShadow shadow(Colors::bgPrimary.withAlpha(0.7f), 8, {-2, 0});
        shadow.drawForRectangle(g, getLocalBounds());
        
        // Border
        g.setColour(Colors::border);
        g.drawRoundedRectangle(bounds, Radius::md, 1.0f);
        
        // Section separators
        g.setColour(Colors::border.withAlpha(0.3f));
        
        // After meters
        float y = 40 + 120 + 40 + 80;
        g.drawLine(12, y, bounds.getWidth() - 12, y, 1.0f);
    }
    
    void resized() override
    {
        using namespace DesignTokens;
        
        auto bounds = getLocalBounds().reduced(Spacing::md);
        
        // Title
        titleLabel.setBounds(bounds.removeFromTop(24));
        bounds.removeFromTop(Spacing::sm);
        
        // Meters
        meters.setBounds(bounds.removeFromTop(120));
        bounds.removeFromTop(Spacing::sm);
        
        // Clip indicator
        clipIndicator.setBounds(bounds.removeFromTop(24).reduced(Spacing::lg, 0));
        bounds.removeFromTop(Spacing::sm);
        
        // Output gain
        auto gainBounds = bounds.removeFromTop(60);
        outputGain->setBounds(gainBounds.withSizeKeepingCentre(48, 60));
        bounds.removeFromTop(Spacing::lg);
        
        // FX section
        fxLabel.setBounds(bounds.removeFromTop(20));
        bounds.removeFromTop(Spacing::sm);
        
        // Reverb (4 knobs)
        auto reverbRow1 = bounds.removeFromTop(50);
        int knobSpacing = (reverbRow1.getWidth() - 32 * 4) / 3;
        reverbSize->setBounds(reverbRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        reverbRow1.removeFromLeft(knobSpacing);
        reverbDamp->setBounds(reverbRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        reverbRow1.removeFromLeft(knobSpacing);
        reverbPreDelay->setBounds(reverbRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        reverbRow1.removeFromLeft(knobSpacing);
        reverbDiffusion->setBounds(reverbRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        
        bounds.removeFromTop(Spacing::xs);
        
        auto reverbRow2 = bounds.removeFromTop(50);
        reverbMix->setBounds(reverbRow2.withSizeKeepingCentre(32, 50));
        
        bounds.removeFromTop(Spacing::sm);
        
        // Delay
        auto delayRow1 = bounds.removeFromTop(50);
        int dKnobSpacing = (delayRow1.getWidth() - 32 * 3) / 2;
        delayTime->setBounds(delayRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        delayRow1.removeFromLeft(dKnobSpacing);
        delayFeedback->setBounds(delayRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        delayRow1.removeFromLeft(dKnobSpacing);
        delayMix->setBounds(delayRow1.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        
        bounds.removeFromTop(Spacing::xs);
        
        auto delayRow2 = bounds.removeFromTop(50);
        int midWidth = 80;
        delayMode.setBounds(delayRow2.removeFromLeft(midWidth));
        delayRow2.removeFromLeft(Spacing::sm);
        delayModRate->setBounds(delayRow2.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        delayRow2.removeFromLeft(dKnobSpacing);
        delayModDepth->setBounds(delayRow2.removeFromLeft(32).withSizeKeepingCentre(32, 50));
        delayRow2.removeFromLeft(Spacing::sm);
        delaySyncBtn.setBounds(delayRow2.removeFromLeft(48).withHeight(24));
    }
    
    void setClipping(bool clipping)
    {
        if (clipping)
        {
            clipIndicator.setColour(juce::TextButton::buttonColourId, DesignTokens::Colors::danger);
            clipIndicator.setColour(juce::TextButton::textColourOffId, DesignTokens::Colors::textPrimary);
        }
        else
        {
            clipIndicator.setColour(juce::TextButton::buttonColourId, DesignTokens::Colors::danger.withAlpha(0.3f));
            clipIndicator.setColour(juce::TextButton::textColourOffId, DesignTokens::Colors::danger);
        }
        clipIndicator.repaint();
    }
    
    StereoMeter& getMeters() { return meters; }
    RotaryKnob& getOutputGain() { return *outputGain; }
    RotaryKnob& getReverbSize() { return *reverbSize; }
    RotaryKnob& getReverbDamp() { return *reverbDamp; }
    RotaryKnob& getReverbMix() { return *reverbMix; }
    RotaryKnob& getReverbPreDelay() { return *reverbPreDelay; }
    RotaryKnob& getReverbDiffusion() { return *reverbDiffusion; }
    RotaryKnob& getDelayTime() { return *delayTime; }
    RotaryKnob& getDelayFeedback() { return *delayFeedback; }
    RotaryKnob& getDelayMix() { return *delayMix; }
    juce::ComboBox& getDelayMode() { return delayMode; }
    RotaryKnob& getDelayModRate() { return *delayModRate; }
    RotaryKnob& getDelayModDepth() { return *delayModDepth; }
    
private:
    void applyTheme()
    {
        using namespace DesignTokens;
        
        delaySyncBtn.setColour(juce::TextButton::buttonColourId, Colors::bgTertiary);
        delaySyncBtn.setColour(juce::TextButton::buttonOnColourId, Colors::accent);
        delaySyncBtn.setColour(juce::TextButton::textColourOffId, Colors::textSecondary);
        delaySyncBtn.setColour(juce::TextButton::textColourOnId, Colors::textPrimary);
    }
    
    juce::Label titleLabel;
    StereoMeter meters;
    juce::TextButton clipIndicator;
    std::unique_ptr<RotaryKnob> outputGain;
    
    juce::Label fxLabel;
    std::unique_ptr<RotaryKnob> reverbSize;
    std::unique_ptr<RotaryKnob> reverbDamp;
    std::unique_ptr<RotaryKnob> reverbMix;
    std::unique_ptr<RotaryKnob> reverbPreDelay;
    std::unique_ptr<RotaryKnob> reverbDiffusion;
    std::unique_ptr<RotaryKnob> delayTime;
    std::unique_ptr<RotaryKnob> delayFeedback;
    std::unique_ptr<RotaryKnob> delayMix;
    juce::ComboBox delayMode;
    std::unique_ptr<RotaryKnob> delayModRate;
    std::unique_ptr<RotaryKnob> delayModDepth;
    juce::TextButton delaySyncBtn;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterPanel)
};
