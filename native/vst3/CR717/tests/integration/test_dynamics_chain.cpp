#include "../../Source/MasterDynamics.h"
#include <cassert>
#include <iostream>

void testFullChain()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    // Configure chain: comp + clipper + limiter
    dynamics.setThreshold(-12.0f);
    dynamics.setRatio(4.0f);
    dynamics.setAttack(10.0f);
    dynamics.setRelease(100.0f);
    
    dynamics.setClipperDrive(6.0f);
    dynamics.setClipperMix(50.0f);
    dynamics.setClipperCurve(0);
    dynamics.setClipperOversampling(2);
    
    dynamics.setLimiterCeiling(-0.3f);
    dynamics.setLimiterRelease(50.0f);
    dynamics.setLimiterOversampling(true);
    
    // Hot signal
    juce::AudioBuffer<float> buffer(2, 512);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, 0.8f);
    
    // Process full chain
    dynamics.process(buffer, true, true, true);
    
    // Verify output is controlled
    float peak = 0.0f;
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            peak = std::max(peak, std::abs(buffer.getSample(ch, i)));
    
    float ceilingGain = juce::Decibels::decibelsToGain(-0.3f);
    
    std::cout << "Integration Test: Full Chain - Peak: " << peak 
              << ", Ceiling: " << ceilingGain << std::endl;
    
    assert(peak <= ceilingGain * 1.02f); // Allow 2% tolerance
    assert(peak > 0.1f); // Not silent
    
    std::cout << "✓ Full dynamics chain working correctly" << std::endl;
}

int main()
{
    std::cout << "=== Dynamics Chain Integration Test ===" << std::endl;
    testFullChain();
    return 0;
}
