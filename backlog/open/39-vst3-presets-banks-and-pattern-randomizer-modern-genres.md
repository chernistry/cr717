Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# VST3 Presets (Banks), Genre‑Aware Pattern Randomizer, Modern Styles

Objective
- Provide rich, modern factory content and creative tools: multiple preset banks (sounds + patterns) across popular electronic styles and a genre‑aware pattern randomizer/humanizer. Minimum 6 presets per style; tempo/swing suggestions included.

Styles & Targets (≥ 6 each)
- UK Garage / 2‑Step (132–138 BPM; swing 54–62%)
- Drum & Bass (170–176 BPM; swing minimal)
- Techno (128–136 BPM; straight; occasional triplets)
- House / Deep House (120–126 BPM; mild swing 52–56%)
- Electro / Breaks (128–132 BPM; syncopation; claps/snare variations)
- Trap / Hip‑Hop (70–80 BPM or 140–160 double‑time; distinct hat rolls)
- Lo‑Fi (72–96 BPM; soft hats; lower velocities; tape‑ish)
- Footwork / Juke (155–165 BPM; toms & syncopation)

Acceptance Criteria (DoD)
- Factory preset banks shipped with plugin (embedded resources or separate JSON):
  - Each preset includes: name, style, recommended BPM/swing, voice parameter set, pattern steps (if Step module available), FX send amounts, master chain defaults.
  - At least 6 presets per style (≥ 48 total).
- Randomizer:
  - Genre‑aware randomization: per‑voice probabilities, constraints (snare backbeat for house/garage, kick syncopation for 2‑step, hat densities per genre), swing alignment.
  - Humanization: subtle timing (pre‑quantized offset range), velocity variance, hat rolls (trap/DnB) where applicable.
  - Seed support; undo/redo safe; “lock” voices from change.
- Preset Browser UI: searchable list, style filters, favoriting, last‑used.
- State integration: presets load/save with APVTS; host automation compatible.

Steps
1) Content format
   - Define Preset schema JSON; include style meta, BPM/swing, params, pattern (if present), FX sends, master.
2) Banks & authoring
   - Author 6+ per style (start with UKG/House/Techno/Trap/DnB/Lo‑Fi/Electro/Footwork). Add unit tests validating schema.
3) Randomizer
   - Implement genre config tables (probabilities per instrument, allowed placements, density). Add humanization config.
   - UI: Randomize button with genre dropdown and amount (mild/medium/extreme); lock toggles per voice.
4) Browser
   - Preset list with style filter, search; load/apply with smoothing.
5) QA
   - Verify loads are click‑free; check content path size; ensure no blocking IO on audio thread.

Affected Files
- native/vst3/TR808Garage/Source/content/{Preset,Randomizer,Browser}.*
- native/vst3/TR808Garage/Resources/presets/*.json
- UI panels under `ui/presets/*`

Risks & Mitigations
- Content size → embed only metadata; ship patterns/params compact; compress.
- Randomizer going off‑genre → cap extremes; offer “mild” default; add “lock” per voice.

Dependencies
- 32 (parameters/state), 33/35 (GUI editor), optional Step Grid module.

