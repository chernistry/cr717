Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 49 — Mixer: Per-Voice Level/Pan/Sends, FX Returns, Master Bus

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§4.3 Mixer/FX, §6 Master meter)
- Best practices: `.sdd/best_practices_vst.md` (no heavy work in paint, lock-free)

Goal
- Implement right-side mixer with per-voice level/pan/send controls, FX section (Reverb/Delay returns), and master bus with meters.

Scope
- Per-voice: Level (fader), Pan, Send A/B (reverb/delay), Mute/Solo.
- FX: Reverb (Size, Decay, Mix), Delay (Time, Feedback, Mix, Sync toggle). Tokenized UI.
- Master: Output gain, Peak/RMS/True-Peak meter (see Ticket 48), soft clip indicator.
- Signal flow visuals indicating sends and master chain.

Acceptance Criteria
- Mixer reflects live parameter changes; Mute/Solo logic correct across voices.
- FX params are host-automatable and reflected in UI without zipper noise.
- Master meter behaves per Ticket 48; master output matches gain structure without clipping unless desired.

Tasks
1) Implement `MixerPanel` with per-voice strips.
2) Implement `FxPanel` with Reverb/Delay; link to DSP.
3) Add Mute/Solo logic (UI + audio engine hook).
4) Integrate master meters from Ticket 48; add output gain.

References
- Spec §4.3/§6; coding rules; DSP tickets for parity.

