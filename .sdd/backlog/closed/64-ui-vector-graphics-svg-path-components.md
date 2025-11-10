# 64 — UI: Vector Graphics via juce::Path and juce::Drawable (SVG)

Read `.sdd/architect.md`, `.sdd/best_practices_vst.md`, and coding rules first.

Context
- Replace raster icons with scalable vector graphics. Use `juce::Path` for custom shapes and `juce::Drawable` for SVG icons. Ensure crisp rendering and theme-aware colours.

Dependencies
- Ticket 63 (theme/tokens) to supply colours and metrics.

Scope
- Introduce `IconFactory` that loads SVGs from BinaryData and tints per theme.
- Implement custom knob/fader backgrounds using `juce::Path` (rounded rects, arcs) with gradient fills from tokens.
- Provide scalable toolbar/status icons as `Drawable` instances (SVG source under Resources).

Acceptance Criteria
- All icons render from vector sources and scale cleanly at 1×/1.5×/2× DPI.
- Knob/fader components render with `juce::Path` using tokenized colours and radii.
- No blurry edges; stroke alignment correct; colours adapt to theme changes.

Implementation Steps
1) Files
   - `native/vst3/CR717/Source/ui/graphics/IconFactory.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/VectorKnob.h/.cpp`
   - `native/vst3/CR717/Source/ui/components/VectorFader.h/.cpp`
   - `native/vst3/CR717/Resources/svg/*.svg` (new assets)
2) IconFactory
   - `std::unique_ptr<juce::Drawable> loadSVG(String name);` using `XmlDocument` + `Drawable::createFromSVG` from BinaryData.
   - `Drawable` tint via `Drawable::replaceColour()` or set colour on `draw()`.
3) Components
   - Override `paint()` to draw shapes via `juce::Path`, `PathStrokeType`, gradients.
   - Respect tokens: radii, elevation (shadows), active/hover colours.
4) DPI/HiDPI
   - Test scaling; avoid cached bitmaps; let JUCE draw vectors per repaint.
5) QA
   - Visual test at multiple DPIs; hover/active states (Ticket 65) verified.

Affected Files
- `native/vst3/CR717/Source/ui/components/*` (new)
- `native/vst3/CR717/Source/ui/graphics/*` (new)
- `native/vst3/CR717/Resources/svg/*` (new)

Risks & Mitigations
- Excessive paints → avoid complex paths each frame; cache inexpensive paths; reuse Drawables.
- Colour mismatch → centralize theme colours; no hardcoded hex in components.

