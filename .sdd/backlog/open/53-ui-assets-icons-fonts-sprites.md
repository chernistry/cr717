Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 53 — UI Assets: Icons, Fonts, Sprites (@1x/@2x/@3x)

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§13 Visual language, §21 Artifacts)
- Apple HIG: Retina assets

Goal
- Prepare and integrate a curated asset set (SVG/PNG) with proper scaling and caching.

Scope
- Icons: vector where possible; raster fallbacks at @1x/@2x/@3x.
- Fonts: system sans stack with fallback; license-safe.
- Sprite sheets where beneficial; caching and reuse.
- Asset loader tied to scale factor; no blurring.

Acceptance Criteria
- Assets render crisp at common DPIs; no runtime file I/O on audio thread.
- Centralized asset manager caches and invalidates on DPI changes.

Tasks
1) Create `resources/ui/icons/*` and generate scaled assets.
2) Implement `AssetManager` with DPI-aware loading and caching.
3) Replace placeholder icons with new set.
4) Document asset conventions.

References
- Spec §13/§21; Apple HIG scaling guidance.

