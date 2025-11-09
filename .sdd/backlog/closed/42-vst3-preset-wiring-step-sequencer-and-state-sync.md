Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 — Preset Wiring to Step Sequencer & State Sync

Problem
- Selecting presets does not load patterns into the sequencer. Need reliable wiring between preset selection (APVTS/state) and Step module model + UI redraw.

Objective
- Ensure preset selection updates the internal pattern model and the UI step grid promptly, saves/restores in state, and supports DAW session recall.

Acceptance Criteria (DoD)
- Selecting a factory/user preset updates: pattern steps, BPM/swing suggestions, per‑voice params, FX sends, master chain defaults (where present).
- UI grid redraws immediately; current bank letter/preset shown.
- State saved via `getStateInformation`/`setStateInformation`; DAW session reload restores selected preset and pattern contents exactly.
- Optional: “Apply to DAW” action (export MIDI) and “Drag MIDI” handle (ticket 40) consistent with current pattern.

Design
- Preset structure contains: name/id, style, bpm/swing, per‑voice params, pattern steps, fx sends, master defaults. Stored in JSON or embedded table.
- APVTS listener or custom PresetManager notifies sequencer model → calls UI `updateFromModel()`.
- Threading: apply preset on message thread; parameter smoothing; no audio thread locks.

Steps
1) PresetManager
   - Implement load/select; notify listeners (Editor, Processor). Guard reentrancy. Persist current preset id in state.
2) Sequencer model
   - Expose `applyPattern(const Pattern&)`; update engine’s trigger map; request UI repaint.
3) UI wiring
   - PatternBank/PresetBrowser triggers PresetManager->apply; update labels, status bar.
4) State/Recall
   - Serialize preset id + full pattern snapshot (for edited presets) in state blob. Verify recall across sample rate/buffer size.

Affected Files
- `native/vst3/CR717/Source/{PresetManager, PluginProcessor, SequencerModel}.*`
- `native/vst3/CR717/Source/ui/{PatternBankBar, StepGrid}.*`

Dependencies
- 32 (parameters/state), 39 (preset content), 40 (MIDI export for DAW alignment).
