#pragma once

#include "Voice.h"

class ClosedHiHatVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 7000.0));
    }

    void trigger(float velocity) override
    {
        env = velocity;
        active = true;
        updateSmoothedValues();
    }
    
    void stop() override
    {
        env = 0.0f;
        active = false;
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

            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            
            float sample = noise * level.getNextValue() * 0.6f;
            
            env *= 0.985f;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};

class OpenHiHatVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 6000.0));
    }

    void trigger(float velocity) override
    {
        env = velocity;
        active = true;
        updateSmoothedValues();
    }
    
    void stop() override
    {
        env = 0.0f;
        active = false;
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

            float currentDecay = decay.getNextValue();
            
            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            
            float sample = noise * level.getNextValue() * 0.5f;
            
            env *= 0.9985f - (currentDecay * 0.002f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};
