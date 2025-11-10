# 65 — UI: Effects & Animations (Shadows, Blur, Transitions, Hover)

Read `.sdd/architect.md`, `.sdd/best_practices_vst.md`, and coding rules first.

Context
- Add modern visual polish: drop shadows, background blur/glow, smooth transitions, hover states, and `juce::ComponentAnimator` micro-animations. Keep GPU/CPU cost low.

Dependencies
- Ticket 63 (LookAndFeel/theme) and 64 (vector components).

Scope
- Shadows: use `juce::DropShadower`/`juce::DropShadow` for panels/popups.
- Blur/Glow: `juce::GlowEffect` for soft background blur where applicable.
- Transitions: `juce::ComponentAnimator` for position/alpha/scale micro-animations.
- Hover states: per-component hover tint/scale with timers for 120–180 ms easing.

Acceptance Criteria
- Panels and popups render soft shadows consistent with elevation tokens.
- Hover on buttons/knobs/faders shows subtle tint/scale; transitions are smooth.
- Opening/closing popovers animates (alpha/position) within ~150 ms.
- No jank or long tasks during animations on typical machine.

Implementation Steps
1) Effects helpers
   - `native/vst3/CR717/Source/ui/effects/Effects.h/.cpp`: wrappers for DropShadow/Glow with tokenized parameters (blur radius, colour, offset).
2) Hover behaviours
   - Extend `VectorKnob`/`VectorFader` to handle `mouseEnter/Exit`, animate via `ComponentAnimator` (alpha/scale 0.98→1.0) and tint change.
3) Transitions
   - Common `animateIn/Out(Component&)` utility for dialogs/popovers; 150 ms ease-in-out.
4) Performance
   - Avoid per-frame heap; reuse animators; minimal repaint region; no heavy effects on rapid repaint components.
5) QA
   - Manual checks: shadows aligned, blur subtle, no flicker; measure repaint count in Debug.

Affected Files
- `native/vst3/CR717/Source/ui/effects/*` (new)
- `native/vst3/CR717/Source/ui/components/*` (updates)
- `native/vst3/CR717/Source/ui/editor/*` (integration)

Risks & Mitigations
- Overdraw cost → limit blur to static backdrops; prefer shadows over blur where possible.
- Accessibility → keep motion subtle; allow “reduced motion” flag in theme.

