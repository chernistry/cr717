#pragma once

#include "Voice.h"

class ClapVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 1500.0));
    }

    void trigger(float velocity) override
    {
        env = velocity;
        clapCount = 0;
        clapDelay = 0;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && (env > 0.0001f || clapCount < 3); }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            // Multi-clap effect
            if (clapDelay > 0)
            {
                clapDelay--;
                if (clapDelay == 0 && clapCount < 3)
                {
                    env = 0.8f - (clapCount * 0.15f);
                    clapCount++;
                    if (clapCount < 3)
                        clapDelay = static_cast<int>(sampleRate * 0.015);
                }
            }
            else if (clapCount == 0)
            {
                clapDelay = static_cast<int>(sampleRate * 0.015);
                clapCount = 1;
            }

            if (env <= 0.0001f && clapCount >= 3)
            {
                active = false;
                break;
            }

            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            
            float sample = noise * level.getNextValue();
            
            env *= 0.98f;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    int clapCount = 0;
    int clapDelay = 0;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};

class RimShotVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 2500.0));
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

            // Metallic tone
            float freq = (540.0f + tune.getNextValue() * 100.0f) / static_cast<float>(sampleRate);
            float tone = std::sin(phase * juce::MathConstants<float>::twoPi);
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;

            // Noise
            float noise = (random.nextFloat() * 2.0f - 1.0f);
            noise = filter.processSingleSampleRaw(noise);

            float sample = (tone * 0.3f + noise * 0.7f) * env * level.getNextValue();
            
            env *= 0.99f;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f;
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};
