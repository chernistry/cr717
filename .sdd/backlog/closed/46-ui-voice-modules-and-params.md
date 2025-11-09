Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 46 — Voice Modules UI (BD/SD/CH/OH/CP) with Param Attachments

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§4.2 Voice modules, §5 Controls mapping)
- Best practices: `.sdd/best_practices_vst.md` (RT-safe UI, parameter attachments)
- Web model reference: `src/state/voiceParams.ts`

Goal
- Build premium voice cards for BD, SD, CH, OH, CP with knobs/faders/switches mapped to APVTS parameters, including accents and velocity behaviors.

Scope
- Per-voice panel: title, on/off, key params (e.g., BD: Tune, Decay, Attack, Click, Dist; SD: Tune, Snappy, Decay; Hats: Tone, Decay, Open/Closed link; Clap: Spread, Tone, Reverb send, etc.).
- Small/elevated visual touches: LED, subtle separators, tokenized spacing.
- Parameter attachments: `AudioProcessorValueTreeState::SliderAttachment`/`ButtonAttachment`.
- Keyboard focus order; tooltips/numeric entry; double-click reset; Shift for fine tune.

Acceptance Criteria
- All five voice modules render at recommended sizes and bind to APVTS.
- Fine adjustments and reset gestures work; tooltips show ranges/units.
- On/off state deactivates voice processing without tearing.
- No allocations in paint; 60 fps under knob scrubbing.

Tasks
1) Define per-voice param set (ensure parity with DSP tickets).
2) Implement `VoiceCard` base + concrete voice panels.
3) Attach APVTS params and gestures (double-click reset, Shift fine).
4) Add light visual tests/screenshots at 1200×680 and 1400×780.

References
- Spec §4.2/§5; JUCE attachments; coding rules.

