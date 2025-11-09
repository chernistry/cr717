#include "PluginProcessor.h"
#include "PluginEditor.h"

// MIDI note mapping (GM Drum Map compatible)
// C1 (36) = BD, D1 (38) = SD, F#1 (42) = CH, A#1 (46) = OH, D#1 (39) = CP, C#1 (37) = RS

TR808GarageProcessor::TR808GarageProcessor()
    : AudioProcessor(BusesProperties()
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
}

void TR808GarageProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    setLatencySamples(0);
    
    bassDrum.prepare(sampleRate, samplesPerBlock);
    snareDrum.prepare(sampleRate, samplesPerBlock);
    closedHat.prepare(sampleRate, samplesPerBlock);
    openHat.prepare(sampleRate, samplesPerBlock);
    clap.prepare(sampleRate, samplesPerBlock);
    rimShot.prepare(sampleRate, samplesPerBlock);
}

void TR808GarageProcessor::releaseResources()
{
}

juce::AudioProcessorEditor* TR808GarageProcessor::createEditor()
{
    return new TR808GarageEditor(*this);
}

bool TR808GarageProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void TR808GarageProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;

    buffer.clear();

    // Update host info
    if (auto* playHead = getPlayHead())
    {
        if (auto pos = playHead->getPosition())
        {
            if (pos->getBpm())
                hostBPM = *pos->getBpm();
            hostIsPlaying = pos->getIsPlaying();
        }
    }

    // Update voice parameters from APVTS
    updateVoiceParameters();

    // Process MIDI events
    for (const auto metadata : midiMessages)
    {
        const auto msg = metadata.getMessage();
        if (msg.isNoteOn())
            handleMidiMessage(msg);
    }

    // Render all active voices
    int numSamples = buffer.getNumSamples();
    
    if (bassDrum.isActive())
        bassDrum.renderNextBlock(buffer, 0, numSamples);
    
    if (snareDrum.isActive())
        snareDrum.renderNextBlock(buffer, 0, numSamples);
    
    if (closedHat.isActive())
        closedHat.renderNextBlock(buffer, 0, numSamples);
    
    if (openHat.isActive())
        openHat.renderNextBlock(buffer, 0, numSamples);
    
    if (clap.isActive())
        clap.renderNextBlock(buffer, 0, numSamples);
    
    if (rimShot.isActive())
        rimShot.renderNextBlock(buffer, 0, numSamples);

    // Apply master level
    float masterLevel = apvts.getRawParameterValue(ParamIDs::masterLevel)->load();
    buffer.applyGain(masterLevel);
}

void TR808GarageProcessor::updateVoiceParameters()
{
    // Bass Drum
    bassDrum.setLevel(apvts.getRawParameterValue(ParamIDs::bdLevel)->load());
    bassDrum.setTune(apvts.getRawParameterValue(ParamIDs::bdTune)->load());
    bassDrum.setDecay(apvts.getRawParameterValue(ParamIDs::bdDecay)->load());
    bassDrum.setTone(apvts.getRawParameterValue(ParamIDs::bdTone)->load());
    
    // Snare Drum
    snareDrum.setLevel(apvts.getRawParameterValue(ParamIDs::sdLevel)->load());
    snareDrum.setTune(apvts.getRawParameterValue(ParamIDs::sdTune)->load());
    snareDrum.setDecay(apvts.getRawParameterValue(ParamIDs::sdDecay)->load());
    snareDrum.setTone(apvts.getRawParameterValue(ParamIDs::sdSnappy)->load());
    
    // Closed Hi-Hat
    closedHat.setLevel(apvts.getRawParameterValue(ParamIDs::chLevel)->load());
    closedHat.setTone(apvts.getRawParameterValue(ParamIDs::chTone)->load());
    
    // Open Hi-Hat
    openHat.setLevel(apvts.getRawParameterValue(ParamIDs::ohLevel)->load());
    openHat.setDecay(apvts.getRawParameterValue(ParamIDs::ohDecay)->load());
    openHat.setTone(apvts.getRawParameterValue(ParamIDs::ohTone)->load());
    
    // Clap
    clap.setLevel(apvts.getRawParameterValue(ParamIDs::cpLevel)->load());
    clap.setTone(apvts.getRawParameterValue(ParamIDs::cpTone)->load());
    
    // Rim Shot
    rimShot.setLevel(apvts.getRawParameterValue(ParamIDs::rsLevel)->load());
    rimShot.setTune(apvts.getRawParameterValue(ParamIDs::rsTune)->load());
}

void TR808GarageProcessor::handleMidiMessage(const juce::MidiMessage& msg)
{
    int note = msg.getNoteNumber();
    float velocity = msg.getVelocity() / 127.0f;
    
    Voice* voice = getVoiceForNote(note);
    if (voice != nullptr)
        voice->trigger(velocity);
}

Voice* TR808GarageProcessor::getVoiceForNote(int noteNumber)
{
    switch (noteNumber)
    {
        case 36: return &bassDrum;      // C1 - Bass Drum
        case 38: return &snareDrum;     // D1 - Snare Drum
        case 42: return &closedHat;     // F#1 - Closed Hi-Hat
        case 46: return &openHat;       // A#1 - Open Hi-Hat
        case 39: return &clap;          // D#1 - Hand Clap
        case 37: return &rimShot;       // C#1 - Rim Shot
        default: return nullptr;
    }
}

void TR808GarageProcessor::setCurrentProgram(int index)
{
    if (index >= 0 && index < 8)
    {
        currentPreset = index;
        loadPreset(index);
    }
}

