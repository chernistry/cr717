# 63 — UI: LookAndFeel_V4, Theme Tokens, and Fonts (JUCE)

Read `.sdd/architect.md`, `.sdd/best_practices_vst.md`, and coding standards for native UI first.

Context
- Improve visual quality while staying on JUCE: modern LookAndFeel_V4 customization, design tokens (colors/spacing/typography), custom fonts, gradients, shadows.

Dependencies
- None hard, but aligns with Ticket 49 (Mixer UI) and existing component library.

Scope
- Create centralized theme with design tokens (colors, radii, spacing, elevation, typography).
- Subclass `juce::LookAndFeel_V4` to apply tokens: colours, corner radii, outlines, default component metrics.
- Integrate custom fonts (Google Fonts TTF/OTF) via BinaryData and set as default typefaces.
- Add gradient helpers and token-based palette (light/dark ready) with easy switch.

Acceptance Criteria
- Global look & feel applied app-wide (`setDefaultLookAndFeel`) without visual regressions.
- Buttons, sliders, combo boxes use themed colours, radii, and typography from tokens.
- Fonts embedded (BinaryData) and used via `Typeface::createSystemTypefaceFor` with proper fallback.
- Gradients and subtle shadows present on primary UI surfaces per tokens.

Implementation Steps
1) Files (native VST3 GUI):
   - `native/vst3/CR717/Source/ui/theme/DesignTokens.h`
   - `native/vst3/CR717/Source/ui/theme/Theme.h/.cpp`
   - `native/vst3/CR717/Source/ui/theme/LookAndFeelEx.h/.cpp`
2) Design tokens
   - Define constants: `struct DesignTokens { struct Colors {...}; struct Radii {...}; struct Elevation {...}; struct Typography {...}; }`.
   - Provide two palettes: `Light`, `Dark`.
3) LookAndFeelEx
   - Subclass `juce::LookAndFeel_V4`; override draw methods for buttons, sliders, combo boxes; use `juce::DropShadow` subtly.
   - Expose `applyTheme(const Theme&)` to swap tokens.
4) Fonts
   - Add TTF/OTF to `native/vst3/CR717/Resources/fonts/` and embed via BinaryData.
   - Load in `Theme::loadFonts()` using `juce::Typeface::createSystemTypefaceFor(BinaryData::..., size)` and set via `setDefaultSansSerifTypeface()`.
5) Wire-up
   - In editor constructor: `lookAndFeel = std::make_unique<LookAndFeelEx>(); juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get()); lookAndFeel->applyTheme(Theme::makeDark());`
6) QA
   - Visual pass: buttons/sliders/combos reflect tokens; check HiDPI.

Affected Files
- `native/vst3/CR717/Source/ui/editor/*` (integration)
- `native/vst3/CR717/Source/ui/theme/*` (new)
- `native/vst3/CR717/Resources/fonts/*` (new)

Risks & Mitigations
- Font embedding size → pick 1–2 weights; subset fonts if needed.
- Overriding too many draw methods → start with essentials; iterate.

