#pragma once

#include "Voice.h"

class SnareDrumVoice : public Voice
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
        
        filter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 2000.0));
    }

    void trigger(float velocity) override
    {
        phase = 0.0f;
        env = velocity;
        noiseEnv = velocity;
        active = true;
        updateSmoothedValues();
    }

    bool isActive() const override { return active && (env > 0.0001f || noiseEnv > 0.0001f); }

    void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) override
    {
        if (!active) return;

        for (int i = 0; i < numSamples; ++i)
        {
            if (env <= 0.0001f && noiseEnv <= 0.0001f)
            {
                active = false;
                break;
            }

            float currentLevel = level.getNextValue();
            float currentDecay = decay.getNextValue();
            float currentTone = tone.getNextValue();
            float currentTune = tune.getNextValue() + fineTune.getNextValue();

            // Body tone (180Hz)
            float freq = (180.0f + currentTune * 20.0f) / static_cast<float>(sampleRate);
            float body = std::sin(phase * juce::MathConstants<float>::twoPi) * env * 0.5f;
            phase += freq;
            if (phase >= 1.0f) phase -= 1.0f;

            // Noise component
            float noise = (random.nextFloat() * 2.0f - 1.0f) * noiseEnv;
            noise = filter.processSingleSampleRaw(noise);

            float sample = (body + noise * currentTone) * currentLevel;

            env *= 0.996f - (currentDecay * 0.003f);
            noiseEnv *= 0.993f - (currentDecay * 0.003f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase = 0.0f;
    float env = 0.0f;
    float noiseEnv = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter filter;
};
