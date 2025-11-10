#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace ParamIDs
{
    inline constexpr auto masterLevel = "masterLevel";
    
    // BD
    inline constexpr auto bdLevel = "bdLevel";
    inline constexpr auto bdTune = "bdTune";
    inline constexpr auto bdFineTune = "bdFineTune";
    inline constexpr auto bdDecay = "bdDecay";
    inline constexpr auto bdTone = "bdTone";
    inline constexpr auto bdPan = "bdPan";
    
    // SD
    inline constexpr auto sdLevel = "sdLevel";
    inline constexpr auto sdTune = "sdTune";
    inline constexpr auto sdFineTune = "sdFineTune";
    inline constexpr auto sdDecay = "sdDecay";
    inline constexpr auto sdSnappy = "sdSnappy";
    inline constexpr auto sdPan = "sdPan";
    
    // LT, MT, HT
    inline constexpr auto ltLevel = "ltLevel";
    inline constexpr auto ltTune = "ltTune";
    inline constexpr auto ltFineTune = "ltFineTune";
    inline constexpr auto ltDecay = "ltDecay";
    inline constexpr auto ltPan = "ltPan";
    
    inline constexpr auto mtLevel = "mtLevel";
    inline constexpr auto mtTune = "mtTune";
    inline constexpr auto mtFineTune = "mtFineTune";
    inline constexpr auto mtDecay = "mtDecay";
    inline constexpr auto mtPan = "mtPan";
    
    inline constexpr auto htLevel = "htLevel";
    inline constexpr auto htTune = "htTune";
    inline constexpr auto htFineTune = "htFineTune";
    inline constexpr auto htDecay = "htDecay";
    inline constexpr auto htPan = "htPan";
    
    // RS, CP
    inline constexpr auto rsLevel = "rsLevel";
    inline constexpr auto rsTune = "rsTune";
    inline constexpr auto rsPan = "rsPan";
    
    inline constexpr auto cpLevel = "cpLevel";
    inline constexpr auto cpTone = "cpTone";
    inline constexpr auto cpPan = "cpPan";
    
    // CH, OH
    inline constexpr auto chLevel = "chLevel";
    inline constexpr auto chTone = "chTone";
    inline constexpr auto chPan = "chPan";
    
    inline constexpr auto ohLevel = "ohLevel";
    inline constexpr auto ohDecay = "ohDecay";
    inline constexpr auto ohTone = "ohTone";
    inline constexpr auto ohPan = "ohPan";
    
    // CY, RD, CB
    inline constexpr auto cyLevel = "cyLevel";
    inline constexpr auto cyDecay = "cyDecay";
    inline constexpr auto cyTone = "cyTone";
    inline constexpr auto cyPan = "cyPan";
    
    inline constexpr auto rdLevel = "rdLevel";
    inline constexpr auto rdTone = "rdTone";
    inline constexpr auto rdPan = "rdPan";
    
    inline constexpr auto cbLevel = "cbLevel";
    inline constexpr auto cbTune = "cbTune";
    inline constexpr auto cbPan = "cbPan";
    
    // Hi-hat choke
    inline constexpr auto hhChoke = "hhChoke";
    
    // FX Sends (per voice)
    inline constexpr auto bdSendA = "bdSendA";
    inline constexpr auto bdSendB = "bdSendB";
    inline constexpr auto sdSendA = "sdSendA";
    inline constexpr auto sdSendB = "sdSendB";
    inline constexpr auto ltSendA = "ltSendA";
    inline constexpr auto ltSendB = "ltSendB";
    inline constexpr auto mtSendA = "mtSendA";
    inline constexpr auto mtSendB = "mtSendB";
    inline constexpr auto htSendA = "htSendA";
    inline constexpr auto htSendB = "htSendB";
    inline constexpr auto rsSendA = "rsSendA";
    inline constexpr auto rsSendB = "rsSendB";
    inline constexpr auto cpSendA = "cpSendA";
    inline constexpr auto cpSendB = "cpSendB";
    inline constexpr auto chSendA = "chSendA";
    inline constexpr auto chSendB = "chSendB";
    inline constexpr auto ohSendA = "ohSendA";
    inline constexpr auto ohSendB = "ohSendB";
    inline constexpr auto cySendA = "cySendA";
    inline constexpr auto cySendB = "cySendB";
    inline constexpr auto rdSendA = "rdSendA";
    inline constexpr auto rdSendB = "rdSendB";
    inline constexpr auto cbSendA = "cbSendA";
    inline constexpr auto cbSendB = "cbSendB";
    
    // Reverb
    inline constexpr auto reverbSize = "reverbSize";
    inline constexpr auto reverbDamp = "reverbDamp";
    inline constexpr auto reverbWidth = "reverbWidth";
    inline constexpr auto reverbWet = "reverbWet";
    
    // Delay
    inline constexpr auto delayTime = "delayTime";
    inline constexpr auto delayFeedback = "delayFeedback";
    inline constexpr auto delayWet = "delayWet";
    
    // Master Dynamics
    inline constexpr auto compThreshold = "compThreshold";
    inline constexpr auto compRatio = "compRatio";
    inline constexpr auto compAttack = "compAttack";
    inline constexpr auto compRelease = "compRelease";
    inline constexpr auto compKnee = "compKnee";
    inline constexpr auto compMakeup = "compMakeup";
    inline constexpr auto compAutoMakeup = "compAutoMakeup";
    inline constexpr auto compScHpf = "compScHpf";
    inline constexpr auto compDetector = "compDetector";
    inline constexpr auto compLookahead = "compLookahead";
    inline constexpr auto compEnabled = "compEnabled";
    inline constexpr auto limiterCeiling = "limiterCeiling";
    inline constexpr auto limiterRelease = "limiterRelease";
    inline constexpr auto limiterKnee = "limiterKnee";
    inline constexpr auto limiterLookahead = "limiterLookahead";
    inline constexpr auto limiterOversampling = "limiterOversampling";
    inline constexpr auto limiterEnabled = "limiterEnabled";
    inline constexpr auto clipperDrive = "clipperDrive";
    inline constexpr auto clipperOutput = "clipperOutput";
    inline constexpr auto clipperMix = "clipperMix";
    inline constexpr auto clipperCurve = "clipperCurve";
    inline constexpr auto clipperOversampling = "clipperOversampling";
    inline constexpr auto clipperEnabled = "clipperEnabled";
}

