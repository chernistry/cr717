#pragma once

#include <juce_dsp/juce_dsp.h>

class MasterDynamics
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        // Gentle compressor
        compressor.prepare({sampleRate, static_cast<juce::uint32>(maxBlockSize), 2});
        compressor.setThreshold(-12.0f);
        compressor.setRatio(3.0f);
        compressor.setAttack(10.0f);
        compressor.setRelease(100.0f);
        
        // Limiter
        limiter.prepare({sampleRate, static_cast<juce::uint32>(maxBlockSize), 2});
        limiter.setThreshold(-0.5f);
        limiter.setRelease(50.0f);
    }

    void setCompressorThreshold(float threshold)
    {
        compressor.setThreshold(threshold);
    }

    void setCompressorRatio(float ratio)
    {
        compressor.setRatio(ratio);
    }

    void setLimiterThreshold(float threshold)
    {
        limiter.setThreshold(threshold);
    }

    void process(juce::AudioBuffer<float>& buffer, bool compEnabled, bool limiterEnabled)
    {
        juce::dsp::AudioBlock<float> block(buffer);
        juce::dsp::ProcessContextReplacing<float> context(block);
        
        // Compressor
        if (compEnabled)
            compressor.process(context);
        
        // Limiter
        if (limiterEnabled)
            limiter.process(context);
        
        // Soft clip
        softClip(buffer);
    }

    void reset()
    {
        compressor.reset();
        limiter.reset();
    }

private:
    void softClip(juce::AudioBuffer<float>& buffer)
    {
        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            auto* data = buffer.getWritePointer(ch);
            for (int i = 0; i < buffer.getNumSamples(); ++i)
            {
                // Soft clip using tanh
                data[i] = std::tanh(data[i] * 0.9f);
            }
        }
    }

    juce::dsp::Compressor<float> compressor;
    juce::dsp::Limiter<float> limiter;
};
