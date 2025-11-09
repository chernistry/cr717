#pragma once

#include "Voice.h"

class BassDrumVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        // Prepare click buffer
        clickPhase = 0.0f;
        clickEnv = 0.0f;
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        active = true;
        
        // Trigger click
        clickPhase = 0.0f;
        clickEnv = velocity * 0.3f;
        pitchEnvTime = 0.0f;
        
        updateSmoothedValues();
    }

    bool isActive() const override { return active && (env > 0.0001f || clickEnv > 0.0001f); }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f && clickEnv <= 0.0001f)
            {
                active = false;
                break;
            }

            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float currentDecay = decay.getNextValue();
            float currentLevel = level.getNextValue();
            float currentTone = tone.getNextValue();

            // TR-808 spec: Bridged-T resonator at 56 Hz with pitch overshoot
            // Pitch envelope: brief overshoot then settle to 56 Hz over ~10ms
            pitchEnvTime += 1.0f / static_cast<float>(sampleRate);
            float pitchMult = 1.0f;
            if (pitchEnvTime < 0.01f) {
                // Exponential drop from overshoot
                pitchMult = 1.0f + (0.1f * std::exp(-pitchEnvTime / 0.003f));
            }
            
            float baseFreq = 56.0f * std::pow(2.0f, currentTune / 12.0f);
            float freq = (baseFreq * pitchMult) / static_cast<float>(sampleRate);
            
            // Generate sine wave (bridged-T resonator)
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env;
            
            // Simple tone LPF (~1.5 kHz cutoff)
            float lpfCutoff = 1500.0f + (currentTone * 500.0f);
            float lpfCoeff = std::exp(-juce::MathConstants<float>::twoPi * lpfCutoff / static_cast<float>(sampleRate));
            sample = sample * (1.0f - lpfCoeff) + lastLpf * lpfCoeff;
            lastLpf = sample;
            
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            
            // Click injection (HP-filtered pulse)
            if (clickEnv > 0.0001f) {
                float clickSample = (juce::Random::getSystemRandom().nextFloat() * 2.0f - 1.0f) * clickEnv;
                // Simple HP filter at 2 kHz
                float hpCoeff = std::exp(-juce::MathConstants<float>::twoPi * 2000.0f / static_cast<float>(sampleRate));
                clickSample = clickSample - lastHpf;
                lastHpf = lastHpf * hpCoeff + clickSample * (1.0f - hpCoeff);
                sample += clickSample * 0.3f;
                
                // Fast click decay
                clickEnv *= 0.95f;
            }
            
            // Amplitude envelope: 100-1000ms range (default 500ms)
            float decayTime = 0.1f + currentDecay * 0.9f;
            float decayRate = std::exp(-1.0f / (decayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            sample *= currentLevel;
            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f;
    float env = 0.0f;
    bool active = false;
    
    // Click injection
    float clickPhase = 0.0f;
    float clickEnv = 0.0f;
    
    // Pitch envelope
    float pitchEnvTime = 0.0f;
    
    // Filters
    float lastLpf = 0.0f;
    float lastHpf = 0.0f;
};
