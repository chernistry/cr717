Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# 55 — Status Strip & Diagnostics

Context
- Spec: `.sdd/backlog/tasks/001-rehaul-ui-func.md` (§15 Micro-interactions mentions status messages)

Goal
- Provide a bottom status strip for transient messages (sync state, DnD errors, meter overloads) with a small diagnostics overlay in Debug.

Scope
- Status area with queue + timeouts; error/warn/info styles from tokens.
- API to post messages from UI modules (DnD, Preset, Meter, Host Sync).
- Optional Debug overlay toggled by a key (e.g., F12) showing FPS, repaint rects, scale factor.

Acceptance Criteria
- Messages appear with correct severity styling and auto-dismiss.
- Developers can toggle diagnostics overlay in Debug builds.

Tasks
1) Implement `StatusStrip` component and message bus.
2) Wire DnD/preset/meter events to status messages.
3) Add diagnostics overlay with FPS and repaint rectangles.

References
- Spec §15; performance ticket §52.

