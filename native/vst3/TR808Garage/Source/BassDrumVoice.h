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
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f)
            {
                active = false;
                break;
            }

            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float currentDecay = decay.getNextValue();
            float currentLevel = level.getNextValue();

            // Pitch envelope: 65Hz -> 50Hz
            float pitchEnv = 65.0f + (currentTune * 12.0f) + (env * 15.0f);
            float freq = pitchEnv / static_cast<float>(sampleRate);
            
            // Generate sine
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * currentLevel;
            
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            
            // Decay
            float decayRate = 0.9995f - (currentDecay * 0.002f);
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f;
    float env = 0.0f;
    bool active = false;
};
