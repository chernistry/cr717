Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 50 — Preset Browser, Banks A–H, Randomizer/Humanizer

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§4.1 Top Bar, §7 Presets, §12 State)
- Functionality: `.sdd/functionality.md` (pattern banks A–H)

Goal
- Implement a modern preset browser overlay, bank A–H selection, state save/recall, and genre-aware randomizer/humanizer for patterns and params.

Scope
- Browser: overlay 960×560, keyboard nav (↑/↓, Enter Load, Cmd+S Save), search/filter, categories/styles.
- Banks: A–H selector; per-bank pattern slots; copy/paste between banks.
- Randomizer/Humanizer: genre-aware templates (e.g., UKG/House/Techno/etc.) with swing/velocity variations.
- State: APVTS `getStateInformation/setStateInformation`; VST3 Program Lists exposure.

Acceptance Criteria
- Preset load instantly updates UI and sequencer model; no stale state.
- Banks A–H work with copy/paste/switch; persistence across sessions.
- Randomizer generates playable patterns per style with reasonable swing/human feel.
- Host program changes reflect current preset; saving/restoring project state is stable.

Tasks
1) Implement `PresetBrowser` overlay with keyboard controls.
2) Implement Banks A–H and pattern slot management.
3) Implement randomizer/humanizer; add a few seed styles.
4) Wire APVTS state and VST3 Program Lists.

References
- Spec §4.1/§7/§12; JUCE state; Steinberg Program Lists.

