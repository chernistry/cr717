Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# VST3 Parameters, MIDI, Host Sync, State & Presets

Objective
- Expose parameters for automation, support MIDI input mapping to voices, apply host tempo info (for future internal patterns), and implement state save/restore + factory presets reflecting banks A–H.

Acceptance Criteria (DoD)
- Parameters defined via `AudioProcessorValueTreeState` (or equivalent) with IDs and ranges: per-voice Level, Tune, Decay, Tone, Snappy/HH Decay; global Master.
- MIDI: Note→Voice map documented; velocity scales amplitude; pitch bend → optional fine tune (global).
- Host sync available in processor (BPM, PPQ, playing state) and plumbed to future sequencer module (can be unused now).
- State: full parameter state and current preset serialized and restored; compatible across sample rates/buffer sizes.
- Factory presets: 8 presets approximating web banks A–H (names aligned).

Steps
1) Parameter schema
   - Create ParamIDs; min/max/defaults; skew where needed. Add smoothing.
2) APVTS wiring
   - Attach parameters; processor getters; value listeners.
3) MIDI handling
   - Implement noteOn/off routing to voices. Velocity and optional PB. Handle sustain pedal (ignore for drum kit by default).
4) Host info
   - Read `AudioPlayHead` for BPM/position; store in state (for future use).
5) State & presets
   - Implement `getStateInformation`/`setStateInformation`; add 8 factory presets.

Affected Files
- `native/vst3/TR808Garage/Source/PluginProcessor.*`
- `native/vst3/TR808Garage/Source/Params.*`
- `native/vst3/TR808Garage/Source/Presets.*`

Risks & Mitigations
- Parameter spam (UI) → throttle GUI updates; use async flags.
- Preset portability → keep preset schema stable; version tag in state blob.

Dependencies
- Ticket 31 (DSP core).

