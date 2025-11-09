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
    lowTom.prepare(sampleRate, samplesPerBlock);
    midTom.prepare(sampleRate, samplesPerBlock);
    highTom.prepare(sampleRate, samplesPerBlock);
    rimShot.prepare(sampleRate, samplesPerBlock);
    clap.prepare(sampleRate, samplesPerBlock);
    closedHat.prepare(sampleRate, samplesPerBlock);
    openHat.prepare(sampleRate, samplesPerBlock);
    cymbal.prepare(sampleRate, samplesPerBlock);
    ride.prepare(sampleRate, samplesPerBlock);
    cowbell.prepare(sampleRate, samplesPerBlock);
    
    // FX
    reverb.prepare(sampleRate, samplesPerBlock);
    delay.prepare(sampleRate, samplesPerBlock);
    masterDynamics.prepare(sampleRate, samplesPerBlock);
    
    reverbBuffer.setSize(2, samplesPerBlock);
    delayBuffer.setSize(2, samplesPerBlock);
    voiceBuffer.setSize(2, samplesPerBlock);
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
    reverbBuffer.clear();
    delayBuffer.clear();
    
    // Helper lambda to render voice with sends
    auto renderVoiceWithSends = [&](Voice& voice, const char* sendAID, const char* sendBID) {
        if (voice.isActive()) {
            voiceBuffer.clear();
            voice.renderNextBlock(voiceBuffer, 0, numSamples);
            
            float sendA = apvts.getRawParameterValue(sendAID)->load();
            float sendB = apvts.getRawParameterValue(sendBID)->load();
            
            // Add to main mix
            for (int ch = 0; ch < 2; ++ch) {
                buffer.addFrom(ch, 0, voiceBuffer, ch, 0, numSamples);
                reverbBuffer.addFrom(ch, 0, voiceBuffer, ch, 0, numSamples, sendA);
                delayBuffer.addFrom(ch, 0, voiceBuffer, ch, 0, numSamples, sendB);
            }
        }
    };
    
    renderVoiceWithSends(bassDrum, ParamIDs::bdSendA, ParamIDs::bdSendB);
    renderVoiceWithSends(snareDrum, ParamIDs::sdSendA, ParamIDs::sdSendB);
    renderVoiceWithSends(lowTom, ParamIDs::ltSendA, ParamIDs::ltSendB);
    renderVoiceWithSends(midTom, ParamIDs::mtSendA, ParamIDs::mtSendB);
    renderVoiceWithSends(highTom, ParamIDs::htSendA, ParamIDs::htSendB);
    renderVoiceWithSends(rimShot, ParamIDs::rsSendA, ParamIDs::rsSendB);
    renderVoiceWithSends(clap, ParamIDs::cpSendA, ParamIDs::cpSendB);
    renderVoiceWithSends(closedHat, ParamIDs::chSendA, ParamIDs::chSendB);
    renderVoiceWithSends(openHat, ParamIDs::ohSendA, ParamIDs::ohSendB);
    renderVoiceWithSends(cymbal, ParamIDs::cySendA, ParamIDs::cySendB);
    renderVoiceWithSends(ride, ParamIDs::rdSendA, ParamIDs::rdSendB);
    renderVoiceWithSends(cowbell, ParamIDs::cbSendA, ParamIDs::cbSendB);

    // Update FX parameters
    updateFXParameters();

    // Process FX buses
    reverb.process(reverbBuffer);
    delay.process(delayBuffer);
    
    // Mix FX returns
    float reverbWet = apvts.getRawParameterValue(ParamIDs::reverbWet)->load();
    float delayWet = apvts.getRawParameterValue(ParamIDs::delayWet)->load();
    
    for (int ch = 0; ch < 2; ++ch)
    {
        buffer.addFrom(ch, 0, reverbBuffer, ch, 0, numSamples, reverbWet);
        buffer.addFrom(ch, 0, delayBuffer, ch, 0, numSamples, delayWet);
    }

    // Master dynamics
    bool compEnabled = apvts.getRawParameterValue(ParamIDs::compEnabled)->load() > 0.5f;
    bool limiterEnabled = apvts.getRawParameterValue(ParamIDs::limiterEnabled)->load() > 0.5f;
    masterDynamics.process(buffer, compEnabled, limiterEnabled);

    // Apply master level
    float masterLevel = apvts.getRawParameterValue(ParamIDs::masterLevel)->load();
    buffer.applyGain(masterLevel);
}

