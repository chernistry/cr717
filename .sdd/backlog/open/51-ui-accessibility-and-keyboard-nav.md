Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 51 — Accessibility & Keyboard Navigation (JUCE)

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§9 Accessibility, §15 Micro-interactions)
- Best practices: `.sdd/best_practices_vst.md` (focus rings, minimum targets ≥ 28 px)

Goal
- Ensure full keyboard navigation, focus order, descriptive accessibility nodes, and WCAG AA contrast in the plugin UI.

Scope
- Focus order across top bar → voices → mixer/FX → status.
- Focus ring visuals per tokens; hover/focus states distinct.
- Accessibility: `juce::AccessibilityHandler` descriptions, roles, values.
- Input sizes: minimum hit targets ≥ 28 px logical.

Acceptance Criteria
- All controls reachable by keyboard; focus rings visible and consistent.
- Screen reader exposes names/roles/values for controls.
- Contrast checks pass AA for text and critical UI.

Tasks
1) Implement focus traversal policy and ring drawing.
2) Add accessibility handlers for custom controls.
3) Add contrast checks in dev mode (debug overlay / screenshot QA).
4) Update developer docs: accessibility guidelines.

References
- Spec §9/§15; JUCE Accessibility docs; Apple HIG.

