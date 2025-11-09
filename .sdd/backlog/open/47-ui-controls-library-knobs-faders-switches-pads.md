Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 47 — Controls Library: Knobs, Faders, Switches, Buttons, Pads

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§5 Controls, §15 Micro-interactions)
- Best practices: `.sdd/best_practices_vst.md` (input handling, focus, accessibility)

Goal
- Implement a cohesive control set (Knob, Fader, Toggle, Button, Step Pad, Picker) with tokenized visuals and premium micro-interactions.

Scope
- Knob: concentric arc, tick marks (major/minor), value bubble, double-click reset, Shift fine.
- Fader: vertical, 14–18 px track, end stops, numeric tooltip.
- Toggle/Button: pressed/hover/disabled states; focus ring.
- Step Pad: 24–32 px adaptive, LED accent; supports accent/velocity mode.
- Picker: banks (A–H), preset list, pattern operations.
- Accessibility: keyboard nav with consistent order and ARIA-like descriptions via JUCE.

Acceptance Criteria
- Controls render consistently across DPI and themes using tokens.
- All gestures supported: hover, drag, wheel, double-click reset, Ctrl/Cmd numeric entry, Shift fine.
- Focus traversal works; screen reader descriptions present via JUCE Accessibility.
- No jank during high-frequency updates; repaints are minimal.

Tasks
1) Implement controls in `src/ui/controls/*` using `LookAndFeelCR717`.
2) Add micro-interactions per spec (§15) and tooltips.
3) Add demo harness component to sanity-check each control.
4) Document usage patterns and style guidelines.

References
- Spec §5/§15; JUCE Accessibility; coding rules.

