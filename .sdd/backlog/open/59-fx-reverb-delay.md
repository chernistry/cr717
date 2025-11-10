# 59 — FX: Reverb (Freeverb-like) and Delay (Tempo-Sync, Filters, Ping-Pong)

Read `.sdd/best_practices_vst.md` (§4) and native coding rules first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§2.1 Reverb, §2.2 Delay)
- Goal: Lightweight, musical reverb and delay with per-voice sends and master returns (JUCE/C++ implementation).

Dependencies
- Ticket 49 (UI Mixer & Sends) — existing spec for sends/returns
- Ticket 61 (parameter mapping) — to expose controls and persistence

Scope — Reverb
- Algorithm: Freeverb-like (8 comb + 4 allpass), stereo
- Room size 0–100% (default 50%), Damping 1–16 kHz (default 8 kHz)
- Pre-delay 0–100 ms (default 20 ms), Wet/Dry 0–100% (default 30%)
- Diffusion 0–100% (default 70%), Width 0–100% (default 100%)

Scope — Delay
- Time 1–2000 ms or BPM-synced (1/64..4 bars), default 375 ms
- Feedback 0–95% (oscillation ~98%), Wet/Dry 0–100% (default 25%)
- Feedback loop filters: HPF 20–1000 Hz, LPF 1 k–20 kHz
- Stereo modes: mono, ping-pong, stereo
- Optional modulation: 0.1–5 Hz, 0–10 ms

Acceptance Criteria
- Sends from voices reach FX returns; Wet/Dry behaves as expected
- Delay ping-pong alternates L/R; filters darken repeats musically
- Reverb tail respects damping and predelay; width control collapses to mono at 0%
- C++ unit tests render impulses and check decay/envelope behavior (offline render harness)

Implementation Steps
1) Files (native VST3):
   - `native/vst3/CR717/Source/dsp/fx/Reverb.h/.cpp` (Freeverb-like or `juce::dsp::Reverb` wrapper)
   - `native/vst3/CR717/Source/dsp/fx/Delay.h/.cpp` (`juce::dsp::DelayLine<float>` + filters)
   - `native/vst3/CR717/Source/processing/Sends.h/.cpp` (per-voice sends management)
2) Tempo sync helper: `native/vst3/CR717/Source/dsp/utils/Tempo.h/.cpp`
3) Wire returns into master chain pre-dynamics (or ADR if post)
4) Tests: `native/tests/unit/dsp/test_reverb.cpp`, `test_delay.cpp`

Affected Files
- `native/vst3/CR717/Source/dsp/fx/Reverb.*`, `Delay.*` (new)
- `native/vst3/CR717/Source/processing/Sends.*`, `MasterBus.*`
- `native/tests/unit/dsp/test_reverb.cpp`, `test_delay.cpp`

Risks & Mitigations
- CPU from stereo reverb: optimize comb/allpass counts; parameterize quality
- Feedback instability: clamp feedback < 0.98; denormal guards
