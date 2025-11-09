#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "BassDrumVoice.h"
#include "SnareDrumVoice.h"
#include "HiHatVoice.h"
#include "PercussionVoice.h"
#include "Parameters.h"

class TR808GarageProcessor : public juce::AudioProcessor
{
public:
    TR808GarageProcessor();
    ~TR808GarageProcessor() override = default;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }

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
    
    // Voice instances
    BassDrumVoice bassDrum;
    SnareDrumVoice snareDrum;
    ClosedHiHatVoice closedHat;
    OpenHiHatVoice openHat;
    ClapVoice clap;
    RimShotVoice rimShot;

    // Host info
    double hostBPM = 120.0;
    bool hostIsPlaying = false;
    
    int currentPreset = 0;

    void handleMidiMessage(const juce::MidiMessage& msg);
    Voice* getVoiceForNote(int noteNumber);
    void updateVoiceParameters();
    void loadPreset(int index);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TR808GarageProcessor)
};
