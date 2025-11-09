TR-808 Garage — Functionality & Current UI Summary (For redesign brief)

Purpose
- Provide a complete, precise description of the current instrument’s functionality and UI, plus gaps and constraints, so another designer/AI can propose a top-tier, modern VST UI redesign. Treat this document as the “what exists + what we need” brief.

Product Summary
- Type: Drum machine instrument inspired by TR‑808
- Platforms:
  - Web app (TypeScript/Vite) — reference UX and behavior
  - Native plugin (VST3 now; AU optional) — in progress
- Goal: premium, modern VST instrument, low-latency, clean minimal aesthetic, faithful 808 character with modern workflow (presets, FX, MIDI export, DAW integration)

Core Features (functional)
- 12 Voices (current web app)
  - BD (Bass Drum)
  - SD (Snare Drum)
  - LT, MT, HT (Low/Mid/High Tom)
  - RS (Rimshot)
  - CP (Clap)
  - CH (Closed Hi‑Hat)
  - OH (Open Hi‑Hat)
  - CY (Crash)
  - RD (Ride)
  - CB (Cowbell)
- Per-voice parameters (web app defaults, see src/state/voiceParams.ts)
  - BD: level, tone, decay, tuning
  - SD: level, tone, snappy, tuning
  - Toms (LT/MT/HT): level, tuning, decay
  - RS/CP/CB: level
  - CH/OH: level, decay
  - CY/RD: level, tone, decay
  - Note: in the plugin we plan to add FineTune and Pan for tuned voices, plus multi-out routing; choke group for hats.
- Pattern system (web app)
  - 16-step grid, pattern banks A–H
  - Default bank content: curated UK Garage variants (A–H) with BPM and swing suggestions
  - Global: BPM, Swing (0–75%)
  - Swing applied to odd 16ths; hat patterns designed to feel authentic under swing
- Presets & Banks
  - Web: banks A–H with genre‑styled UKG presets
  - Plugin (planned): multiple style banks (House, Techno, DnB, Trap, Electro, Lo‑Fi, Footwork), ≥ 6 presets per style
- MIDI & Host
  - MIDI mapping (planned): GM‑like notes to voices, velocity→amplitude
  - Host sync (planned): BPM/transport; used for tempo‑synced delay and (optionally) internal step module
- FX & Master (planned)
  - Per‑voice sends A/B (Reverb/Delay), Master chain (comp/limiter/soft-clip), true‑peak metering
- Export/Integration (planned)
  - External OS drag‑and‑drop of MIDI (.mid) from plugin into DAW tracks/clips (e.g., Ableton), temp file + fallback “Save MIDI…”

Current Web UI (reference)
- Layout
  - Header: Logo, Controls container, BPM slider+value, Swing slider+value, Load Default, Theme toggle
  - Main: Sequencer section (LED pads per instrument row, 1–16 header with 4‑step grouping), Sidebar: Voice Params panel, Info panel
  - Footer: Status text, FPS, Help button; Help overlay (shortcuts)
