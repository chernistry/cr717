#pragma once

#include "Voice.h"

class LowTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
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
            if (env <= 0.0001f) { active = false; break; }
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float freq = (105.0f + currentTune * 10.0f) / static_cast<float>(sampleRate);
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            env *= 0.996f - (decay.getNextValue() * 0.002f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f;
    bool active = false;
};

class MidTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
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
            if (env <= 0.0001f) { active = false; break; }
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float freq = (125.0f + currentTune * 10.0f) / static_cast<float>(sampleRate);
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            env *= 0.996f - (decay.getNextValue() * 0.002f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f;
    bool active = false;
};

class HighTomVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        fineTune.reset(sr, 0.02);
        decay.reset(sr, 0.02);
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
            if (env <= 0.0001f) { active = false; break; }
            
            float currentTune = tune.getNextValue() + fineTune.getNextValue();
            float freq = (155.0f + currentTune * 10.0f) / static_cast<float>(sampleRate);
            float sample = std::sin(phase * juce::MathConstants<float>::twoPi) * env * level.getNextValue();
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;
            env *= 0.996f - (decay.getNextValue() * 0.002f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f, env = 0.0f;
    bool active = false;
};
