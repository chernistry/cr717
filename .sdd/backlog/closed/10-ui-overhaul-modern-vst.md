# UI Overhaul — Modern VST‑grade Interface (M1)

Objective
- Redesign the UI to feel like a modern VST-quality drum machine in one iteration: visually polished, discoverable, and performance-ready. No backend; browser‑only.

Scope (In / Out)
- In: Full visual redesign; responsive layout; theming; performance UI; accessibility; keyboard shortcuts; parameter controls wired to existing audio params.
- Out: Non‑UI audio DSP overhauls (keep current engine; add stubs only if needed).

Acceptance Criteria (DoD)
- Visual design: modern panel layout with clear sections: Transport, Pattern, Instruments, Step Grid, Mixer, FX, Settings.
- Controls per voice: Level, Pan, Tune, Decay, Tone; Snare “Snappy”, HH “Decay”. Mute/Solo per voice.
- Step grid: 16 steps with on/off, Accent toggle, and per‑step Velocity (0–127) via mini‑bar or alt‑drag.
- Global controls: BPM, Swing %, Master Volume; optional Metronome toggle.
- Mixer strip: per‑voice meters (RMS peek), master meter; clipping indicator.
- FX send panel: simple Reverb/Delay sends (if engine lacks these, UI exists, knobs disabled with tooltip).
- Performance: UI renders ≥ 55 fps on laptop (no jank during playback); audio remains click‑free under normal operation.
- Responsive: ≥ 1024px desktop optimal; usable ≥ 768px width; resizable layout preserves hierarchy.
- Theming: Dark default; CSS variables for colors/space/typography; easy skinning.
- Accessibility: Focus states, ARIA roles/labels on controls; keyboard navigation across key controls.
- Keyboard shortcuts: Space (Play/Stop), Arrow keys (move step cursor), Numbers 1‑8 select voices, +/- change BPM.
- Help: Quick overlay ("?") listing shortcuts and control hints.
- Docs: Updated README section “UI Overview” with screenshots and control map.

Steps
1) Design System
   - Add `src/ui/tokens.css` with CSS variables (colors, spacing, radius, elevations, typography, shadows).
   - Establish global layout grid; update `styles.css` to use tokens.
2) Layout & Navigation
   - Build container layout: header (Transport/BPM/Swing), main (Grid + Instruments + Mixer/FX tabs), footer (status/hints).
   - Add responsive breakpoints (≥768px, ≥1280px); wrap/stack sections appropriately.
3) Controls
   - Implement reusable control primitives: `Knob`, `Fader`, `Toggle`, `Pad`, `Meter` (SVG or Canvas; inertial drag; fine adjust with Alt).
   - Wire per‑voice controls (Level, Pan, Tune, Decay, Tone, Snappy/HH Decay) to existing parameters; if missing, add no‑op stubs with TODO.
4) Step Grid
   - 16‑step grid UI with per‑step on/off, Accent, Velocity (mini bar beneath the step or Alt‑drag to set 0–127). Cursor highlights current step.
   - Efficient rendering (requestAnimationFrame; minimal DOM updates).
5) Mixer & FX
   - Per‑voice meters (RMS approx; peak hold); master meter; clip indicator.
   - FX sends panel (Reverb/Delay) with disabled state if engine lacks effect nodes; tooltips explain.
6) Keyboard & A11y
   - Space for Play/Stop; +/- for BPM; number keys to select voice; Enter toggles step.
   - ARIA roles/labels for controls; tab order sensible; visible focus rings.
7) Theming & Skins
   - Dark theme default; add `data-theme` to `html` for switching; a light theme stub.
8) Performance Pass
   - Avoid layout thrash; throttle resize; batch state updates; ensure ≥ 55 fps during playback; verify no audio clicks.
9) Docs & Screenshots
   - Update README with screenshots, control map, and shortcuts; add a short “Skins” section.

Affected Files
- `index.html` — mount points and ARIA landmarks
- `styles.css` — base reset, imports, grid scaffolding
- `src/ui/tokens.css` — design tokens (new)
- `src/ui/controls/{Knob,Fader,Toggle,Pad,Meter}.ts` — UI primitives (new)
- `src/ui/layout/{Header,Grid,Instruments,Mixer,FX,Footer}.ts` — structural components (new)
- `src/state/ui.ts` — UI state and bindings (new)
- `src/audio/params.ts` — parameter bindings/shims (new; wrap existing engine)
- `README.md` — UI Overview section with screenshots

Tests
- Unit: parameter mapping (src/audio/params.ts), control math (Knob/Fader value->param).
- Component: grid toggling, velocity change, mute/solo wiring.
- E2E (Playwright): Play/Stop works; BPM changes tempo; step toggling audible; keyboard shortcuts operate.
- A11y: basic role/label checks; focus traversal; contrast via tokens.
- Perf: CI smoke (raf loop baseline), manual profile notes included in README.

Risks & Mitigations
- Missing engine features for some controls → ship UI disabled with tooltips; file follow‑up tickets for DSP.
- Performance regressions → batch updates, raf scheduling, avoid heavy layouts; test on a mid‑tier laptop.
- Accessibility gaps → add an explicit A11y check list; iterate if violations found.

Dependencies
- None hard. Assumes baseline sequencer and audio start/stop exist. If not, stub UI with clear disabled states.