const juce::String TR808GarageProcessor::getProgramName(int index)
{
    const char* names[] = {"Pattern A", "Pattern B", "Pattern C", "Pattern D",
                           "Pattern E", "Pattern F", "Pattern G", "Pattern H"};
    return (index >= 0 && index < 8) ? names[index] : "";
}

void TR808GarageProcessor::loadPreset(int index)
{
    // Factory presets (aligned with web version patterns A-H)
    struct PresetData {
        float bdLevel, bdTune, bdDecay, bdTone;
        float sdLevel, sdTune, sdDecay, sdSnappy;
        float chLevel, chTone;
        float ohLevel, ohDecay, ohTone;
        float cpLevel, cpTone;
        float rsLevel, rsTune;
        float master;
    };
    
    const PresetData presets[] = {
        // A: UK Garage (default)
        {0.8f, 0.0f, 0.5f, 0.5f,  0.8f, 0.0f, 0.5f, 0.5f,  0.6f, 0.5f,  0.5f, 0.5f, 0.5f,  0.7f, 0.5f,  0.7f, 0.0f,  0.8f},
        // B: Deep House
        {0.9f, -2.0f, 0.7f, 0.3f,  0.7f, 0.0f, 0.4f, 0.3f,  0.5f, 0.6f,  0.4f, 0.6f, 0.6f,  0.5f, 0.4f,  0.6f, 0.0f,  0.75f},
        // C: Techno
        {1.0f, 0.0f, 0.3f, 0.7f,  0.9f, 2.0f, 0.3f, 0.7f,  0.7f, 0.7f,  0.3f, 0.3f, 0.7f,  0.8f, 0.6f,  0.8f, 3.0f,  0.85f},
        // D: Hip-Hop
        {0.95f, -3.0f, 0.6f, 0.4f,  0.85f, 0.0f, 0.6f, 0.6f,  0.4f, 0.4f,  0.3f, 0.7f, 0.4f,  0.9f, 0.5f,  0.75f, 0.0f,  0.8f},
        // E: Minimal
        {0.7f, 0.0f, 0.4f, 0.6f,  0.6f, 0.0f, 0.4f, 0.4f,  0.8f, 0.8f,  0.2f, 0.4f, 0.8f,  0.4f, 0.3f,  0.5f, 0.0f,  0.7f},
        // F: Breakbeat
        {0.85f, 0.0f, 0.5f, 0.5f,  0.9f, 1.0f, 0.5f, 0.8f,  0.6f, 0.5f,  0.6f, 0.5f, 0.5f,  0.8f, 0.6f,  0.7f, 2.0f,  0.8f},
        // G: Ambient
        {0.6f, 0.0f, 0.8f, 0.3f,  0.5f, 0.0f, 0.7f, 0.3f,  0.3f, 0.3f,  0.7f, 0.8f, 0.3f,  0.3f, 0.2f,  0.4f, 0.0f,  0.65f},
        // H: Aggressive
        {1.0f, 3.0f, 0.4f, 0.8f,  1.0f, 4.0f, 0.4f, 0.9f,  0.8f, 0.9f,  0.4f, 0.4f, 0.9f,  1.0f, 0.8f,  0.9f, 5.0f,  0.9f}
    };
    
    if (index < 0 || index >= 8) return;
    
    const auto& preset = presets[index];
    
    apvts.getParameter(ParamIDs::bdLevel)->setValueNotifyingHost(preset.bdLevel);
    apvts.getParameter(ParamIDs::bdTune)->setValueNotifyingHost((preset.bdTune + 12.0f) / 24.0f);
    apvts.getParameter(ParamIDs::bdDecay)->setValueNotifyingHost(preset.bdDecay);
    apvts.getParameter(ParamIDs::bdTone)->setValueNotifyingHost(preset.bdTone);
    
    apvts.getParameter(ParamIDs::sdLevel)->setValueNotifyingHost(preset.sdLevel);
    apvts.getParameter(ParamIDs::sdTune)->setValueNotifyingHost((preset.sdTune + 12.0f) / 24.0f);
    apvts.getParameter(ParamIDs::sdDecay)->setValueNotifyingHost(preset.sdDecay);
    apvts.getParameter(ParamIDs::sdSnappy)->setValueNotifyingHost(preset.sdSnappy);
    
    apvts.getParameter(ParamIDs::chLevel)->setValueNotifyingHost(preset.chLevel);
    apvts.getParameter(ParamIDs::chTone)->setValueNotifyingHost(preset.chTone);
    
    apvts.getParameter(ParamIDs::ohLevel)->setValueNotifyingHost(preset.ohLevel);
    apvts.getParameter(ParamIDs::ohDecay)->setValueNotifyingHost(preset.ohDecay);
    apvts.getParameter(ParamIDs::ohTone)->setValueNotifyingHost(preset.ohTone);
    
    apvts.getParameter(ParamIDs::cpLevel)->setValueNotifyingHost(preset.cpLevel);
    apvts.getParameter(ParamIDs::cpTone)->setValueNotifyingHost(preset.cpTone);
    
    apvts.getParameter(ParamIDs::rsLevel)->setValueNotifyingHost(preset.rsLevel);
    apvts.getParameter(ParamIDs::rsTune)->setValueNotifyingHost((preset.rsTune + 12.0f) / 24.0f);
    
    apvts.getParameter(ParamIDs::masterLevel)->setValueNotifyingHost(preset.master);
}

void TR808GarageProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void TR808GarageProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TR808GarageProcessor();
}
