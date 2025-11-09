#pragma once

#include <juce_dsp/juce_dsp.h>

class TempoSyncDelay
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        this->sampleRate = sampleRate;
        
        int maxDelaySamples = static_cast<int>(sampleRate * 2.0); // 2 seconds max
        delayLineL.resize(maxDelaySamples, 0.0f);
        delayLineR.resize(maxDelaySamples, 0.0f);
        
        writePos = 0;
        
        lpFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, 8000.0));
        hpFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, 200.0));
    }

    void setDelayTime(float beats, double bpm)
    {
        // Convert beats to seconds
        double delaySeconds = (beats * 60.0) / bpm;
        delaySamples = static_cast<int>(delaySeconds * sampleRate);
        delaySamples = juce::jlimit(1, static_cast<int>(delayLineL.size()) - 1, delaySamples);
    }

    void setFeedback(float fb)
    {
        feedback = juce::jlimit(0.0f, 0.95f, fb);
    }

    void setWetLevel(float wet)
    {
        wetLevel = juce::jlimit(0.0f, 1.0f, wet);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        auto* leftChannel = buffer.getWritePointer(0);
        auto* rightChannel = buffer.getWritePointer(1);
        int numSamples = buffer.getNumSamples();

        for (int i = 0; i < numSamples; ++i)
        {
            // Read from delay line
            int readPos = (writePos - delaySamples + delayLineL.size()) % delayLineL.size();
            float delayedL = delayLineL[readPos];
            float delayedR = delayLineR[readPos];

            // Apply filters to feedback
            delayedL = lpFilter.processSingleSampleRaw(delayedL);
            delayedL = hpFilter.processSingleSampleRaw(delayedL);
            delayedR = lpFilter.processSingleSampleRaw(delayedR);
            delayedR = hpFilter.processSingleSampleRaw(delayedR);

            // Write to delay line (input + feedback)
            delayLineL[writePos] = leftChannel[i] + delayedL * feedback;
            delayLineR[writePos] = rightChannel[i] + delayedR * feedback;

            // Mix wet/dry
            leftChannel[i] = leftChannel[i] * (1.0f - wetLevel) + delayedL * wetLevel;
            rightChannel[i] = rightChannel[i] * (1.0f - wetLevel) + delayedR * wetLevel;

            writePos = (writePos + 1) % delayLineL.size();
        }
    }

    void reset()
    {
        std::fill(delayLineL.begin(), delayLineL.end(), 0.0f);
        std::fill(delayLineR.begin(), delayLineR.end(), 0.0f);
        writePos = 0;
        lpFilter.reset();
        hpFilter.reset();
    }

private:
    double sampleRate = 44100.0;
    std::vector<float> delayLineL, delayLineR;
    int writePos = 0;
    int delaySamples = 0;
    float feedback = 0.5f;
    float wetLevel = 0.5f;
    juce::IIRFilter lpFilter, hpFilter;
};
