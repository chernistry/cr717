# 60 — Voices: Enhanced Parameters (Filters, Envelopes, Pan/Width)

Read native coding rules and VST best practices first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§3.1 Filter Parameters, §3.2 Pan & Stereo)
- Goal: Extend existing voices (BD, SD, CH, OH, CP) with calibrated filter/envelope parameters; add per-voice pan and stereo controls.

Dependencies
- Tickets 03–06 (base voices) — already implemented
- Ticket 61 (parameter mapping via APVTS & UI attachments)

Scope
- Filters per voice: LP/HP/BP 12/24 dB as appropriate; default cutoffs per voice
  - BD: LP around 80 Hz; SD: BP ~3 kHz; HH: HP ~8 kHz
- Resonance 0–100%; Envelope amount −100..+100%; Attack 0–100 ms; Decay 10–2000 ms
- Drive 0–24 dB (pre/post filter as ADR), key tracking 0–100% (default 0%)
- Pan −100..+100 (constant-power −3 dB), Stereo width 0–200% where applicable (OH/CP)
- Respect choke between CH/OH

Acceptance Criteria
- Each voice exposes new params with sensible defaults matching spec
- Filters/envelopes modulate timbre as expected without zipper noise
- Pan law constant-power; width collapses to mono at 0% and expands to 200%
- C++ unit tests render parameter sweeps per voice (offline buffer) and assert energy shift consistent with cutoff/resonance changes

Implementation Steps
1) Files (native VST3):
   - `native/vst3/CR717/Source/dsp/voices/common/Filter.h/.cpp`, `Envelope.h/.cpp`, `Pan.h/.cpp`, `StereoWidth.h/.cpp`
2) Update each voice in `native/vst3/CR717/Source/dsp/voices/` to insert filter and apply env amount
3) Parameter smoothing where needed (cutoff, resonance, drive) via SmoothedValue
4) Tests: `native/tests/unit/voices/*_params_tests.cpp`

Affected Files
- `native/vst3/CR717/Source/dsp/voices/common/*` (new)
- `native/vst3/CR717/Source/dsp/voices/*` (augment)
- `native/tests/unit/voices/*`

Risks & Mitigations
- CPU increase: reuse nodes per trigger; keep graph minimal; avoid allocations
- Stability: clamp resonance/Q to avoid self-oscillation unless intended
