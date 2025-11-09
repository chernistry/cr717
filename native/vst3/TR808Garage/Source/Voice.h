#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>

class Voice
{
public:
    Voice() = default;
    virtual ~Voice() = default;

    virtual void prepare(double sampleRate, int maxBlockSize) = 0;
    virtual void trigger(float velocity) = 0;
    virtual bool isActive() const = 0;
    virtual void renderNextBlock(juce::AudioBuffer<float>& buffer, int startSample, int numSamples) = 0;
    virtual void stop() { /* Override if needed for choke groups */ }
    
    void setLevel(float level) { targetLevel = level; }
    void setTune(float semitones) { targetTune = semitones; }
    void setFineTune(float cents) { targetFineTune = cents; }
    void setDecay(float decay) { targetDecay = decay; }
    void setTone(float tone) { targetTone = tone; }
    void setPan(float pan) { targetPan = pan; }

protected:
    double sampleRate = 44100.0;
    
    // Smoothed parameters
    juce::SmoothedValue<float> level{1.0f};
    juce::SmoothedValue<float> tune{0.0f};
    juce::SmoothedValue<float> fineTune{0.0f};
    juce::SmoothedValue<float> decay{0.5f};
    juce::SmoothedValue<float> tone{0.5f};
    juce::SmoothedValue<float> pan{0.0f};
    
    float targetLevel = 1.0f;
    float targetTune = 0.0f;
    float targetFineTune = 0.0f;
    float targetDecay = 0.5f;
    float targetTone = 0.5f;
    float targetPan = 0.0f;
    
    void updateSmoothedValues()
    {
        level.setTargetValue(targetLevel);
        tune.setTargetValue(targetTune);
        fineTune.setTargetValue(targetFineTune);
        decay.setTargetValue(targetDecay);
        tone.setTargetValue(targetTone);
        pan.setTargetValue(targetPan);
    }
    
    // Apply pan to stereo buffer
    void applyPan(juce::AudioBuffer<float>& buffer, int startSample, int numSamples, float sample)
    {
        float currentPan = pan.getNextValue();
        float leftGain = std::cos((currentPan + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
        float rightGain = std::sin((currentPan + 1.0f) * juce::MathConstants<float>::pi / 4.0f);
        
        buffer.getWritePointer(0)[startSample] += sample * leftGain;
        buffer.getWritePointer(1)[startSample] += sample * rightGain;
    }
};
