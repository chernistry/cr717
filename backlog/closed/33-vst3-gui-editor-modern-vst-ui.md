Read /Users/sasha/IdeaProjects/personal_projects/tr808-synth/.sdd/coding_rules.md first

# VST3 GUI Editor — Modern VST UI (Low-Jank, Skinnable)

Objective
- Build a modern, responsive plugin editor with low-jank rendering and a design consistent with a premium VST. Implement knobs/faders/meters, voice section, mixer strip, and global controls.

Acceptance Criteria (DoD)
- Editor renders at 60 fps on a laptop; no blocking on audio thread; UI updates decoupled.
- Controls: per-voice Level/Tune/Decay/Tone/Snappy/HH Decay; global Master; meters (peak/RMS approx).
- Styling: dark theme + tokens; scalable layout; Retina-ready. No OpenGL hard dependency (optional flag).
- Parameter binding via APVTS attachments; automation gestures correct.

Steps
1) Design tokens & layout
   - Port token system; define sizes/colors/spacing; set minimum window sizes.
2) Controls
   - Implement knob/fader widgets; smooth interaction; fine adjust with modifier.
3) Sections
   - Transport/global; voice grid; mixer/FX (FX can be disabled until DSP nodes exist).
4) Meters & performance
   - Audio thread → lock-free meter taps; UI timer ~30–60 Hz; clamp updates.
5) QA
   - Mouse/keyboard accessibility; resize behavior; high DPI checks.

Affected Files
- `native/vst3/TR808Garage/Source/PluginEditor.*`
- `native/vst3/TR808Garage/Source/ui/**`

Risks & Mitigations
- UI jank from heavy painting → cache layers where useful; avoid heavy gradients; throttle repaint.
- Accessibility gaps → ensure focus traversal and labeling for screen readers where possible.

Dependencies
- Ticket 32 (parameters/state) and 31 (DSP voices).

