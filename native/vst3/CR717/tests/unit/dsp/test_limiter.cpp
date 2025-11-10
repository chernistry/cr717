#include "../../../Source/MasterDynamics.h"
#include <cassert>
#include <cmath>
#include <iostream>

void testCeilingEnforcement()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    // Configure limiter: -0.3dB ceiling
    dynamics.setLimiterCeiling(-0.3f);
    dynamics.setLimiterRelease(50.0f);
    dynamics.setLimiterKnee(0.0f);
    dynamics.setLimiterLookahead(5.0f);
    dynamics.setLimiterOversampling(true);
    
    // Create hot signal: 0dB (above ceiling)
    juce::AudioBuffer<float> buffer(2, 512);
    float testLevel = 1.0f; // 0dBFS
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, testLevel);
    
    // Process
    dynamics.process(buffer, false, true);
    
    // Check peak doesn't exceed ceiling
    float ceilingGain = juce::Decibels::decibelsToGain(-0.3f);
    float peak = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            peak = std::max(peak, std::abs(buffer.getSample(ch, i)));
    
    std::cout << "Test: Ceiling Enforcement - Ceiling: " << ceilingGain 
              << ", Peak: " << peak << std::endl;
    assert(peak <= ceilingGain * 1.01f); // Allow 1% tolerance
}

void testTransientPreservation()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setLimiterCeiling(-0.3f);
    dynamics.setLimiterRelease(100.0f);
    dynamics.setLimiterKnee(0.5f);
    dynamics.setLimiterLookahead(5.0f);
    dynamics.setLimiterOversampling(true);
    
    // Create transient: impulse
    juce::AudioBuffer<float> buffer(2, 512);
    buffer.clear();
    buffer.setSample(0, 100, 1.0f); // Sharp transient
    buffer.setSample(1, 100, 1.0f);
    
    dynamics.process(buffer, false, true);
    
    // Check transient was limited but not destroyed
    float peak = std::max(std::abs(buffer.getSample(0, 100)), 
                          std::abs(buffer.getSample(1, 100)));
    
    std::cout << "Test: Transient Preservation - Peak after limiting: " << peak << std::endl;
    assert(peak > 0.5f && peak <= juce::Decibels::decibelsToGain(-0.3f) * 1.01f);
}

void testOversamplingToggle()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setLimiterCeiling(-0.3f);
    dynamics.setLimiterRelease(50.0f);
    dynamics.setLimiterKnee(0.0f);
    dynamics.setLimiterLookahead(5.0f);
    
    juce::AudioBuffer<float> buffer1(2, 512);
    juce::AudioBuffer<float> buffer2(2, 512);
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
        {
            float val = 0.9f;
            buffer1.setSample(ch, i, val);
            buffer2.setSample(ch, i, val);
        }
    
    // Process with oversampling
    dynamics.setLimiterOversampling(true);
    dynamics.process(buffer1, false, true);
    
    // Process without oversampling
    dynamics.reset();
    dynamics.setLimiterOversampling(false);
    dynamics.process(buffer2, false, true);
    
    std::cout << "Test: Oversampling Toggle - Both modes processed successfully" << std::endl;
    // Both should work without crashing
}

void testSoftKnee()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setLimiterCeiling(-1.0f);
    dynamics.setLimiterRelease(50.0f);
    dynamics.setLimiterLookahead(5.0f);
    dynamics.setLimiterOversampling(false);
    
    juce::AudioBuffer<float> bufferHard(2, 512);
    juce::AudioBuffer<float> bufferSoft(2, 512);
    
    float testLevel = juce::Decibels::decibelsToGain(-0.5f); // Near ceiling
    
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
        {
            bufferHard.setSample(ch, i, testLevel);
            bufferSoft.setSample(ch, i, testLevel);
        }
    
    // Hard knee
    dynamics.setLimiterKnee(0.0f);
    dynamics.process(bufferHard, false, true);
    
    // Soft knee
    dynamics.reset();
    dynamics.setLimiterKnee(2.0f);
    dynamics.process(bufferSoft, false, true);
    
    std::cout << "Test: Soft Knee - Hard and soft knee modes processed" << std::endl;
}

int main()
{
    std::cout << "=== True-Peak Limiter Unit Tests ===" << std::endl;
    
    testCeilingEnforcement();
    testTransientPreservation();
    testOversamplingToggle();
    testSoftKnee();
    
    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;
}
