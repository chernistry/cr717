#pragma once

#include <juce_dsp/juce_dsp.h>

class AlgorithmicReverb
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        this->sampleRate = sampleRate;
        
        juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = static_cast<juce::uint32>(maxBlockSize);
        spec.numChannels = 2;
        reverb.prepare(spec);
        
        // Pre-delay (max 100ms)
        preDelayLine.prepare(spec);
        preDelayLine.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.1));
        preDelayLine.setDelay(static_cast<float>(sampleRate * 0.02)); // 20ms default
        
        // Diffusers (first-order all-pass stages pre reverb)
        diffuserL1.reset(); diffuserL2.reset();
        diffuserR1.reset(); diffuserR2.reset();
        diffuserL1.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        diffuserL2.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        diffuserR1.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        diffuserR2.setType(juce::dsp::FirstOrderTPTFilterType::allpass);
        diffuserL1.prepare(spec); diffuserL2.prepare(spec);
        diffuserR1.prepare(spec); diffuserR2.prepare(spec);
        
        juce::dsp::Reverb::Parameters params;
        params.roomSize = 0.5f;
        params.damping = 0.5f;
        params.wetLevel = 0.33f;
        params.dryLevel = 0.0f;
        params.width = 1.0f;
        params.freezeMode = 0.0f;
        reverb.setParameters(params);
        
        diffusion = 0.7f;
    }

    void setRoomSize(float size)
    {
        auto params = reverb.getParameters();
        params.roomSize = juce::jlimit(0.0f, 1.0f, size);
        reverb.setParameters(params);
    }

    void setDamping(float damp)
    {
        auto params = reverb.getParameters();
        params.damping = juce::jlimit(0.0f, 1.0f, damp);
        reverb.setParameters(params);
    }

    void setWidth(float width)
    {
        auto params = reverb.getParameters();
        params.width = juce::jlimit(0.0f, 1.0f, width);
        reverb.setParameters(params);
    }

    void setWetLevel(float wet)
    {
        auto params = reverb.getParameters();
        params.wetLevel = juce::jlimit(0.0f, 1.0f, wet);
        reverb.setParameters(params);
    }

    void setPreDelay(float ms)
    {
        float delaySamples = (ms * 0.001f) * sampleRate;
        preDelayLine.setDelay(delaySamples);
    }

    void setDiffusion(float diff)
    {
        diffusion = juce::jlimit(0.0f, 1.0f, diff);
        // Map diffusion into all-pass cutoff frequencies
        // Lower diffusion -> lower cutoff (less smearing), higher diffusion -> higher cutoff
        float f1 = juce::jmap(diffusion, 0.0f, 1.0f, 800.0f, 4000.0f);
        float f2 = juce::jmap(diffusion, 0.0f, 1.0f, 1200.0f, 6000.0f);
        diffuserL1.setCutoffFrequency(f1);
        diffuserL2.setCutoffFrequency(f2);
        diffuserR1.setCutoffFrequency(f1);
        diffuserR2.setCutoffFrequency(f2);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        
        // Apply pre-delay
        juce::dsp::ProcessContextReplacing<float> preDelayContext(block);
        preDelayLine.process(preDelayContext);
        
        // Apply pre-diffusion via two all-pass stages per channel
        auto* left = buffer.getWritePointer(0);
        auto* right = buffer.getWritePointer(1);
        int n = buffer.getNumSamples();
        for (int i = 0; i < n; ++i)
        {
            left[i]  = diffuserL2.processSample(0, diffuserL1.processSample(0, left[i]));
            right[i] = diffuserR2.processSample(1, diffuserR1.processSample(1, right[i]));
        }
        
        // Apply reverb
        juce::dsp::ProcessContextReplacing<float> context(block);
        reverb.process(context);
    }

    void reset()
    {
        reverb.reset();
        preDelayLine.reset();
    }

private:
    juce::dsp::Reverb reverb;
    juce::dsp::DelayLine<float> preDelayLine{8192};
    juce::dsp::FirstOrderTPTFilter<float> diffuserL1, diffuserL2, diffuserR1, diffuserR2;
    double sampleRate = 48000.0;
    float diffusion = 0.7f;
};