- Tokens (styles.css + src/ui/tokens.css)
  - Dark theme colors (bgPrimary #1A1A1A, accents #FF6B35, etc.), spacing scale, radius, shadows, type sizes
- Interaction
  - Step toggling with LED feedback, current step highlight (green glow), instrument labels clickable to select voice panel
  - Keyboard shortcuts (Space play/stop, +/- BPM, D default pattern, ? help)
- Pattern Banks
  - PatternSelector (A–H buttons, copy/paste/clear), hotkeys 1–8

Current Plugin UI (VST3) — state & issues
- Status: early stage. We have UI/UX backlog tickets to reach parity and premium quality (35–36), plus DSP/FX/presets tickets.
- Critical issues (user‑reported)
  - Controls misaligned, drifted; proportions off; widgets at inconsistent sizes
  - Missing or unrefined step sequencer (acceptable to ship later, but layout must account for it)
  - Dragging MIDI out of plugin to DAW fails (disappears crossing plugin window)
  - Presets do not populate sequencer on selection (wiring missing)
- Root causes (likely)
  - No strict grid/constraints system; missing tokens→JUCE mapping; no DPI scaling plan
  - Using JUCE internal DnD instead of OS‑level external file DnD
  - PresetManager not updating SequencerModel/UI in sync; state recall incomplete

Technical Constraints & Targets
- Engine: JUCE 7.x, VST3 SDK; RT‑safe audio (no allocations in processBlock); UI at 60 fps with ≤16 ms paint budget
- Cross‑DAW behavior: Ableton, Reaper, Studio One as minimum smoke
- UI scaling: min window 1100×620; scale 0.75–1.25; DPI 1× and 2× crisp
- Accessibility: focus rings, keyboard traversal, hit targets ≥ 28 px, AA contrast

What we need from the redesign (goals)
- Visual language: modern, clean, premium; minimal skeuomorphism; readable hierarchy
- A full design system: tokens (colors, spacing, type, radii, shadows), component specs, micro‑interactions, themes (dark/light)
- Robust layout: clear sections — Header/Transport, Pattern Bank (A–H w/ copy/paste/clear + preset browser), Voice Grid (12 voices), optional Step Grid module (1–16 header + group‑of‑4 spacing), Mixer/FX tab, Footer/Status
- Component library with JUCE‑friendly specs: Knob, Fader, Toggle, Pad, Meter, Tabs, Dropdowns, Buttons, Segmented controls; all sizes and state visuals
- Performance by design: repaint coalescing, dirty rect painting, caching heavy paths; meters at 30–60 Hz; no allocations in paint
- Integration: UI affordances for external MIDI drag‑and‑drop and preset browser

Open Problems to Solve (explicit)
- External MIDI drag‑and‑drop to DAW: provide a Drag MIDI affordance that initiates OS‑level DnD using a temp .mid file; add Save/Reveal fallback
- Preset→Sequencer wiring: selecting preset instantly updates pattern+UI and persists in state for DAW recall
- Layout/dimensions: enforce grid; tokenized spacing; fix proportion/scale; verify 1×/2× DPI
- Full voice parity: expose all 12 voices with pro‑friendly controls (incl. FineTune, Pan, optional multi‑out); hats choke group
- Modern content: ship multiple genre banks, preset browser with style filters, and a genre‑aware pattern randomizer/humanizer

Code Pointers (web)
- Pattern & bank: src/state/pattern.ts, src/state/store.ts, src/state/types.ts
- Voice params: src/state/voiceParams.ts
- UI elements: src/ui/sequencerGrid.ts, src/ui/voiceParamsPanel.ts, styles.css, src/ui/tokens.css

Design References (to consider)
- Top drum machine plugins: Roland TR‑808/909 Cloud, D16 Nepheton/Drumazon, Sonic Charge Microtonic, Arturia Spark, Softube Heartbeat, NI Battery, XLN XO/AD
- Modern minimal instrument UIs with strong typography, clear grid, subtle depth, and consistent motion

Deliverable expected from design research
- A complete, implementation‑ready spec (Markdown/Figma) covering:
  - Tokens (JSON + JUCE mapping), grid/layout, components with exact geometry/state, micro‑interactions, theming
  - Detailed comp for each section (Header, Pattern Bank, Voice Grid, optional Step Grid, Mixer/FX, Footer)
  - Behavior for external MIDI DnD, preset browser, state recall; performance constraints
  - Asset list (icons, fonts), and style audit images

Non‑Goals (for now)
- Overly skeuomorphic skins; heavy textures; 3D dials with complex shadows
- Long/fancy animations that risk jank or distract in DAW workflows

Notes
- The step sequencer is optional for v1 of the plugin UI but must be designed as a module we can enable later without redesigning the whole layout.
- The web app currently serves as a source for behavior and content (patterns, params) but not as a visual quality bar — the plugin must look premium.

