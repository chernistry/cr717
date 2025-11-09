#pragma once

#include "Voice.h"

class CymbalVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        decay.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 8000.0));
    }

    void trigger(float velocity) override { env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            float sample = noise * level.getNextValue() * 0.4f;
            env *= 0.9992f - (decay.getNextValue() * 0.001f);
            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};

class RideVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 7500.0));
    }

    void trigger(float velocity) override { env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            float noise = (random.nextFloat() * 2.0f - 1.0f) * env;
            noise = filter.processSingleSampleRaw(noise);
            float sample = noise * level.getNextValue() * 0.35f;
            env *= 0.9994f;
            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float env = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};

class CowbellVoice : public Voice
{
public:
    void prepare(double sr, int) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tune.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        filter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 800.0));
    }

    void trigger(float velocity) override { phase1 = 0.0f; phase2 = 0.0f; env = velocity; active = true; updateSmoothedValues(); }
    bool isActive() const override { return active && env > 0.0001f; }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f) { active = false; break; }
            float tuneVal = tune.getNextValue();
            float f1 = (540.0f + tuneVal * 50.0f) / static_cast<float>(sampleRate);
            float f2 = (800.0f + tuneVal * 70.0f) / static_cast<float>(sampleRate);
            
            float osc = std::sin(phase1 * juce::MathConstants<float>::twoPi) * 0.5f +
                       std::sin(phase2 * juce::MathConstants<float>::twoPi) * 0.5f;
            phase1 += f1; if (phase1 >= 1.0f) phase1 -= 1.0f;
            phase2 += f2; if (phase2 >= 1.0f) phase2 -= 1.0f;
            
            float sample = filter.processSingleSampleRaw(osc) * env * level.getNextValue();
            env *= 0.992f;
            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase1 = 0.0f, phase2 = 0.0f, env = 0.0f;
    bool active = false;
    juce::IIRFilter filter;
};
