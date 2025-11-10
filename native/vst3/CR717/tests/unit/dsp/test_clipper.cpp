#include "../../../Source/MasterDynamics.h"
#include <cassert>
#include <cmath>
#include <iostream>

void testNoBoundedOutput()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setClipperDrive(12.0f);
    dynamics.setClipperOutput(0.0f);
    dynamics.setClipperMix(100.0f);
    dynamics.setClipperCurve(0); // tanh
    dynamics.setClipperOversampling(2); // 4x
    
    juce::AudioBuffer<float> buffer(2, 512);
    for (int ch = 0; ch < 2; ++ch)
        for (int i = 0; i < 512; ++i)
            buffer.setSample(ch, i, 2.0f); // Hot signal
    
    dynamics.process(buffer, false, false, true);
    
    // Check no NaN and bounded output
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            float sample = buffer.getSample(ch, i);
            assert(!std::isnan(sample));
            assert(std::abs(sample) < 2.0f); // Should be clipped
        }
    }
    
    std::cout << "Test: No NaN, Bounded Output - Passed" << std::endl;
}

void testCurveSelection()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setClipperDrive(6.0f);
    dynamics.setClipperOutput(0.0f);
    dynamics.setClipperMix(100.0f);
    dynamics.setClipperOversampling(0); // Off for speed
    
    const char* curveNames[] = {"Tanh", "Atan", "Poly"};
    
    for (int curve = 0; curve < 3; ++curve)
    {
        dynamics.reset();
        dynamics.setClipperCurve(curve);
        
        juce::AudioBuffer<float> buffer(2, 512);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, 1.5f);
        
        dynamics.process(buffer, false, false, true);
        
        float peak = 0.0f;
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                peak = std::max(peak, std::abs(buffer.getSample(ch, i)));
        
        std::cout << "Test: Curve " << curveNames[curve] << " - Peak: " << peak << std::endl;
        assert(peak < 1.5f); // Should be clipped
    }
}

void testParallelMix()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setClipperDrive(12.0f);
    dynamics.setClipperOutput(0.0f);
    dynamics.setClipperCurve(0);
    dynamics.setClipperOversampling(0);
    
    juce::AudioBuffer<float> buffer100(2, 512);
    juce::AudioBuffer<float> buffer50(2, 512);
    
    for (int ch = 0; ch < 2; ++ch)
    {
        for (int i = 0; i < 512; ++i)
        {
            buffer100.setSample(ch, i, 1.0f);
            buffer50.setSample(ch, i, 1.0f);
        }
    }
    
    // 100% wet
    dynamics.setClipperMix(100.0f);
    dynamics.process(buffer100, false, false, true);
    
    // 50% wet
    dynamics.reset();
    dynamics.setClipperMix(50.0f);
    dynamics.process(buffer50, false, false, true);
    
    float peak100 = std::abs(buffer100.getSample(0, 0));
    float peak50 = std::abs(buffer50.getSample(0, 0));
    
    std::cout << "Test: Parallel Mix - 100%: " << peak100 << ", 50%: " << peak50 << std::endl;
    assert(peak50 > peak100); // 50% mix should be louder (more dry signal)
}

void testOversamplingModes()
{
    MasterDynamics dynamics;
    dynamics.prepare(48000.0, 512);
    
    dynamics.setClipperDrive(12.0f);
    dynamics.setClipperOutput(0.0f);
    dynamics.setClipperMix(100.0f);
    dynamics.setClipperCurve(0);
    
    const char* osNames[] = {"Off", "2x", "4x"};
    
    for (int os = 0; os < 3; ++os)
    {
        dynamics.reset();
        dynamics.setClipperOversampling(os);
        
        juce::AudioBuffer<float> buffer(2, 512);
        for (int ch = 0; ch < 2; ++ch)
            for (int i = 0; i < 512; ++i)
                buffer.setSample(ch, i, 1.5f);
        
        dynamics.process(buffer, false, false, true);
        
        std::cout << "Test: Oversampling " << osNames[os] << " - Processed successfully" << std::endl;
    }
}

int main()
{
    std::cout << "=== Soft Clipper Unit Tests ===" << std::endl;
    
    testNoBoundedOutput();
    testCurveSelection();
    testParallelMix();
    testOversamplingModes();
    
    std::cout << "\n✓ All tests passed!" << std::endl;
    return 0;
}
