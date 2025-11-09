Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 54 — QA: pluginval + DAW/OS/DPI Matrix

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§17 Test matrix, §22 Compatibility)
- Best practices: `.sdd/best_practices_vst.md`

Goal
- Validate the plugin across target DAWs/OS/DPI scales; ensure stability of preset/state flows and OS-level MIDI DnD.

Scope
- Run `pluginval` suites on macOS/Windows.
- DAW matrix: Ableton Live, Reaper, Studio One; Windows 10/11 (100/150/200%), macOS 13–15 (Retina/non-Retina).
- Scenarios: move window across monitors (Per-Monitor-V2), load/save presets, rapid preset switching, DnD `.mid` into DAW.

Acceptance Criteria
- `pluginval` passes; no critical assertions.
- Manual matrix completed; issues tracked; screenshots/videos attached for UI scale cases.
- Preset/state load path verified under host orders (getState → createEditor → setState).

Tasks
1) Add `scripts/qa/run-pluginval.sh` and instructions.
2) Prepare manual checklist doc from spec §17.
3) Capture results and file bugs for regressions.

References
- Spec §17/§22; JUCE/Steinberg docs; pluginval.

