Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 — External MIDI Drag & Drop to DAW (Ableton/others)

Problem
- Drag from the plugin disappears when leaving the plugin window and does not paste into Ableton’s sequencer. Current implementation likely uses JUCE internal DnD (DragAndDropContainer::startDragging) which only works inside the plugin, not into the host/OS.

Objective
- Implement OS‑level external drag‑and‑drop of a generated MIDI clip so users can drag patterns from the plugin directly into DAW arrange/clip slots (Ableton, Reaper, etc.). Provide fallbacks if host blocks DnD.

Acceptance Criteria (DoD)
- A “Drag MIDI” handle/area in the Editor initiates OS drag of a temporary `.mid` file representing the current pattern.
- Drag works into Ableton arrange/clip slots; also works in at least one other DAW (Reaper/Studio One).
- If DnD blocked by host/platform, provide “Save MIDI…” button that writes to a chosen folder and reveals in Finder/Explorer for manual drag.
- Temporary files cleaned periodically; names include pattern/preset info and tempo.

Design
- Generate SMF Type 0/1 from current pattern (tempo meta, 16‑step grid to quarter divisions with swing handled by timing offsets if needed). Store in temp dir.
- Use JUCE external DnD:
  - Call `DragAndDropContainer::performExternalDragDropOfFiles({ midifile.getFullPathName() }, false)` on mouse drag from the handle.
  - Provide a custom drag image (ghost) using a cached icon + pattern name.
- Ensure the Editor (or parent) derives from `DragAndDropContainer`.

Steps
1) MIDI export
   - Implement `MidiExporter::createMidiFile(const Pattern& pat, double bpm, double swing)` → `juce::File` temp path. Include loop length and basic meta.
2) UI handle
   - Add `DragHandle` component; on `mouseDown` record start; on `mouseDrag` if threshold exceeded → call `performExternalDragDropOfFiles` with generated file path; show drag image.
3) Cleanup
   - On app start/periodically delete expired temp MIDI files (older than X days).
4) Fallbacks
   - “Save MIDI…” button that opens FileChooser; “Reveal in Finder” for quick drag if external DnD is blocked.
5) QA
   - Validate in Ableton Live (arrange and session clips), Reaper; check file name tempo/swing markers; confirm clip length and note timing.

Notes
- Internal JUCE DnD does not cross the plugin window; external DnD is required.
- Some hosts can still block OS DnD; always ship a Save/Reveal fallback.

Affected Files
- `native/vst3/CR717/Source/ui/DragHandle.*`
- `native/vst3/CR717/Source/export/MidiExporter.*`
- Editor wiring + LookAndFeel for drag image.
