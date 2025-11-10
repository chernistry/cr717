#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BassDrumVoice.h"
#include "SnareDrumVoice.h"
#include "HiHatVoice.h"
#include "PercussionVoice.h"
#include "TomVoice.h"
#include "CymbalVoice.h"
#include "Parameters.h"
#include "Reverb.h"
#include "Delay.h"
#include "MasterDynamics.h"
#include "Preset.h"
#include "PatternRandomizer.h"
#include "Sequencer.h"

class CR717Processor : public juce::AudioProcessor
{
public:
    CR717Processor();
    ~CR717Processor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "Cherni CR-717"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 36; }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }
    PresetManager& getPresetManager() { return presetManager; }
    PatternRandomizer& getRandomizer() { return randomizer; }
    Sequencer& getSequencer() { return sequencer; }
    
    void loadPreset(const Preset& preset);
    void startSequencer();
    void stopSequencer();

    // Metering (UI thread safe queries)
    float getPeakLevel(int channel) const { return peakLevels[channel].load(); }
    float getRMSLevel(int channel) const { return rmsLevels[channel].load(); }
    bool isClipping() const { return clipping.load(); }

private:
    juce::AudioProcessorValueTreeState apvts;
    
    // 12 voices: BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB
    BassDrumVoice bassDrum;
    SnareDrumVoice snareDrum;
    LowTomVoice lowTom;
    MidTomVoice midTom;
    HighTomVoice highTom;
    RimShotVoice rimShot;
    ClapVoice clap;
    ClosedHiHatVoice closedHat;
    OpenHiHatVoice openHat;
    CymbalVoice cymbal;
    RideVoice ride;
    CowbellVoice cowbell;

    double hostBPM = 120.0;
    bool hostIsPlaying = false;
    int currentPreset = 0;
    
    // FX
    AlgorithmicReverb reverb;
    TempoSyncDelay delay;
    MasterDynamics masterDynamics;
    juce::AudioBuffer<float> reverbBuffer, delayBuffer, voiceBuffer;
    
    PresetManager presetManager;
    PatternRandomizer randomizer;
    Sequencer sequencer;
    
    double nextStepTime = 0.0;
    int samplesUntilNextStep = 0;

    // Metering state (atomic for cross-thread use)
    std::atomic<float> peakLevels[2] { 0.0f, 0.0f };
    std::atomic<float> rmsLevels[2]  { 0.0f, 0.0f };
    std::atomic<bool>  clipping { false };

    void handleMidiMessage(const juce::MidiMessage& msg);
    Voice* getVoiceForNote(int noteNumber);
    void updateVoiceParameters();
    void updateFXParameters();
    void loadPreset(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CR717Processor)
};
