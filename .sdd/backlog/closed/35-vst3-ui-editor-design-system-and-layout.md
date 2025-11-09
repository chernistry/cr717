Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3/AU UI — Design System, Layout, Theming (Modern Drum Machine)

Objective
- Deliver a premium, modern drum‑machine UI for the native plugin (VST3 now; AU later) that closely matches the best paid VST instruments. Port the strongest design elements from the Web app to JUCE, with meticulous attention to every micro element, while keeping 60 fps and RT safety (no blocking audio thread).

References
- Best practices: /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/best_practices_vst.md
- Web UI cues to port: tokens, step header (1–16), 4‑step grouping, LED pads, instrument color stripes, dark theme.

Acceptance Criteria (DoD)
- A responsive, skinnable editor window with grid‑based layout; minimum size 1100×620; scales cleanly 0.75–1.25.
- Sections: Header/Transport+Global, Pattern Bank (A–H) with copy/paste/clear, Voice Grid (12 voices), Step Grid module (optional, can be hidden), Mixer/FX tab, Footer/Status.
- Design tokens implemented in C++ (JUCE) mirroring Web tokens: colors, spacing, radii, shadows, typography, transitions.
- 60 fps UI on a laptop; no stutters when playing/automating; repaint measured ≤16 ms/frame; painting batched and clipped.
- Theme: dark default, light optional; switch at runtime without flicker.
- All controls have consistent states (normal/hover/active/disabled), focus rings, and tooltips.

Design System (Tokens)
- Colors (Dark):
  - bgPrimary #1A1A1A, bgSecondary #2A2A2A, bgTertiary #333333, bgElevated #3A3A3A
  - textPrimary #FFFFFF, textSecondary #AAAAAA, textMuted #666666
  - accent #FF6B35 (hover #FF8555, active #FF4515)
  - success #00FF00, warning #FFAA00, danger #FF3333, border #444444, borderFocus = accent
- Spacing: xs 4 px, sm 8 px, md 16 px, lg 24 px, xl 32 px
- Radius: sm 2 px, md 4 px, lg 8 px
- Shadows: sm 0 1 2 /0.3, md 0 2 8 /0.4, lg 0 4 16 /0.5 (draw with gradients + alpha)
- Typography: System UI; sizes xs 11, sm 12, md 14, lg 16, xl 20; tracking for labels +0.5–1.0
- Transitions: fast 100 ms, normal 200 ms (knob/fader UI only; never audio thread)

Layout (Grid)
- Header (Top Row):
  - Left: Logo (TR‑808 Garage), Transport (Play/Stop optional), BPM numeric (read‑only unless sequencer module enabled), Swing %, Theme toggle, Help.
  - Right: Pattern Bank A–H (buttons), Copy/Paste/Clear, Preset dropdown (factory 8 A–H), Save as…
- Main: Two columns
  - Left: Step Sequencer Panel (optional module; flag to hide until internal sequencer lands); below that Instruments/Pad Grid (12 voices, LED pads).
  - Right: Tabs [Voice Params] [Mixer/FX]
    - Voice Params: per‑voice panel (Level, Tune, Decay, Tone, Snappy/HH Decay). Clicking an instrument label selects the panel.
    - Mixer/FX: per‑voice strips (Level meters), Master strip; Reverb/Delay send controls (disabled until FX nodes exist).
- Footer: Status line (e.g., “Pattern A — UKG Classic”, FPS), minimal diagnostics.

Instrument Cues (Color Stripes)
- Mirror web color hints on instrument labels (subtle 3 px left border): BD=accent, SD=#FFAA00, CP=#FF3385, CH/OH #66D9EF, Toms #C594C5, RS/CB #A6E22E, CY/RD #F92672

Step Header + Grouping (if sequencer shown)
- Header row 1–16 indices, grouped per 4 steps (extra md spacing before steps 1,5,9,13). LED pad style buttons with soft glow, current step highlight (green outer glow), and accent fill when active.

Theming
- Provide Theme struct mapping from tokens; Editor rebuilds cached colors on theme switch. Avoid runtime allocations per frame; pre‑compute brushes/fonts.

Resizing & DPI
- Minimum size 1100×620; scale editor content (rounded to device scale); keep hit targets ≥ 28 px.

Implementation Notes
- Split UI into components: HeaderBar, PatternBankBar, StepHeader, StepGrid, InstrumentRow, VoiceParamPanel, MixerPanel, FXPanel, FooterBar.
- ScopedRepaints; repaint only dirty rectangles; cache expensive paths.
- Run a UI repaint timer at 30–60 Hz for meters only; control repaint on interaction.
- No blocking file IO or allocations in paint(); no locks/allocs in audio thread.

QA
- Visual audit at 1× and 2× scale; keyboard traversal; tooltips; focus rings; color contrast (AA).
- FPS overlay for dev builds; ensure steady 60 fps idle.

Dependencies
- 30 (bootstrap), 32/33 (params & GUI widgets). Can be started in parallel with 33 as they share tokens.
