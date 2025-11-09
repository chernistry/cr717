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
        
        // HP filter at 700 Hz for noise
        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sr, 700.0));
        // BP filter at 1500 Hz for noise
        bpFilter.setCoefficients(juce::IIRCoefficients::makeBandPass(sr, 1500.0, 1.0));
    }

    void trigger(float velocity) override
    {
        phase1 = 0.0f;
        phase2 = 0.0f;
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

            // TR-808 spec: Dual resonators at 180 Hz and 330 Hz
            float tuneMultiplier = std::pow(2.0f, currentTune / 12.0f);
            float freq1 = (180.0f * tuneMultiplier) / static_cast<float>(sampleRate);
            float freq2 = (330.0f * tuneMultiplier) / static_cast<float>(sampleRate);
            
            float body1 = std::sin(phase1 * juce::MathConstants<float>::twoPi) * env * 0.5f;
            float body2 = std::sin(phase2 * juce::MathConstants<float>::twoPi) * env * 0.3f;
            
            phase1 += freq1;
            phase2 += freq2;
            if (phase1 >= 1.0f) phase1 -= 1.0f;
            if (phase2 >= 1.0f) phase2 -= 1.0f;

            // Noise component (HP/BP 700-3kHz)
            float noise = (random.nextFloat() * 2.0f - 1.0f) * noiseEnv;
            noise = hpFilter.processSingleSampleRaw(noise);
            noise = bpFilter.processSingleSampleRaw(noise);

            // Tone controls body vs noise mix
            float bodyMix = currentTone;
            float noiseMix = 0.6f * (1.0f - currentTone * 0.5f);  // Snappy control
            float sample = ((body1 + body2) * bodyMix + noise * noiseMix) * currentLevel;

            // Body decay: 250ms, Noise decay: 200ms
            float bodyDecayRate = std::exp(-1.0f / (0.25f * static_cast<float>(sampleRate)));
            float noiseDecayRate = std::exp(-1.0f / (0.20f * static_cast<float>(sampleRate)));
            
            env *= bodyDecayRate * (0.95f + currentDecay * 0.05f);
            noiseEnv *= noiseDecayRate * (0.95f + currentDecay * 0.05f);

            applyPan(buffer, startSample + i, numSamples, sample);
        }
    }

private:
    float phase1 = 0.0f;
    float phase2 = 0.0f;
    float env = 0.0f;
    float noiseEnv = 0.0f;
    bool active = false;
    juce::Random random;
    juce::IIRFilter hpFilter;
    juce::IIRFilter bpFilter;
};
