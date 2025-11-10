# 62 — Calibration, Meter Alignment, Tests, and Performance

Read `.sdd/best_practices_vst.md`, native coding rules, and Ticket 48 (meters) first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (global validation)
- Goal: Calibrate gain structure across voices → FX → dynamics; align meters; add C++ unit tests for dynamics/FX; verify performance budgets in plugin context.

Dependencies
- Tickets 56–59 (dynamics/FX) and 60 (voice params)
- Ticket 48 (UI meters) and 49 (mixer)

Scope
- Level calibration at −18 dBFS nominal; headroom for bus comp/limiter per spec
- Verify compressor GR ranges, limiter ceiling (true-peak), clipper gain-comp
- Validate reverb/delay tails & feedback stability
- Performance: ensure no long tasks; measure CPU p95 during playback

Acceptance Criteria
- True-peak limiter prevents overshoots beyond ceiling (±0.1 dB)
- Bus comp GR within expected range on test loop; SC-HPF effect measurable
- Clipper maintains loudness within ±0.5 dB (Mix=50%)
- E2E playback shows 0 long tasks; memory <100MB after 5 min; jitter <2 ms

Implementation Steps
1) Add fixtures: test loops/impulses under `native/tests/fixtures/`
2) C++ unit tests for compressor/limiter/clipper and FX using offline buffer renders
3) Integration test: offline render of full chain; assert meter readings
4) Perf: measure block processing time in a host-simulated harness; ensure pluginval passes

Affected Files
- `native/tests/fixtures/*`, `native/tests/unit/dsp/*`
- `native/tests/integration/audio_chain_tests.cpp`
- `native/vst3/CR717/Source/ui/editor/*` (optional meter hooks)

Risks & Mitigations
- Host differences: validate in at least one DAW (e.g., Live/Reaper) after pluginval
- Offline render limitations: test relative properties, not absolute spectra
