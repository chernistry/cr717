Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 52 — UI Performance & Paint Optimizations

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§11 Performance, §20 Anti-patterns)
- Best practices: `.sdd/best_practices_vst.md` (no allocations in paint, buffered images)

Goal
- Achieve 60 fps target through dirty-rect repainting, buffered images for heavy visuals, throttled updates, and minimal layout recompute.

Scope
- Buffered backgrounds for panels/knobs; invalidate only changed regions.
- Coalesce repaints; throttle high-frequency UI updates.
- Avoid dynamic allocations in `paint()`; pre-allocate paths/resources.
- Profile and document hotspots.

Acceptance Criteria
- Under continuous interaction (knob scrubbing + meters), frame time ≤ 16 ms on target machines.
- No GC/alloc spikes in `paint()`; reduced repaint rectangles observed.
- Performance checklist documented; screenshots/profiles attached.

Tasks
1) Add buffered images for heavy vector content.
2) Dirty-rect repaint strategy across main containers.
3) Throttle UI timers; batch state updates.
4) Add a profiling build flag and capture traces.

References
- Spec §11/§20; JUCE docs on painting and optimization.

