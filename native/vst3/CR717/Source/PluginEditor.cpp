/*
  Cherni CR-717 Plugin Editor - Professional UI
  New UI integration with 12-column grid, sequencer, voice strips, and master panel
*/

#include "PluginEditor.h"

CR717Editor::CR717Editor(CR717Processor& p)
    : AudioProcessorEditor(&p)
    , processor(p)
{
    setLookAndFeel(&lookAndFeel);

    // Size and resize limits
    setSize(1440, 900);
    setResizable(true, true);
    setResizeLimits(1100, 720, 1680, 980);

    // Grid layout
    gridLayout = std::make_unique<GridLayout>(getWidth(), getHeight());

    // Header
    header = std::make_unique<HeaderPanel>();
    header->onPlayStop = [this]
    {
        if (processor.getSequencer().getPlaying()) processor.stopSequencer();
        else processor.startSequencer();
    };
    header->onBPMChange = [this](double bpm)
    {
        processor.getSequencer().setBPM(bpm);
    };
    header->onPresetChange = [this](int index)
    {
        handlePresetChange(index);
    };
    header->onPatternChange = [this](int bank)
    {
        // Map A-H to first 8 presets for now (pattern banks)
        handlePresetChange(bank);
    };
    addAndMakeVisible(header.get());

    // Populate presets in header
    {
        const auto& presets = processor.getPresetManager().getPresets();
        juce::StringArray displayNames;
        for (const auto& pr : presets)
            displayNames.add(pr.style + " - " + pr.name);
        header->setPresetList(displayNames);
        header->setSelectedPresetIndex(processor.getCurrentProgram());
        header->setBPM(processor.getSequencer().getBPM());
    }

    // Sequencer
    sequencer = std::make_unique<PadGrid>();
    sequencer->onPadStateChanged = [this](int step, int voice, StepPad::State state)
    {
        savePadToProcessor(step, voice, state);
    };
    sequencer->onRequestPadState = [this](int step, int voice) -> StepPad::State
    {
        bool active = processor.getSequencer().getStep(voice, step);
        bool accent = processor.getSequencer().getAccent(voice, step);
        return accent ? StepPad::State::Accent : (active ? StepPad::State::On : StepPad::State::Off);
    };
    addAndMakeVisible(sequencer.get());

    // Voice strips
    voiceStrips = std::make_unique<VoiceStripPanel>();
    voiceStrips->onVoiceMuteChanged = [this](int, bool){};
    voiceStrips->onVoiceSoloChanged = [this](int, bool){};
    addAndMakeVisible(voiceStrips.get());

    // Attach voice parameters to strips
    auto& apvts = processor.getAPVTS();
    // BD
    voiceStrips->getStrip(0)->attachToParameters(apvts, ParamIDs::bdLevel, ParamIDs::bdPan, ParamIDs::bdDecay, ParamIDs::bdTone);
    // SD
    voiceStrips->getStrip(1)->attachToParameters(apvts, ParamIDs::sdLevel, ParamIDs::sdPan, ParamIDs::sdDecay, ParamIDs::sdSnappy);
    // LT, MT, HT
    voiceStrips->getStrip(2)->attachToParameters(apvts, ParamIDs::ltLevel, ParamIDs::ltPan, ParamIDs::ltDecay);
    voiceStrips->getStrip(3)->attachToParameters(apvts, ParamIDs::mtLevel, ParamIDs::mtPan, ParamIDs::mtDecay);
    voiceStrips->getStrip(4)->attachToParameters(apvts, ParamIDs::htLevel, ParamIDs::htPan, ParamIDs::htDecay);
    // RS
    voiceStrips->getStrip(5)->attachToParameters(apvts, ParamIDs::rsLevel, ParamIDs::rsPan);
    // CP
    voiceStrips->getStrip(6)->attachToParameters(apvts, ParamIDs::cpLevel, ParamIDs::cpPan, {}, ParamIDs::cpTone);
    // CH
    voiceStrips->getStrip(7)->attachToParameters(apvts, ParamIDs::chLevel, ParamIDs::chPan, {}, ParamIDs::chTone);
    // OH
    voiceStrips->getStrip(8)->attachToParameters(apvts, ParamIDs::ohLevel, ParamIDs::ohPan, ParamIDs::ohDecay, ParamIDs::ohTone);
    // CY
    voiceStrips->getStrip(9)->attachToParameters(apvts, ParamIDs::cyLevel, ParamIDs::cyPan, ParamIDs::cyDecay, ParamIDs::cyTone);
    // RD
    voiceStrips->getStrip(10)->attachToParameters(apvts, ParamIDs::rdLevel, ParamIDs::rdPan, {}, ParamIDs::rdTone);
    // CB
    voiceStrips->getStrip(11)->attachToParameters(apvts, ParamIDs::cbLevel, ParamIDs::cbPan);

    // Master panel
    masterPanel = std::make_unique<MasterPanel>();
    addAndMakeVisible(masterPanel.get());

    // Attach master + FX params
    masterPanel->getOutputGain().setRange(0.0, 1.0, 0.001);
    masterLevelAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::masterLevel, masterPanel->getOutputGain());
    // Reverb
    reverbSizeAttachment     = std::make_unique<SliderAttachment>(apvts, ParamIDs::reverbSize,     masterPanel->getReverbSize());
    reverbDampAttachment     = std::make_unique<SliderAttachment>(apvts, ParamIDs::reverbDamp,     masterPanel->getReverbDamp());
    reverbWetAttachment      = std::make_unique<SliderAttachment>(apvts, ParamIDs::reverbWet,      masterPanel->getReverbMix());
    reverbPreDelayAttachment = std::make_unique<SliderAttachment>(apvts, ParamIDs::reverbPreDelay, masterPanel->getReverbPreDelay());
    reverbDiffusionAttachment= std::make_unique<SliderAttachment>(apvts, ParamIDs::reverbDiffusion,masterPanel->getReverbDiffusion());
    // Delay
    delayTimeAttachment      = std::make_unique<SliderAttachment>(apvts, ParamIDs::delayTime,      masterPanel->getDelayTime());
    delayFeedbackAttachment  = std::make_unique<SliderAttachment>(apvts, ParamIDs::delayFeedback,  masterPanel->getDelayFeedback());
    delayWetAttachment       = std::make_unique<SliderAttachment>(apvts, ParamIDs::delayWet,       masterPanel->getDelayMix());
    delayModeAttachment      = std::make_unique<ComboBoxAttachment>(apvts, ParamIDs::delayStereoMode, masterPanel->getDelayMode());
    delayModRateAttachment   = std::make_unique<SliderAttachment>(apvts, ParamIDs::delayModRate,   masterPanel->getDelayModRate());
    delayModDepthAttachment  = std::make_unique<SliderAttachment>(apvts, ParamIDs::delayModDepth,  masterPanel->getDelayModDepth());

    // Load initial pattern into PadGrid
    loadPatternFromProcessor();

    // Start timer (60 fps for playhead; meters run at ~30Hz internally)
    startTimer(16);
}

