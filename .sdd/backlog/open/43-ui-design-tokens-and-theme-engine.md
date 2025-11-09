Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 43 — UI Design Tokens and Theme Engine (JUCE)

Context
- Architect spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§3 Tokens, §13 Palette, §14 Sizes, §21 Artifacts)
- Best practices: `.sdd/best_practices_vst.md` (UI performance, AA contrast, DPI)
- Functionality brief: `.sdd/functionality.md`

Goal
- Implement a centralized design-token system (colors/typography/spacing/radii/shadows) and dark theme, exported to JUCE as constexpr values and `juce::Colour`s.

Scope
- Token source: JSON (colors, sizes, spacing). Build-time generator → C++ header `ui_tokens.hpp` (namespace `ui::tokens`).
- Colors: map provided HSL palette to sRGB `juce::Colour` (AA ≥ 4.5:1 for body text).
- Typography: font stack (SF Pro/Segoe UI), 11/12/14/16/20 pt steps, line-height 1.2–1.35.
- Spacing/radii: base 8 px scale; radii 4/8 px; subtle shadows.
- Theme: dark default, easy extension for light.
- JUCE LookAndFeel: create `LookAndFeelCR717` consuming tokens; set as default in editor.
- Assets: prepare token JSON → C++ pipeline in CMake; add `tokens.json` to repo; generate @ build.

Non-Goals
- Full component library visuals (covered in Ticket 47).

Acceptance Criteria
- A `ui_tokens.hpp` is generated at build with constants for colors/sizes/spacing.
- `LookAndFeelCR717` applies tokenized colors to common controls; text contrast passes WCAG AA.
- Theme switch function exists (dark ready, light placeholder).
- Builds on macOS/Windows; no runtime allocations in paint.

Tasks
1) Create `resources/ui/tokens.json` from spec (§3.1/3.2).
2) Add CMake codegen step to emit `src/ui/ui_tokens.hpp` from JSON.
3) Implement `LookAndFeelCR717` and wire to editor.
4) Add unit check (small JUCE Comp) that prints/validates token values in Debug.
5) Document tokens in `.sdd` and add to README developer notes.

References
- Spec §3, §13, §14, §21; JUCE LookAndFeel; Apple HIG contrast.

