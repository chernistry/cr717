Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# VST3 Voice Parity & Extended Parameters (All 12 Voices, Pro Controls)

Objective
- Ensure the native plugin fully matches the Web instrument feature set and expands where modern VSTs expect more control. Implement all 12 TR‑808 voices with pro‑grade parameter sets, choke groups, pan, and per‑voice output options.

Acceptance Criteria (DoD)
- All 12 voices implemented in DSP: BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB.
- Voice controls (minimum):
  - BD: Level, Tune (coarse), FineTune, Decay, Tone
  - SD: Level, Tune, FineTune, Decay, Tone, Snappy
  - Toms (LT/MT/HT): Level, Tune, FineTune, Decay
  - RS/CP/CB: Level
  - CH/OH: Level, Decay, (optional) Tone
  - CY/RD: Level, Tone, Decay
- Global per‑voice Pan; optional per‑voice Output (Main/Alt out 1–4) for DAW routing.
- Hi‑hat choke group (CH and OH): OH cuts when CH plays (adjustable choke behavior on/off).
- Parameter ranges sane and smoothed; no zipper noise; zero allocations in audio thread.
- MIDI mapping documented (note → voice), velocity mapping to amplitude.

Steps
1) DSP voices completion
   - Implement remaining voices (CY, RD, CB, toms) matching 808 character (use sample‑based MVP here; plan analog model optional later).
2) Parameter set extension
   - Add FineTune to tuned voices; add Pan and Output routing to all voices; wire through APVTS.
3) Choke group
   - Implement CH→OH choke with toggle; consider OH→CH when OH long decay.
4) UI
   - Update Voice Params panel (Web and VST GUI) to expose new params; arrange with consistent grouping.
5) QA
   - Verify no clicks on param change; buffer size changes safe; CPU ≤ 3% @ 44.1k / 64–128 samples.

Affected Files
- native/vst3/TR808Garage/Source/{Voices, PluginProcessor, Params, Routing}.*
- native/vst3/TR808Garage/Source/ui/VoiceParams.*

Risks & Mitigations
- Too many params clutter UI → group & collapse advanced controls; provide sensible defaults.
- DAW routing differences → document multi‑out setup; keep Main default.

Dependencies
- 31 (DSP core), 32 (parameters), 33/35 (GUI framework & design system).

