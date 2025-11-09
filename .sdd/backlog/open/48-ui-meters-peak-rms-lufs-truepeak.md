Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 48 — Meters: Peak/RMS, True-Peak, LUFS (EBU/ITU)

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§6/§16 Metering)
- Best practices: `.sdd/best_practices_vst.md` (lock-free metering, UI rate)

Goal
- Implement master and per-channel meters with proper ballistics and display modes: Peak, RMS, Short-term LUFS, Integrated LUFS, True-Peak.

Scope
- UI: narrow vertical meters (10–14 px width, 120–160 px height), markers at 0/-1/-6/-12/-18/-24/-48 dBFS; LRA view in preset browser/analytics.
- Engine taps: lock-free ring buffer from audio thread to UI thread.
- Update rate: 30–60 Hz on UI thread; no allocations in paint.
- Theme-aware colors; clip indicator with hold/falloff.

Acceptance Criteria
- Meters show stable, correct values; peak/true-peak difference visible at overs.
- UI updates at 30–60 Hz; no audio thread locks/allocations.
- Visual markers match spec; clip indicator latches and clears per spec.

Tasks
1) Add meter taps and ring buffer in DSP (reader on UI thread).
2) Implement `MeterView` with tokenized colors and markers.
3) Add True-Peak/LUFS mode toggles and ballistics.
4) Performance verify (profiling) and screenshots for docs.

References
- Spec §6/§16; ITU-R BS.1770, EBU Tech 3341/3342.

