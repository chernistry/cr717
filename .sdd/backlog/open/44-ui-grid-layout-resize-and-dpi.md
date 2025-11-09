Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 44 — Grid Layout, Resize, DPI Scaling (JUCE/VST3)

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§3.3 Grid, §4 Layouts, §10 DPI/Scaling)
- Best practices: `.sdd/best_practices_vst.md` (Per-Monitor DPI, repaint coalescing)

Goal
- Implement a 12-column grid layout system with base 8 px spacing, window min/recommended/max sizes, and robust DPI/scale handling across monitors/hosts.

Scope
- Editor sizes: min 1100×620, recommended 1200×680, max 1400×780 (without tabs).
- Grid: 12 columns, margins 16 px, gutter 12 px, nested `juce::Grid`/`FlexBox` helpers.
- Scaling: support 0.75–1.25×; respect host `IPlugViewContentScaleSupport`; forward scaleFactor to editor; recompute geometry on `resized()`.
- Per-Monitor-V2 (Windows): adjust scale when moving window between monitors.
- Persist last size/scale in state (controller side), not in processor.

Acceptance Criteria
- Editor can be resized within bounds; grid aligns modules at all sizes with stable spacing.
- Moving the plugin window between monitors with different DPI updates scale correctly (Windows/macOS).
- No blurry text/icons at common scales (1.0/1.25/1.5/2.0 macOS Retina/Windows).
- Repaints use dirty rects only; performance budget 60 fps under typical interaction.

Tasks
1) Implement `UiGrid` helpers for 12-col layout + spacing tokens.
2) Wire VST3 `IPlugView::resizeView` + `IPlugViewContentScaleSupport` → JUCE editor.
3) Add DPI change listener; recalc `getApproximateScaleFactorForComponent()` assets.
4) QA matrix: Windows 100/150/200%, macOS Retina/non-Retina; move across monitors.
5) Add developer docs: layout grid usage patterns.

References
- Spec §3.3/§4/§10; Steinberg content scale; JUCE HiDPI.