inline juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID{ParamIDs::masterLevel, 1}, "Master", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    
    // BD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdLevel, 1}, "BD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTune, 1}, "BD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdFineTune, 1}, "BD Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdDecay, 1}, "BD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdTone, 1}, "BD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdPan, 1}, "BD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // SD
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdLevel, 1}, "SD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdTune, 1}, "SD Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdFineTune, 1}, "SD Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdDecay, 1}, "SD Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdSnappy, 1}, "SD Snappy", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdPan, 1}, "SD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // LT, MT, HT
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltLevel, 1}, "LT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltTune, 1}, "LT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltFineTune, 1}, "LT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltDecay, 1}, "LT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltPan, 1}, "LT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtLevel, 1}, "MT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtTune, 1}, "MT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtFineTune, 1}, "MT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtDecay, 1}, "MT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtPan, 1}, "MT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htLevel, 1}, "HT Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htTune, 1}, "HT Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htFineTune, 1}, "HT Fine", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htDecay, 1}, "HT Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htPan, 1}, "HT Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // RS, CP
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsLevel, 1}, "RS Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsTune, 1}, "RS Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsPan, 1}, "RS Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpLevel, 1}, "CP Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpTone, 1}, "CP Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpPan, 1}, "CP Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // CH, OH
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chLevel, 1}, "CH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.6f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chTone, 1}, "CH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chPan, 1}, "CH Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohLevel, 1}, "OH Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohDecay, 1}, "OH Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohTone, 1}, "OH Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohPan, 1}, "OH Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // CY, RD, CB
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyLevel, 1}, "CY Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyDecay, 1}, "CY Decay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyTone, 1}, "CY Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cyPan, 1}, "CY Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdLevel, 1}, "RD Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdTone, 1}, "RD Tone", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdPan, 1}, "RD Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbLevel, 1}, "CB Level", juce::NormalisableRange<float>(0.0f, 1.0f), 0.7f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbTune, 1}, "CB Tune", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbPan, 1}, "CB Pan", juce::NormalisableRange<float>(-1.0f, 1.0f), 0.0f));
    
    // Hi-hat choke
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::hhChoke, 1}, "HH Choke", true));
    
    // FX Sends (all 12 voices)
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdSendA, 1}, "BD Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::bdSendB, 1}, "BD Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdSendA, 1}, "SD Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::sdSendB, 1}, "SD Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltSendA, 1}, "LT Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.15f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ltSendB, 1}, "LT Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtSendA, 1}, "MT Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.15f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::mtSendB, 1}, "MT Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htSendA, 1}, "HT Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.15f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::htSendB, 1}, "HT Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsSendA, 1}, "RS Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rsSendB, 1}, "RS Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpSendA, 1}, "CP Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.25f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cpSendB, 1}, "CP Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.05f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chSendA, 1}, "CH Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.05f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::chSendB, 1}, "CH Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohSendA, 1}, "OH Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::ohSendB, 1}, "OH Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cySendA, 1}, "CY Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cySendB, 1}, "CY Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdSendA, 1}, "RD Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.2f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::rdSendB, 1}, "RD Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbSendA, 1}, "CB Reverb", juce::NormalisableRange<float>(0.0f, 1.0f), 0.1f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::cbSendB, 1}, "CB Delay", juce::NormalisableRange<float>(0.0f, 1.0f), 0.0f));
    
    // Reverb
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::reverbSize, 1}, "Reverb Size", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::reverbDamp, 1}, "Reverb Damp", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::reverbWidth, 1}, "Reverb Width", juce::NormalisableRange<float>(0.0f, 1.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::reverbWet, 1}, "Reverb Wet", juce::NormalisableRange<float>(0.0f, 1.0f), 0.3f));
    
    // Delay
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::delayTime, 1}, "Delay Time", juce::NormalisableRange<float>(0.125f, 1.0f), 0.25f)); // 1/8 to 1 beat
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::delayFeedback, 1}, "Delay Feedback", juce::NormalisableRange<float>(0.0f, 0.9f), 0.4f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::delayWet, 1}, "Delay Wet", juce::NormalisableRange<float>(0.0f, 1.0f), 0.3f));
    
    // Master Dynamics
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compThreshold, 1}, "Comp Threshold", juce::NormalisableRange<float>(-40.0f, 0.0f), -12.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ParamIDs::compRatio, 1}, "Comp Ratio", juce::StringArray{"1:1", "2:1", "4:1", "8:1", "10:1", "20:1", "∞:1"}, 2));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compAttack, 1}, "Comp Attack", juce::NormalisableRange<float>(0.05f, 100.0f, 0.0f, 0.3f), 10.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compRelease, 1}, "Comp Release", juce::NormalisableRange<float>(10.0f, 2000.0f, 0.0f, 0.3f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compKnee, 1}, "Comp Knee", juce::NormalisableRange<float>(0.0f, 12.0f), 6.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compMakeup, 1}, "Comp Makeup", juce::NormalisableRange<float>(-12.0f, 24.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::compAutoMakeup, 1}, "Comp Auto Makeup", false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compScHpf, 1}, "Comp SC HPF", juce::NormalisableRange<float>(20.0f, 500.0f, 0.0f, 0.3f), 80.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::compDetector, 1}, "Comp RMS Mode", true));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::compLookahead, 1}, "Comp Lookahead", juce::NormalisableRange<float>(0.0f, 5.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::compEnabled, 1}, "Comp Enabled", true));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::limiterCeiling, 1}, "Limiter Ceiling", juce::NormalisableRange<float>(-0.3f, 0.0f), -0.3f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::limiterRelease, 1}, "Limiter Release", juce::NormalisableRange<float>(10.0f, 1000.0f, 0.0f, 0.3f), 50.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::limiterKnee, 1}, "Limiter Knee", juce::NormalisableRange<float>(0.0f, 3.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::limiterLookahead, 1}, "Limiter Lookahead", juce::NormalisableRange<float>(0.0f, 10.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::limiterOversampling, 1}, "Limiter Oversampling", true));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::limiterEnabled, 1}, "Limiter Enabled", true));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::clipperDrive, 1}, "Clipper Drive", juce::NormalisableRange<float>(0.0f, 24.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::clipperOutput, 1}, "Clipper Output", juce::NormalisableRange<float>(-12.0f, 12.0f), 0.0f));
    layout.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{ParamIDs::clipperMix, 1}, "Clipper Mix", juce::NormalisableRange<float>(0.0f, 100.0f), 100.0f));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ParamIDs::clipperCurve, 1}, "Clipper Curve", juce::StringArray{"Tanh", "Atan", "Poly"}, 0));
    layout.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{ParamIDs::clipperOversampling, 1}, "Clipper OS", juce::StringArray{"Off", "2x", "4x"}, 2));
    layout.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{ParamIDs::clipperEnabled, 1}, "Clipper Enabled", false));
    
    return layout;
}
