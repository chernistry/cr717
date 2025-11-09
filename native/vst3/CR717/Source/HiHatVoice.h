#pragma once

#include "Voice.h"

// TR-808 spec: Six square-wave oscillators at exact frequencies
static const float OSC_FREQS[6] = { 205.3f, 304.4f, 369.6f, 522.7f, 540.0f, 800.0f };

class ClosedHiHatVoice : public Voice
{
public:
    void prepare(double sr, int maxBlockSize) override
    {
        sampleRate = sr;
        level.reset(sr, 0.02);
        tone.reset(sr, 0.02);
        pan.reset(sr, 0.02);
        
        // Dual 3rd-order BPF at 3.44 kHz and 7.10 kHz
        bp1.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 3440.0, 2.0));
        bp2.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 7100.0, 2.0));
        
        for (int i = 0; i < 6; ++i) {
            phases[i] = 0.0f;
        }
    }

    void trigger(float velocity) override
    {
        env = velocity;
        active = true;
        updateSmoothedValues();
    }
    
    void stop() override
    {
        // Fast choke (2-5ms)
        env *= 0.01f;
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

            // Generate 6 square oscillators
            float oscSum = 0.0f;
            for (int j = 0; j < 6; ++j) {
                float freq = OSC_FREQS[j] / static_cast<float>(sampleRate);
                oscSum += (phases[j] < 0.5f ? 1.0f : -1.0f) * 0.15f;
                phases[j] += freq;
                if (phases[j] >= 1.0f) phases[j] -= 1.0f;
            }
            
            // Apply dual BPF
            float filtered = bp1.processSingleSampleRaw(oscSum);
            filtered = bp2.processSingleSampleRaw(filtered);
            
            float sample = filtered * env * level.getNextValue();
            
            // Expo decay: 190ms
            float decayRate = std::exp(-1.0f / (0.19f * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phases[6] = {0};
    float env = 0.0f;
    bool active = false;
    juce::IIRFilter bp1, bp2;
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
        
        // Dual 3rd-order BPF at 3.44 kHz and 7.10 kHz
        bp1.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 3440.0, 2.0));
        bp2.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 7100.0, 2.0));
        
        for (int i = 0; i < 6; ++i) {
            phases[i] = 0.0f;
        }
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
            
            // Generate 6 square oscillators
            float oscSum = 0.0f;
            for (int j = 0; j < 6; ++j) {
                float freq = OSC_FREQS[j] / static_cast<float>(sampleRate);
                oscSum += (phases[j] < 0.5f ? 1.0f : -1.0f) * 0.15f;
                phases[j] += freq;
                if (phases[j] >= 1.0f) phases[j] -= 1.0f;
            }
            
            // Apply dual BPF
            float filtered = bp1.processSingleSampleRaw(oscSum);
            filtered = bp2.processSingleSampleRaw(filtered);
            
            float sample = filtered * env * level.getNextValue();
            
            // Expo decay: 490ms (longer than CH)
            float baseDecayTime = 0.49f * (0.5f + currentDecay * 0.5f);
            float decayRate = std::exp(-1.0f / (baseDecayTime * static_cast<float>(sampleRate)));
            env *= decayRate;

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phases[6] = {0};
    float env = 0.0f;
    bool active = false;
    juce::IIRFilter bp1, bp2;
};
