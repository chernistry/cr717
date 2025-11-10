# 57 — True-Peak Limiter with Oversampling (JUCE/VST3)

Read `.sdd/best_practices_vst.md` (§4 Anti-aliasing) and native coding rules first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§1.2 Limiter)
- Goal: Lookahead limiter with true-peak detection (oversampled), clean brick-wall behavior, minimal artifacts (JUCE/C++).

Dependencies
- Ticket 56 (compressor) — order in master chain: Compressor → Limiter
- Ticket 62 (calibration & tests) — for meter alignment and ppk checks

Scope
- Peak limiter with lookahead 0–10 ms (default 5 ms)
- True-peak detection via 4× oversampling (linear-phase FIR upsampler)
- Threshold −20..0 dBFS, Ceiling −0.3..0 dBFS, Knee 0–3 dB
- Release 10–1000 ms + Auto
- Optional oversampling bypass for CPU save

Acceptance Criteria
- No overshoots beyond Ceiling with true-peak on (±0.1 dB tolerance)
- Kick/snare transients preserved with reasonable release; no obvious distortion at −0.3 dBFS ceiling
- C++ unit test renders a transient buffer through the limiter; verifies peaks <= ceiling (offline render harness)

Implementation Steps
1) Files (native VST3):
   - `native/vst3/CR717/Source/dsp/dynamics/Limiter.h/.cpp`
   - `native/vst3/CR717/Source/processing/MasterBus.*` (chain after compressor)
2) True-peak detection: 4× oversampling via `juce::dsp::Oversampling<float>` (linear-phase FIR)
3) Lookahead: `juce::dsp::DelayLine<float>`; instant attack gain computer to clamp peaks
4) Parameter smoothing for threshold/ceiling/release via `juce::SmoothedValue`
5) Tests: `native/tests/unit/dsp/test_limiter.cpp`

Affected Files
- `native/vst3/CR717/Source/dsp/dynamics/Limiter.*` (new)
- `native/vst3/CR717/Source/processing/MasterBus.*` (update)
- `native/tests/unit/dsp/test_limiter.cpp` (new)

Risks & Mitigations
- CPU from oversampling: allow toggle (on by default); use short FIR
- Latency from lookahead: document and expose; keep acceptable for plugin use
