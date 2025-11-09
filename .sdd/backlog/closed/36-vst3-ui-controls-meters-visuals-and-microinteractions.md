Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3/AU UI — Controls, Meters, Visuals, Micro‑interactions (Premium Feel)

Objective
- Implement best‑in‑class controls (knobs, faders, toggles, pads), meters, and micro‑interactions to achieve a premium commercial VST feel. Port winning patterns from Web UI; refine every element in detail; keep performance at 60 fps and avoid any audio‑thread interference.

Controls (Detailed Spec)
- Knob (rotary, continuous):
  - Size: 40–52 px (base), scalable. Ring arc 300°, tick marks every 10°, main at 12/3/6/9.
  - States: normal (soft shadow), hover (accent ring glow), active (accent fill), disabled (50% alpha). Fine adjust with Alt (×0.1). Double‑click resets to default. Drag vertical (relative), horizontal for coarse (Shift).
  - Value tooltip near knob center (rounded rect), throttle at 30 Hz.
  - Drawing: pre‑rendered tick paths cached; ring gradient; no per‑pixel allocs.
- Fader (vertical):
  - Height 120–160 px; handle 12×20 px with accent; track with subtle gradient and 1 px inner highlight.
  - Steps: smooth; Shift for coarse; Alt for fine; double‑click reset.
- Toggle/Switch:
  - Two‑state pill (28×16 px), knob snaps; hover shadow; space/enter toggles when focused.
- Pad (step/instrument):
  - LED indicator (inner circle); soft glow when active; current step outer glow; press animation 100 ms.
- Meter (peak + RMS approx):
  - Vertical bar, 0–‑inf dB; color zones: green (<‑12 dB), yellow (‑12..‑3), red (>‑3). Peak hold (1 s), RMS trail. Update timer 30–60 Hz. Lock‑free data from audio thread.

Micro‑Interactions & UX
- Focus rings: 2 px accent outline with 2 px offset on focusable controls.
- Tooltips: short labels; appear after 500 ms; do not steal focus.
- Keyboard: Tab traversal by column; Enter toggles; arrows nudge knob (±1%), Shift (±10%).
- Animations: only on UI thread; capped at 100–200 ms; no layout thrash.

Visual Polish
- Shadows: sm/md/lg tokens; no heavy blurs; cache drop‑shadow paths.
- Highlights: inner 1 px highlight on panels; subtle gradients on elevated surfaces.
- Icons: 1‑color vector paths (e.g., phosphor icons converted to Path); pre‑load; no raster scaling artifacts.

Performance
- Avoid repaint storms: coalesce parameter change notifications; throttle meters; repaint dirty bounds only.
- Do not allocate in paint(); pre‑allocate paths/gradients. Profile paint() time (dev overlay) and ensure ≤16 ms/frame.

Mapping to Parameters
- Per voice (BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB):
  - Level (0..1), Tune (0.5..2.0, skewed), Decay (per voice default caps), Tone (0..1), Snappy/HH Decay where applicable.
- Global: Master Level, optional Accent amount (future), Swing (if sequencer is present).

State & Defaults
- Display defaults as faint ticks; double‑click resets to defaults.
- Persist last selected voice panel; theme toggle persists.

Testing & QA
- Interaction tests: click/drag knobs/faders; keyboard focus; tooltips; double‑click reset.
- Visual: check aliasing at 1× and 2× DPI; consistent colors with tokens.
- Perf: enable FPS overlay; ensure 60 fps; no GC pressure.

Deliverables
- `ui/controls/*.{h,cpp}` for Knob, Fader, Toggle, Pad, Meter; `ui/style/Tokens.{h,cpp}`; `ui/icons/*`.
- Demo screen captures (GIF/MP4) under `docs/ui/` showing interactions.

Dependencies
- 33 (GUI editor) + 32 (parameters) for attachments; meters rely on audio thread taps in 31.

