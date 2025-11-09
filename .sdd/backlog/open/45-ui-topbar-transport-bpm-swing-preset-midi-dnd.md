Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 45 — Top Bar: Transport, BPM/Swing, Presets, Pattern Ops, MIDI DnD

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§4.1 Top panel, §8 MIDI DnD, §12 Preset/State)
- Functionality: `.sdd/functionality.md` (banks A–H, pattern model, swing)

Goal
- Implement the top bar module with host sync indicator, local/host BPM handling, swing, preset browser access, bank A–H selector, pattern copy/paste/clear, and OS-level MIDI drag-and-drop (.mid) to DAW.

Scope
- Host sync: read tempo/position from VST3 `ProcessContext`; toggle Host vs. Local BPM.
- Controls: Play/Stop, BPM numeric/drag, Swing %, Bank A–H, Preset Browser button, Pattern ops (Copy/Paste/Clear).
- MIDI DnD: generate temporary `.mid` file and call `juce::DragAndDropContainer::performExternalDragDropOfFiles`.
- Fallback: Save `.mid` and Reveal in Finder/Explorer when OS DnD unsupported.

Acceptance Criteria
- Host tempo mode engages when host provides tempo; local BPM editable otherwise.
- Swing updates sequencer model immediately and is persisted in state.
- Pattern ops work across banks; copy/paste payload includes velocities/accents.
- Dragging from “Drag MIDI” over DAW timeline inserts the correct pattern; works in Ableton, Reaper, Studio One.
- No UI stalls; DnD uses background thread for file write; UI thread remains responsive.

Tasks
1) Implement `TopBar` component with tokenized layout.
2) Wire host tempo detection and UI toggle.
3) Implement pattern ops (copy/paste/clear) with bank awareness.
4) Implement `.mid` generator + OS DnD + Save/Reveal fallback.
5) Add quick tests/manual checklist for DAWs.

References
- Spec §4.1/§8/§12; JUCE DnD docs; Steinberg ProcessContext.

