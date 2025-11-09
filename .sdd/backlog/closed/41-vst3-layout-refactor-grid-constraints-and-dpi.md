Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 — Layout Refactor (Grid, Constraints, DPI) for Clean Composition

Problem
- Elements appear misaligned and “drift”; layout breaks on certain sizes. Need a robust grid/constraints system matching the spec.

Objective
- Rebuild the Editor layout with a deterministic grid, token‑based spacing, consistent gutters/margins, DPI‑aware scaling, and componentized sections. Guarantee visual consistency at min/recommended sizes and common DPI scales.

Acceptance Criteria (DoD)
- Editor respects min size 1100×620; scales to 0.75–1.25 while preserving alignment and gutters.
- Sections (Header, PatternBank, VoiceGrid, optional StepGrid, Tabs for Voice/Mixer/FX, Footer) align to grid; no overlaps; consistent paddings.
- DPI 1× and 2× render crisp; hit targets ≥ 28 px.
- Tokens (spacing/colors/radii) applied consistently via LookAndFeel/Tokens class.

Design
- Use `juce::Grid` (or FlexBox where simpler) with named template areas. Centralize spacing in `Tokens` singleton.
- Component hierarchy: Editor → HeaderBar, PatternBankBar, Main (Left/Right), FooterBar. Main.Left → StepGrid (optional) + VoiceGrid; Main.Right → Tabbed (VoiceParams, MixerFX).
- Add `ComponentBoundsConstrainer` for resizable windows; clamp min sizes.

Steps
1) Tokens
   - Implement `ui/style/Tokens.{h,cpp}` mapping Web tokens to JUCE (colors, spacing, radii, fonts, shadows).
2) Editor grid
   - Define Grid templates; compute rows/cols in `resized()`; use constants from Tokens; add helper to place components with consistent gutters.
3) DPI & scaling
   - Use `Desktop::getInstance().getDisplays()` to get scale; snap dimensions; keep vector icons.
4) QA
   - Visual pass across sizes; verify alignment; add debug overlay toggling grid (developer only).

Affected Files
- `native/vst3/CR717/Source/ui/{Editor,HeaderBar,PatternBankBar,Main,FooterBar}.*`
- `native/vst3/CR717/Source/ui/style/Tokens.*`

Dependencies
- 35 (design system), 33 (GUI base).
