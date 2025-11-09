Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# VST3 DSP Core — 808 Voices (Low-Latency, RT-Safe)

Objective
- Implement high-quality 808 voices (BD, SD, CH, OH, CP, RS; later LT/MT/HT/CY/RD) with low latency and real-time safety. Choose sample-based MVP with accurate envelopes to minimize complexity while sounding authentic.

Approach
- Use high-quality 808 samples (multi-velocity or single velocity with gain curve) and per-voice envelopes/filters. Preload samples into memory (BinaryData or pre-bundled resources). No dynamic allocation in `processBlock`. Parameter smoothing (exponential/linear) for clicks prevention.

Acceptance Criteria (DoD)
- Voices: BD, SD, CH, OH, CP, RS implemented and triggered via MIDI notes.
- Per-voice params working: Level, Tune (coarse/semitone), Decay, Tone (simple filter), Snappy (SD noise mix), Hat Decay.
- RT-safe: zero allocations in `processBlock`, lock-free queues if needed, denormals disabled.
- Performance: ≤2% CPU at 44.1k, 64-sample buffer on reference laptop; no clicks/pops switching params.
- Sample-rate changes and block sizes handled correctly.

Steps
1) Assets
   - Place samples under `native/vst3/TR808Garage/Resources/808/` (document licensing/source) and embed as BinaryData or load once on prepareToPlay.
2) Voice engine
   - Implement Voice classes per instrument with envelope, tune (resampling), simple tone filter. Precompute resample ratios; no per-sample heap.
3) Parameter smoothing
   - Add rampers for Level/Tone/Decay/Tuning to avoid zipper noise.
4) MIDI trigger
   - Map GM-like notes to voices (doc in README). Velocity → amplitude mapping.
5) Testing
   - Monitor buffer under/overruns; verify thread-safety; run static analysis if available.

Affected Files
- `native/vst3/TR808Garage/Source/*` (Voice classes, Processor, BinaryData)
- `native/vst3/TR808Garage/Resources/808/*`

Risks & Mitigations
- Sample licensing → use free 808 sets with permissive licenses or synthesize later.
- Tuning artifacts → consider high-quality resampler (linear ok for MVP; later improve).

Dependencies
- Ticket 30 (bootstrap).

