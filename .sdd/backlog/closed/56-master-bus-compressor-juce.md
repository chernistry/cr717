# 56 — Master Bus Compressor (JUCE/VST3, SSL/API-style)

Read `.sdd/best_practices_vst.md` and native coding rules first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (§1.1 Bus Compressor)
- Goal: Authentic, musical drum-bus compression with RMS detector, soft knee, SC-HPF, optional lookahead. JUCE/C++ implementation suitable for VST3.

Dependencies
- N/A (plugin timing handled by host); ensure sample-accurate parameter smoothing
- Optional: Ticket 62 (calibration & tests) for final gain structure validation

Scope
- Implement feed-forward RMS detector (10 ms window, switchable Peak)
- Soft-knee gain computer; stepped ratios (1:1, 2:1, 4:1, 8:1, 10:1, 20:1, ∞:1)
- Threshold, Attack (0.05–100 ms), Release (10–2000 ms + Auto), Knee (0–12 dB)
- Makeup (−12..+24 dB) with optional Auto-Makeup ≈ equal-RMS target
- Sidechain HPF (20–500 Hz, 12 dB/oct) in detector path
- Lookahead 0–5 ms (default 0), short delay line for signal/detector

Acceptance Criteria
- Default settings yield ~2–4 dB GR on kick+mix loop at −18 dBFS input
- Attack/Release behave logarithmically and feel musical; Auto-Release adapts
- SC-HPF reduces pumping on BD without dulling hats (audible A/B)
- No clicks/pops on parameter change (SmoothedValue applied)
- C++ unit test verifies GR envelope vs synthetic step inputs (offline render harness)

Implementation Steps
1) Files (native VST3):
   - `native/vst3/CR717/Source/dsp/dynamics/Compressor.h/.cpp`
   - `native/vst3/CR717/Source/processing/MasterBus.h/.cpp` (wire-in)
2) Implement detector and gain computer; SC-HPF via `juce::dsp::IIR::Filter`
3) Lookahead: short circular buffer (preallocated) or `juce::dsp::DelayLine<float>`
4) Parameter smoothing: `juce::SmoothedValue`
5) Auto-Makeup: running RMS (≈300 ms) pre/post, compute delta, clamp
6) Tests: `native/tests/unit/dsp/test_compressor.cpp` (offline buffer render)

Affected Files
- `native/vst3/CR717/Source/dsp/dynamics/Compressor.*` (new)
- `native/vst3/CR717/Source/processing/MasterBus.*` (update)
- `native/tests/unit/dsp/test_compressor.cpp` (new)

Risks & Mitigations
- Latency from lookahead: default 0; expose control; document trade-off
- CPU from RMS/filters: use simple IIR HPF, fixed-size window; avoid allocations

---

## API (draft)
```ts
export interface CompressorOptions {
  attackMs: number; // 0.05–100
  releaseMs: number; // 10–2000 or 0 = auto
  ratio: 1|2|4|8|10|20|"inf";
  thresholdDb: number; // -40..0
  kneeDb: number; // 0..12
  makeupDb: number; // -12..+24
  autoMakeup: boolean;
  scHpfHz: number; // 20..500
  detector: 'rms'|'peak';
  lookaheadMs: number; // 0..5
}
```