void TR808GarageProcessor::updateVoiceParameters()
{
    // Bass Drum
    bassDrum.setLevel(apvts.getRawParameterValue(ParamIDs::bdLevel)->load());
    bassDrum.setTune(apvts.getRawParameterValue(ParamIDs::bdTune)->load());
    bassDrum.setFineTune(apvts.getRawParameterValue(ParamIDs::bdFineTune)->load());
    bassDrum.setDecay(apvts.getRawParameterValue(ParamIDs::bdDecay)->load());
    bassDrum.setTone(apvts.getRawParameterValue(ParamIDs::bdTone)->load());
    bassDrum.setPan(apvts.getRawParameterValue(ParamIDs::bdPan)->load());
    
    // Snare Drum
    snareDrum.setLevel(apvts.getRawParameterValue(ParamIDs::sdLevel)->load());
    snareDrum.setTune(apvts.getRawParameterValue(ParamIDs::sdTune)->load());
    snareDrum.setFineTune(apvts.getRawParameterValue(ParamIDs::sdFineTune)->load());
    snareDrum.setDecay(apvts.getRawParameterValue(ParamIDs::sdDecay)->load());
    snareDrum.setTone(apvts.getRawParameterValue(ParamIDs::sdSnappy)->load());
    snareDrum.setPan(apvts.getRawParameterValue(ParamIDs::sdPan)->load());
    
    // Toms
    lowTom.setLevel(apvts.getRawParameterValue(ParamIDs::ltLevel)->load());
    lowTom.setTune(apvts.getRawParameterValue(ParamIDs::ltTune)->load());
    lowTom.setFineTune(apvts.getRawParameterValue(ParamIDs::ltFineTune)->load());
    lowTom.setDecay(apvts.getRawParameterValue(ParamIDs::ltDecay)->load());
    lowTom.setPan(apvts.getRawParameterValue(ParamIDs::ltPan)->load());
    
    midTom.setLevel(apvts.getRawParameterValue(ParamIDs::mtLevel)->load());
    midTom.setTune(apvts.getRawParameterValue(ParamIDs::mtTune)->load());
    midTom.setFineTune(apvts.getRawParameterValue(ParamIDs::mtFineTune)->load());
    midTom.setDecay(apvts.getRawParameterValue(ParamIDs::mtDecay)->load());
    midTom.setPan(apvts.getRawParameterValue(ParamIDs::mtPan)->load());
    
    highTom.setLevel(apvts.getRawParameterValue(ParamIDs::htLevel)->load());
    highTom.setTune(apvts.getRawParameterValue(ParamIDs::htTune)->load());
    highTom.setFineTune(apvts.getRawParameterValue(ParamIDs::htFineTune)->load());
    highTom.setDecay(apvts.getRawParameterValue(ParamIDs::htDecay)->load());
    highTom.setPan(apvts.getRawParameterValue(ParamIDs::htPan)->load());
    
    // Closed Hi-Hat
    closedHat.setLevel(apvts.getRawParameterValue(ParamIDs::chLevel)->load());
    closedHat.setTone(apvts.getRawParameterValue(ParamIDs::chTone)->load());
    closedHat.setPan(apvts.getRawParameterValue(ParamIDs::chPan)->load());
    
    // Open Hi-Hat
    openHat.setLevel(apvts.getRawParameterValue(ParamIDs::ohLevel)->load());
    openHat.setDecay(apvts.getRawParameterValue(ParamIDs::ohDecay)->load());
    openHat.setTone(apvts.getRawParameterValue(ParamIDs::ohTone)->load());
    openHat.setPan(apvts.getRawParameterValue(ParamIDs::ohPan)->load());
    
    // Clap
    clap.setLevel(apvts.getRawParameterValue(ParamIDs::cpLevel)->load());
    clap.setTone(apvts.getRawParameterValue(ParamIDs::cpTone)->load());
    clap.setPan(apvts.getRawParameterValue(ParamIDs::cpPan)->load());
    
    // Rim Shot
    rimShot.setLevel(apvts.getRawParameterValue(ParamIDs::rsLevel)->load());
    rimShot.setTune(apvts.getRawParameterValue(ParamIDs::rsTune)->load());
    rimShot.setPan(apvts.getRawParameterValue(ParamIDs::rsPan)->load());
    
    // Cymbal
    cymbal.setLevel(apvts.getRawParameterValue(ParamIDs::cyLevel)->load());
    cymbal.setDecay(apvts.getRawParameterValue(ParamIDs::cyDecay)->load());
    cymbal.setTone(apvts.getRawParameterValue(ParamIDs::cyTone)->load());
    cymbal.setPan(apvts.getRawParameterValue(ParamIDs::cyPan)->load());
    
    // Ride
    ride.setLevel(apvts.getRawParameterValue(ParamIDs::rdLevel)->load());
    ride.setTone(apvts.getRawParameterValue(ParamIDs::rdTone)->load());
    ride.setPan(apvts.getRawParameterValue(ParamIDs::rdPan)->load());
    
    // Cowbell
    cowbell.setLevel(apvts.getRawParameterValue(ParamIDs::cbLevel)->load());
    cowbell.setTune(apvts.getRawParameterValue(ParamIDs::cbTune)->load());
    cowbell.setPan(apvts.getRawParameterValue(ParamIDs::cbPan)->load());
}

