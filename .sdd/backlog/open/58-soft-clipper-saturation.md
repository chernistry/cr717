# 58 — Soft Clipper / Saturation (tanh/atan/polynomial) with Oversampling

Read `.sdd/best_practices_vst.md` (§4, §10) first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§1.3 Soft Clipper)
- Goal: Musical soft clipping as final polish (or pre-limiter), with selectable curve and parallel mix; oversampling to reduce aliasing.

Dependencies
- Ticket 57 (limiter) — order may be: Compressor → Clipper → Limiter (decide in ADR)
- Ticket 62 (calibration) — level matching for fair A/B

Scope
- Curves: tanh (smooth), atan (harder), polynomial (x - x^3/3)
- Drive 0–24 dB, Threshold 0–100% (map to dBFS), Output −12..+12 dB, Mix 0–100%
- Oversampling 2×/4× selectable (default 4×)
- Parameter smoothing to avoid zipper noise (SmoothedValue)

Acceptance Criteria
- With Mix=100% and Drive=12 dB, harmonics increase without harsh aliasing (OS on)
- Output compensated to within ±0.5 dB of input when Mix=50% and Output=auto
- Unit test verifies no-NaN, bounded output, and basic level-comp behavior on sine sweep

Implementation Steps
1) Files (native VST3): `native/vst3/CR717/Source/dsp/dynamics/Clipper.h/.cpp`
2) Implement 2×/4× oversampling via `juce::dsp::Oversampling<float>` (short linear-phase)
3) Add auto output compensation (approx equal-loudness by estimated RMS delta)
4) Place in master chain (pre/post limiter based on ADR-clip-order)
5) Tests: `native/tests/unit/dsp/test_clipper.cpp`

Affected Files
- `native/vst3/CR717/Source/dsp/dynamics/Clipper.*` (new)
- `native/vst3/CR717/Source/processing/MasterBus.*` (update)
- `native/tests/unit/dsp/test_clipper.cpp` (new)

Risks & Mitigations
- Aliasing if OS off: default OS on, allow off for perf
- Phase/latency from FIR: short taps; document negligible latency for synth use
