#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    // Master
    inline constexpr auto masterLevel = "masterLevel";
    
    // Bass Drum
    inline constexpr auto bdLevel = "bdLevel";
    inline constexpr auto bdTune = "bdTune";
    inline constexpr auto bdDecay = "bdDecay";
    inline constexpr auto bdTone = "bdTone";
    
    // Snare Drum
    inline constexpr auto sdLevel = "sdLevel";
    inline constexpr auto sdTune = "sdTune";
    inline constexpr auto sdDecay = "sdDecay";
    inline constexpr auto sdSnappy = "sdSnappy";
    
    // Closed Hi-Hat
    inline constexpr auto chLevel = "chLevel";
    inline constexpr auto chTone = "chTone";
    
    // Open Hi-Hat
    inline constexpr auto ohLevel = "ohLevel";
    inline constexpr auto ohDecay = "ohDecay";
    inline constexpr auto ohTone = "ohTone";
    
    // Clap
    inline constexpr auto cpLevel = "cpLevel";
    inline constexpr auto cpTone = "cpTone";
    
    // Rim Shot
    inline constexpr auto rsLevel = "rsLevel";
    inline constexpr auto rsTune = "rsTune";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    // Master
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::masterLevel, 1}, "Master Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    
    // Bass Drum
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bdLevel, 1}, "BD Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bdTune, 1}, "BD Tune",
        juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bdDecay, 1}, "BD Decay",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::bdTone, 1}, "BD Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // Snare Drum
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sdLevel, 1}, "SD Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sdTune, 1}, "SD Tune",
        juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sdDecay, 1}, "SD Decay",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::sdSnappy, 1}, "SD Snappy",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // Closed Hi-Hat
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::chLevel, 1}, "CH Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::chTone, 1}, "CH Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // Open Hi-Hat
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::ohLevel, 1}, "OH Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::ohDecay, 1}, "OH Decay",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::ohTone, 1}, "OH Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // Clap
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::cpLevel, 1}, "CP Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::cpTone, 1}, "CP Tone",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    
    // Rim Shot
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::rsLevel, 1}, "RS Level",
        juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::rsTune, 1}, "RS Tune",
        juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    
    return layout;
}
