#pragma once

#include <juce_dsp/juce_dsp.h>

class MasterDynamics
{
public:
    void prepare(double sampleRate, int maxBlockSize)
    {
        spec = {sampleRate, static_cast<juce::uint32>(maxBlockSize), 2};
        
        // RMS detector window (10ms)
        rmsWindowSize = static_cast<int>(sampleRate * 0.01);
        rmsBuffer.resize(rmsWindowSize * 2, 0.0f);
        rmsIndex = 0;
        
        // Compressor lookahead delay
        lookaheadDelay.prepare(spec);
        lookaheadDelay.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.005)); // 5ms max
        
        // SC HPF (12dB/oct = 2-pole Butterworth)
        scHpf.prepare(spec);
        scHpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, 80.0f);
        
        // Limiter lookahead
        limiterLookahead.prepare(spec);
        limiterLookahead.setMaximumDelayInSamples(static_cast<int>(sampleRate * 0.01)); // 10ms max
        
        // True-peak oversampling (4x, linear-phase FIR)
        oversampling = std::make_unique<juce::dsp::Oversampling<float>>(2, 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple);
        oversampling->initProcessing(maxBlockSize);
        
        // Clipper oversampling (2x and 4x)
        clipperOversampling2x = std::make_unique<juce::dsp::Oversampling<float>>(2, 1, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple);
        clipperOversampling2x->initProcessing(maxBlockSize);
        clipperOversampling4x = std::make_unique<juce::dsp::Oversampling<float>>(2, 2, juce::dsp::Oversampling<float>::filterHalfBandFIREquiripple);
        clipperOversampling4x->initProcessing(maxBlockSize);
        
        // Smoothed parameters
        smoothedThreshold.reset(sampleRate, 0.02);
        smoothedRatio.reset(sampleRate, 0.02);
        smoothedAttack.reset(sampleRate, 0.02);
        smoothedRelease.reset(sampleRate, 0.02);
        smoothedKnee.reset(sampleRate, 0.02);
        smoothedMakeup.reset(sampleRate, 0.02);
        smoothedScHpf.reset(sampleRate, 0.05);
        smoothedLimiterCeiling.reset(sampleRate, 0.02);
        smoothedLimiterRelease.reset(sampleRate, 0.02);
        smoothedLimiterKnee.reset(sampleRate, 0.02);
        smoothedClipperDrive.reset(sampleRate, 0.02);
        smoothedClipperOutput.reset(sampleRate, 0.02);
        smoothedClipperMix.reset(sampleRate, 0.02);
        
        // Auto-makeup RMS tracking (300ms)
        autoMakeupWindowSize = static_cast<int>(sampleRate * 0.3);
        autoMakeupBuffer.resize(autoMakeupWindowSize * 2, 0.0f);
        autoMakeupIndex = 0;
        
        gainReduction = 0.0f;
        envelopeState = 0.0f;
        limiterEnvelope = 1.0f;
    }

    void setThreshold(float db) { smoothedThreshold.setTargetValue(db); }
    void setRatio(float r) { smoothedRatio.setTargetValue(r); }
    void setAttack(float ms) { smoothedAttack.setTargetValue(ms); }
    void setRelease(float ms) { smoothedRelease.setTargetValue(ms); }
    void setKnee(float db) { smoothedKnee.setTargetValue(db); }
    void setMakeup(float db) { smoothedMakeup.setTargetValue(db); }
    void setAutoMakeup(bool enabled) { autoMakeupEnabled = enabled; }
    void setScHpfFreq(float hz) { smoothedScHpf.setTargetValue(hz); }
    void setDetectorMode(bool useRms) { rmsMode = useRms; }
    void setLookahead(float ms) { lookaheadMs = ms; }
    void setLimiterCeiling(float db) { smoothedLimiterCeiling.setTargetValue(db); }
    void setLimiterRelease(float ms) { smoothedLimiterRelease.setTargetValue(ms); }
    void setLimiterKnee(float db) { smoothedLimiterKnee.setTargetValue(db); }
    void setLimiterLookahead(float ms) { limiterLookaheadMs = ms; }
    void setLimiterOversampling(bool enabled) { limiterOversamplingEnabled = enabled; }
    void setClipperDrive(float db) { smoothedClipperDrive.setTargetValue(db); }
    void setClipperOutput(float db) { smoothedClipperOutput.setTargetValue(db); }
    void setClipperMix(float percent) { smoothedClipperMix.setTargetValue(percent); }
    void setClipperCurve(int curve) { clipperCurve = curve; } // 0=tanh, 1=atan, 2=poly
    void setClipperOversampling(int factor) { clipperOversamplingFactor = factor; } // 0=off, 1=2x, 2=4x

    void process(juce::AudioBuffer<float>& buffer, bool compEnabled, bool limiterEnabled, bool clipperEnabled)
    {
        juce::ScopedNoDenormals noDenormals;
        const int blockSize = buffer.getNumSamples();
        // Ensure oversamplers are provisioned for this block size
        if (oversampling)
            oversampling->initProcessing(blockSize);
        if (clipperOversampling2x)
            clipperOversampling2x->initProcessing(blockSize);
        if (clipperOversampling4x)
            clipperOversampling4x->initProcessing(blockSize);
        
        if (compEnabled)
            processCompressor(buffer);
        
        if (clipperEnabled)
            processClipper(buffer);
        
        if (limiterEnabled)
            processTruePeakLimiter(buffer);
    }

    float getGainReduction() const { return gainReduction; }

    void reset()
    {
        lookaheadDelay.reset();
        limiterLookahead.reset();
        scHpf.reset();
        if (oversampling)
            oversampling->reset();
        if (clipperOversampling2x)
            clipperOversampling2x->reset();
        if (clipperOversampling4x)
            clipperOversampling4x->reset();
        std::fill(rmsBuffer.begin(), rmsBuffer.end(), 0.0f);
        std::fill(autoMakeupBuffer.begin(), autoMakeupBuffer.end(), 0.0f);
        rmsIndex = 0;
        autoMakeupIndex = 0;
        gainReduction = 0.0f;
        envelopeState = 0.0f;
        limiterEnvelope = 1.0f;
    }

