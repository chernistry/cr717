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

class TR808GarageProcessor : public juce::AudioProcessor
{
public:
    TR808GarageProcessor();
    ~TR808GarageProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "TR-808 Garage"; }
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 8; }
    int getCurrentProgram() override { return currentPreset; }
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

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

    void handleMidiMessage(const juce::MidiMessage& msg);
    Voice* getVoiceForNote(int noteNumber);
    void updateVoiceParameters();
    void updateFXParameters();
    void loadPreset(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TR808GarageProcessor)
};