CR717Editor::~CR717Editor()
{
    setLookAndFeel(nullptr);
    stopTimer();
}

void CR717Editor::paint(juce::Graphics& g)
{
    using namespace DesignTokens;
    auto bounds = getLocalBounds().toFloat();
    // Subtle radial gradient background for a modern look
    juce::ColourGradient bgGrad(
        Colors::bgPrimary, bounds.getCentreX(), bounds.getCentreY(),
        Colors::bgSecondary.darker(0.2f), bounds.getWidth(), bounds.getBottom(),
        true
    );
    g.setGradientFill(bgGrad);
    g.fillAll();
    
    // Vignette
    g.setColour(juce::Colours::black.withAlpha(0.08f));
    g.drawRect(getLocalBounds());
}

void CR717Editor::resized()
{
    if (!gridLayout) return;
    gridLayout->resize(getWidth(), getHeight());
    auto zones = gridLayout->getZones();
    header->setBounds(zones.header);
    sequencer->setBounds(zones.kit);
    voiceStrips->setBounds(zones.voiceArea);
    masterPanel->setBounds(zones.masterPanel);
}

void CR717Editor::timerCallback()
{
    updatePlayhead();
    updateMeters();
}

void CR717Editor::updatePlayhead()
{
    auto& seq = processor.getSequencer();
    sequencer->setPlaying(seq.getPlaying());
    if (seq.getPlaying())
        sequencer->setCurrentStep(seq.getCurrentStep());
}

void CR717Editor::updateMeters()
{
    auto& meters = masterPanel->getMeters();
    meters.setPeakLevel(0, processor.getPeakLevel(0));
    meters.setPeakLevel(1, processor.getPeakLevel(1));
    meters.setRMSLevel(0, processor.getRMSLevel(0));
    meters.setRMSLevel(1, processor.getRMSLevel(1));
    masterPanel->setClipping(processor.isClipping());
}

void CR717Editor::loadPatternFromProcessor()
{
    auto& seq = processor.getSequencer();
    for (int voice = 0; voice < 12; ++voice)
    {
        for (int step = 0; step < 16; ++step)
        {
            bool active = seq.getStep(voice, step);
            bool accent = seq.getAccent(voice, step);
            sequencer->setPadState(step, voice,
                accent ? StepPad::State::Accent : (active ? StepPad::State::On : StepPad::State::Off));
        }
    }
}

void CR717Editor::savePadToProcessor(int step, int voice, StepPad::State state)
{
    bool active = (state != StepPad::State::Off);
    bool accent = (state == StepPad::State::Accent);
    auto& seq = processor.getSequencer();
    seq.setStep(voice, step, active);
    seq.setAccent(voice, step, accent);
}

void CR717Editor::handlePresetChange(int index)
{
    if (index < 0) return;
    processor.setCurrentProgram(index);
    // Sync header and UI
    header->setSelectedPresetIndex(processor.getCurrentProgram());
    header->setBPM(processor.getSequencer().getBPM());
    loadPatternFromProcessor();
}