void TR808GarageProcessor::updateFXParameters()
{
    // Reverb
    reverb.setRoomSize(apvts.getRawParameterValue(ParamIDs::reverbSize)->load());
    reverb.setDamping(apvts.getRawParameterValue(ParamIDs::reverbDamp)->load());
    reverb.setWidth(apvts.getRawParameterValue(ParamIDs::reverbWidth)->load());
    reverb.setWetLevel(apvts.getRawParameterValue(ParamIDs::reverbWet)->load());
    
    // Delay
    float delayBeats = apvts.getRawParameterValue(ParamIDs::delayTime)->load();
    delay.setDelayTime(delayBeats, hostBPM);
    delay.setFeedback(apvts.getRawParameterValue(ParamIDs::delayFeedback)->load());
    delay.setWetLevel(apvts.getRawParameterValue(ParamIDs::delayWet)->load());
    
    // Master Dynamics
    masterDynamics.setCompressorThreshold(apvts.getRawParameterValue(ParamIDs::compThreshold)->load());
    masterDynamics.setCompressorRatio(apvts.getRawParameterValue(ParamIDs::compRatio)->load());
    masterDynamics.setLimiterThreshold(apvts.getRawParameterValue(ParamIDs::limiterThreshold)->load());
}

void TR808GarageProcessor::handleMidiMessage(const juce::MidiMessage& msg)
{
    int note = msg.getNoteNumber();
    float velocity = msg.getVelocity() / 127.0f;
    
    Voice* voice = getVoiceForNote(note);
    if (voice != nullptr)
    {
        // Hi-hat choke group
        bool chokeEnabled = apvts.getRawParameterValue(ParamIDs::hhChoke)->load() > 0.5f;
        if (chokeEnabled)
        {
            if (note == 42) // Closed hi-hat
                openHat.stop(); // Stop open hat when closed plays
            else if (note == 46) // Open hi-hat
                closedHat.stop(); // Stop closed hat when open plays
        }
        
        voice->trigger(velocity);
    }
}

Voice* TR808GarageProcessor::getVoiceForNote(int noteNumber)
{
    // GM Drum Map: BD=36, SD=38, LT=41, MT=47, HT=50, RS=37, CP=39, CH=42, OH=46, CY=49, RD=51, CB=56
    switch (noteNumber)
    {
        case 36: return &bassDrum;      // C1 - Bass Drum
        case 38: return &snareDrum;     // D1 - Snare Drum
        case 41: return &lowTom;        // F1 - Low Tom
        case 47: return &midTom;        // B1 - Mid Tom
        case 50: return &highTom;       // D2 - High Tom
        case 37: return &rimShot;       // C#1 - Rim Shot
        case 39: return &clap;          // D#1 - Hand Clap
        case 42: return &closedHat;     // F#1 - Closed Hi-Hat
        case 46: return &openHat;       // A#1 - Open Hi-Hat
        case 49: return &cymbal;        // C#2 - Crash Cymbal
        case 51: return &ride;          // D#2 - Ride Cymbal
        case 56: return &cowbell;       // G#2 - Cowbell
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