private:
    void processCompressor(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        
        // Update SC HPF if needed
        float targetScHpf = smoothedScHpf.getNextValue();
        if (std::abs(targetScHpf - currentScHpf) > 0.1f)
        {
            currentScHpf = targetScHpf;
            scHpf.coefficients = juce::dsp::IIR::Coefficients<float>::makeHighPass(spec.sampleRate, currentScHpf);
        }
        
        // Lookahead delay setup
        int lookaheadSamples = static_cast<int>(spec.sampleRate * lookaheadMs * 0.001f);
        lookaheadDelay.setDelay(static_cast<float>(lookaheadSamples));
        
        // Attack/Release coefficients
        float attackCoeff = std::exp(-1.0f / (smoothedAttack.getNextValue() * 0.001f * spec.sampleRate));
        float releaseCoeff = std::exp(-1.0f / (smoothedRelease.getNextValue() * 0.001f * spec.sampleRate));
        
        float inputRms = 0.0f, outputRms = 0.0f;
        
        for (int i = 0; i < numSamples; ++i)
        {
            // Detector path: stereo sum
            float detectorSample = 0.0f;
            for (int ch = 0; ch < numChannels; ++ch)
                detectorSample += buffer.getSample(ch, i);
            detectorSample /= numChannels;
            
            // SC HPF
            float scFiltered = scHpf.processSample(detectorSample);
            
            // Level detection (RMS or Peak)
            float level = 0.0f;
            if (rmsMode)
            {
                // RMS window
                int idx = rmsIndex % rmsWindowSize;
                rmsBuffer[idx] = scFiltered * scFiltered;
                float sum = 0.0f;
                for (int j = 0; j < rmsWindowSize; ++j)
                    sum += rmsBuffer[j];
                level = std::sqrt(sum / rmsWindowSize);
                rmsIndex++;
            }
            else
            {
                level = std::abs(scFiltered);
            }
            
            float levelDb = juce::Decibels::gainToDecibels(level + 1e-6f);
            
            // Gain computer with soft knee
            float threshold = smoothedThreshold.getCurrentValue();
            float ratio = smoothedRatio.getCurrentValue();
            float knee = smoothedKnee.getCurrentValue();
            
            float gr = 0.0f;
            if (knee > 0.1f)
            {
                // Soft knee
                float kneeStart = threshold - knee * 0.5f;
                float kneeEnd = threshold + knee * 0.5f;
                
                if (levelDb < kneeStart)
                    gr = 0.0f;
                else if (levelDb > kneeEnd)
                    gr = (threshold - levelDb) + (levelDb - threshold) / ratio;
                else
                {
                    float x = levelDb - kneeStart;
                    float w = knee;
                    gr = (x * x) / (2.0f * w * ratio);
                }
            }
            else
            {
                // Hard knee
                if (levelDb > threshold)
                    gr = (threshold - levelDb) + (levelDb - threshold) / ratio;
            }
            
            // Envelope follower
            float targetGain = juce::Decibels::decibelsToGain(gr);
            if (targetGain < envelopeState)
                envelopeState = attackCoeff * envelopeState + (1.0f - attackCoeff) * targetGain;
            else
                envelopeState = releaseCoeff * envelopeState + (1.0f - releaseCoeff) * targetGain;
            
            // Track input RMS for auto-makeup
            if (autoMakeupEnabled)
            {
                int amIdx = autoMakeupIndex % autoMakeupWindowSize;
                autoMakeupBuffer[amIdx] = detectorSample * detectorSample;
                autoMakeupIndex++;
            }
            
            // Apply gain reduction with lookahead
            for (int ch = 0; ch < numChannels; ++ch)
            {
                float sample = buffer.getSample(ch, i);
                
                if (lookaheadSamples > 0)
                {
                    lookaheadDelay.pushSample(ch, sample);
                    sample = lookaheadDelay.popSample(ch);
                }
                
                sample *= envelopeState;
                
                // Track output RMS for auto-makeup
                if (autoMakeupEnabled && ch == 0)
                    outputRms += sample * sample;
                
                buffer.setSample(ch, i, sample);
            }
            
            if (autoMakeupEnabled && i == 0)
                inputRms = detectorSample * detectorSample;
        }
        
        // Auto-makeup calculation
        float makeup = smoothedMakeup.getNextValue();
        if (autoMakeupEnabled && numSamples > 0)
        {
            float inRms = 0.0f, outRms = 0.0f;
            for (int j = 0; j < autoMakeupWindowSize; ++j)
            {
                inRms += autoMakeupBuffer[j];
                outRms += autoMakeupBuffer[j] * envelopeState * envelopeState;
            }
            inRms = std::sqrt(inRms / autoMakeupWindowSize);
            outRms = std::sqrt(outRms / autoMakeupWindowSize);
            
            if (outRms > 1e-6f)
            {
                float autoGain = inRms / outRms;
                makeup = juce::Decibels::gainToDecibels(autoGain);
                makeup = juce::jlimit(-12.0f, 24.0f, makeup);
            }
        }
        
        // Apply makeup gain
        float makeupGain = juce::Decibels::decibelsToGain(makeup);
        buffer.applyGain(makeupGain);
        
        // Store GR for metering
        gainReduction = juce::Decibels::gainToDecibels(envelopeState);
    }

    void processClipper(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        
        float driveDb = smoothedClipperDrive.getNextValue();
        float outputDb = smoothedClipperOutput.getNextValue();
        float mix = smoothedClipperMix.getNextValue() * 0.01f; // 0-100% to 0-1
        
        float driveGain = juce::Decibels::decibelsToGain(driveDb);
        float outputGain = juce::Decibels::decibelsToGain(outputDb);
        
        // Store dry signal for parallel mix
        juce::AudioBuffer<float> dryBuffer(numChannels, numSamples);
        for (int ch = 0; ch < numChannels; ++ch)
            dryBuffer.copyFrom(ch, 0, buffer, ch, 0, numSamples);
        
        auto processClipCurve = [this](float sample) -> float
        {
            switch (clipperCurve)
            {
                case 0: // tanh (smooth)
                    return std::tanh(sample);
                case 1: // atan (harder)
                    return (2.0f / juce::MathConstants<float>::pi) * std::atan(sample * 1.5f);
                case 2: // polynomial (x - x^3/3)
                {
                    float x = juce::jlimit(-1.5f, 1.5f, sample);
                    return x - (x * x * x) / 3.0f;
                }
                default:
                    return std::tanh(sample);
            }
        };
        
        if (clipperOversamplingFactor > 0)
        {
            // Apply oversampling
            juce::dsp::AudioBlock<float> block(buffer);
            auto* os = (clipperOversamplingFactor == 1) ? clipperOversampling2x.get() : clipperOversampling4x.get();
            
            auto oversampledBlock = os->processSamplesUp(block);
            const int osNumSamples = static_cast<int>(oversampledBlock.getNumSamples());
            
            for (int i = 0; i < osNumSamples; ++i)
            {
                for (size_t ch = 0; ch < oversampledBlock.getNumChannels(); ++ch)
                {
                    float sample = oversampledBlock.getSample(ch, i);
                    sample *= driveGain;
                    sample = processClipCurve(sample);
                    sample *= outputGain;
                    oversampledBlock.setSample(ch, i, sample);
                }
            }
            
            os->processSamplesDown(block);
        }
        else
        {
            // No oversampling
            for (int ch = 0; ch < numChannels; ++ch)
            {
                auto* data = buffer.getWritePointer(ch);
                for (int i = 0; i < numSamples; ++i)
                {
                    float sample = data[i];
                    sample *= driveGain;
                    sample = processClipCurve(sample);
                    sample *= outputGain;
                    data[i] = sample;
                }
            }
        }
        
        // Parallel mix
        if (mix < 0.999f)
        {
            for (int ch = 0; ch < numChannels; ++ch)
            {
                auto* wet = buffer.getWritePointer(ch);
                auto* dry = dryBuffer.getReadPointer(ch);
                for (int i = 0; i < numSamples; ++i)
                    wet[i] = dry[i] * (1.0f - mix) + wet[i] * mix;
            }
        }
    }

    void processTruePeakLimiter(juce::AudioBuffer<float>& buffer)
    {
        const int numSamples = buffer.getNumSamples();
        const int numChannels = buffer.getNumChannels();
        
        float ceiling = smoothedLimiterCeiling.getNextValue();
        float knee = smoothedLimiterKnee.getNextValue();
        float releaseMs = smoothedLimiterRelease.getNextValue();
        
        int lookaheadSamples = static_cast<int>(spec.sampleRate * limiterLookaheadMs * 0.001f);
        limiterLookahead.setDelay(static_cast<float>(lookaheadSamples));
        
        float releaseCoeff = std::exp(-1.0f / (releaseMs * 0.001f * spec.sampleRate));
        float ceilingGain = juce::Decibels::decibelsToGain(ceiling);
        
        if (limiterOversamplingEnabled && oversampling)
        {
            // True-peak detection via 4x oversampling
            juce::dsp::AudioBlock<float> block(buffer);
            auto oversampledBlock = oversampling->processSamplesUp(block);
            
            const int osNumSamples = static_cast<int>(oversampledBlock.getNumSamples());
            
            for (int i = 0; i < osNumSamples; ++i)
            {
                float peak = 0.0f;
                for (size_t ch = 0; ch < oversampledBlock.getNumChannels(); ++ch)
                    peak = std::max(peak, std::abs(oversampledBlock.getSample(ch, i)));
                
                // Gain computer with soft knee
                float targetGain = 1.0f;
                if (peak > ceilingGain)
                {
                    if (knee > 0.01f)
                    {
                        float kneeStart = ceilingGain * (1.0f - knee * 0.1f);
                        if (peak > kneeStart)
                        {
                            float excess = (peak - kneeStart) / (ceilingGain - kneeStart);
                            targetGain = kneeStart / peak + (1.0f - excess) * (ceilingGain - kneeStart) / peak;
                        }
                    }
                    else
                    {
                        targetGain = ceilingGain / peak;
                    }
                }
                
                // Instant attack, smooth release
                if (targetGain < limiterEnvelope)
                    limiterEnvelope = targetGain;
                else
                    limiterEnvelope = releaseCoeff * limiterEnvelope + (1.0f - releaseCoeff) * targetGain;
                
                // Apply gain
                for (size_t ch = 0; ch < oversampledBlock.getNumChannels(); ++ch)
                    oversampledBlock.setSample(ch, i, oversampledBlock.getSample(ch, i) * limiterEnvelope);
            }
            
            oversampling->processSamplesDown(block);
        }
        else
        {
            // Standard peak limiting (no oversampling)
            for (int i = 0; i < numSamples; ++i)
            {
                float peak = 0.0f;
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float sample = buffer.getSample(ch, i);
                    if (lookaheadSamples > 0)
                    {
                        limiterLookahead.pushSample(ch, sample);
                        sample = limiterLookahead.popSample(ch);
                    }
                    peak = std::max(peak, std::abs(sample));
                }
                
                float targetGain = 1.0f;
                if (peak > ceilingGain)
                    targetGain = ceilingGain / peak;
                
                if (targetGain < limiterEnvelope)
                    limiterEnvelope = targetGain;
                else
                    limiterEnvelope = releaseCoeff * limiterEnvelope + (1.0f - releaseCoeff) * targetGain;
                
                for (int ch = 0; ch < numChannels; ++ch)
                {
                    float sample = buffer.getSample(ch, i);
                    if (lookaheadSamples > 0)
                    {
                        limiterLookahead.pushSample(ch, sample);
                        sample = limiterLookahead.popSample(ch);
                    }
                    buffer.setSample(ch, i, sample * limiterEnvelope);
                }
            }
        }
    }

    juce::dsp::ProcessSpec spec;
    juce::dsp::DelayLine<float> lookaheadDelay{1024};
    juce::dsp::DelayLine<float> limiterLookahead{2048};
    juce::dsp::IIR::Filter<float> scHpf;
    std::unique_ptr<juce::dsp::Oversampling<float>> oversampling;
    std::unique_ptr<juce::dsp::Oversampling<float>> clipperOversampling2x;
    std::unique_ptr<juce::dsp::Oversampling<float>> clipperOversampling4x;
    
    juce::SmoothedValue<float> smoothedThreshold, smoothedRatio, smoothedAttack, smoothedRelease;
    juce::SmoothedValue<float> smoothedKnee, smoothedMakeup, smoothedScHpf;
    juce::SmoothedValue<float> smoothedLimiterCeiling, smoothedLimiterRelease, smoothedLimiterKnee;
    juce::SmoothedValue<float> smoothedClipperDrive, smoothedClipperOutput, smoothedClipperMix;
    
    std::vector<float> rmsBuffer, autoMakeupBuffer;
    int rmsWindowSize = 0, rmsIndex = 0;
    int autoMakeupWindowSize = 0, autoMakeupIndex = 0;
    
    float gainReduction = 0.0f, envelopeState = 0.0f, limiterEnvelope = 1.0f;
    float currentScHpf = 80.0f, lookaheadMs = 0.0f, limiterLookaheadMs = 5.0f;
    bool rmsMode = true, autoMakeupEnabled = false, limiterOversamplingEnabled = true;
    int clipperCurve = 0, clipperOversamplingFactor = 2; // 0=off, 1=2x, 2=4x
};
