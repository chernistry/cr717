#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <array>

class Sequencer
{
public:
    static constexpr int NUM_STEPS = 16;
    static constexpr int NUM_VOICES = 12;
    
    struct Pattern
    {
        struct Step { bool on = false; bool accent = false; };
        std::array<std::array<Step, NUM_STEPS>, NUM_VOICES> steps{};
        
        void clear()
        {
            for (auto& voice : steps)
            {
                for (auto& s : voice)
                    s = {};
            }
        }
        
        void setStep(int voice, int step, bool active)
        {
            if (voice >= 0 && voice < NUM_VOICES && step >= 0 && step < NUM_STEPS)
                steps[voice][step].on = active;
        }
        
        void setStep(int voice, int step, bool active, bool isAccent)
        {
            if (voice >= 0 && voice < NUM_VOICES && step >= 0 && step < NUM_STEPS)
            {
                steps[voice][step].on = active;
                steps[voice][step].accent = isAccent && active;
            }
        }
        
        void setAccent(int voice, int step, bool isAccent)
        {
            if (voice >= 0 && voice < NUM_VOICES && step >= 0 && step < NUM_STEPS)
                steps[voice][step].accent = isAccent && steps[voice][step].on;
        }
        
        bool getStep(int voice, int step) const
        {
            if (voice >= 0 && voice < NUM_VOICES && step >= 0 && step < NUM_STEPS)
                return steps[voice][step].on;
            return false;
        }
        
        bool getAccent(int voice, int step) const
        {
            if (voice >= 0 && voice < NUM_VOICES && step >= 0 && step < NUM_STEPS)
                return steps[voice][step].accent;
            return false;
        }
    };
    
    Sequencer() { pattern.clear(); }
    
    void setStep(int voice, int step, bool active) { pattern.setStep(voice, step, active); }
    void setStep(int voice, int step, bool active, bool accent) { pattern.setStep(voice, step, active, accent); }
    void setAccent(int voice, int step, bool accent) { pattern.setAccent(voice, step, accent); }
    bool getStep(int voice, int step) const { return pattern.getStep(voice, step); }
    bool getAccent(int voice, int step) const { return pattern.getAccent(voice, step); }
    
    void setCurrentStep(int step) { currentStep = step % NUM_STEPS; }
    int getCurrentStep() const { return currentStep; }
    
    void setPlaying(bool shouldPlay) { isPlaying = shouldPlay; }
    bool getPlaying() const { return isPlaying; }
    void togglePlayback() { isPlaying = !isPlaying; }
    
    void setBPM(double bpm) { currentBPM = bpm; }
    double getBPM() const { return currentBPM; }
    
    const Pattern& getPattern() const { return pattern; }
    void setPattern(const Pattern& newPattern) { pattern = newPattern; }
    
    // Generate MIDI data for drag & drop
    juce::MidiFile generateMidiFile() const
    {
        juce::MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(960);
        
        juce::MidiMessageSequence track;
        
        // GM Drum Map
        const int noteMap[NUM_VOICES] = {36, 38, 41, 47, 50, 37, 39, 42, 46, 49, 51, 56};
        
        double ticksPerStep = 960.0 / 4.0; // 16th notes
        
        for (int voice = 0; voice < NUM_VOICES; ++voice)
        {
            for (int step = 0; step < NUM_STEPS; ++step)
            {
                if (pattern.getStep(voice, step))
                {
                    double timestamp = step * ticksPerStep;
                    int note = noteMap[voice];
                    
                    track.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)100), timestamp);
                    track.addEvent(juce::MidiMessage::noteOff(1, note), timestamp + ticksPerStep * 0.9);
                }
            }
        }
        
        track.updateMatchedPairs();
        midiFile.addTrack(track);
        
        return midiFile;
    }
    
private:
    Pattern pattern;
    int currentStep = 0;
    bool isPlaying = false;
    double currentBPM = 120.0;
};
