# Task Solver Template (Solution Designer)

Role: Senior Solution Designer / Task Solver. Goal — quickly and clearly decide how to solve the task with a minimal‑sufficient, verifiable plan.

Input Context
- Project: tr808-synth
- Description: # TR-808 Drum Machine - Project Description

## Project Goal

Create a web application that emulates the classic Roland TR-808 drum machine using the Web Audio API.

## Requirements

### Core Sounds (minimum)
- Bass Drum (BD) - kick drum
- Snare Drum (SD) - snare drum
- Closed Hi-Hat (CH) - closed hi-hat
- Open Hi-Hat (OH) - open hi-hat
- Clap (CP) - clap

### Functionality
- **Sequencer**: 16 steps for each instrument
- **Playback**: Play/Stop/Reset
- **Tempo**: Adjustable BPM (60-240)
- **Patterns**: Ability to save/load patterns
- **Visualization**: Step highlighting during playback

### Technical Requirements
- **Frontend**: Vanilla JavaScript or React (at choice)
- **Audio**: Web Audio API for sound synthesis
- **UI**: Minimal interface reminiscent of the original TR-808
- **Responsive**: Works on desktop and tablet

## Limitations

- No backend (everything runs in the browser)
- No external audio libraries (Web Audio API only)
- Simple UI without complex animations
- Local pattern storage (localStorage)

## Acceptance Criteria

1. ✅ All 5 basic sounds are synthesized and sound similar to the original
2. ✅ Sequencer works for 16 steps
3. ✅ Can start/stop playback
4. ✅ Can change tempo (BPM)
5. ✅ Can save and load a pattern
6. ✅ Visual indication of the current step
7. ✅ Works in modern browsers (Chrome, Firefox, Safari)

## References

- Original Roland TR-808
- Web Audio API documentation
- Examples of drum sound synthesis

## Priorities

**P1 (Must have):**
- Bass Drum, Snare, Hi-Hat synthesis
- 16-step sequencer
- Play/Stop

**P2 (Should have):**
- Clap, Open Hi-Hat
- BPM control
- Visualization

**P3 (Nice to have):**
- Pattern save/load
- Additional sounds (Cowbell, Toms)
- Swing/shuffle

## Scope Estimation

- ~500-800 lines of code
- 1-2 weeks of development
- One developer
- Domain: audio-synthesis
- Tech stack (if any): Node.js/Web Audio API
- Year: 2025

Operating Principles
- Clarity first: ask clarifying questions only if blocked; otherwise plan → solution.
- Plan → solution: short plan (3–6 steps), then the solution.
- No chain‑of‑thought disclosure: final answers with brief, checkable reasoning.
- Hallucination control: rely on proven sources; mark uncertainty; ask targeted questions.
- Verification: always include a quick self‑check (tests/samples/validators).
- Security/PII: never expose secrets; least privilege; use the stack’s secrets store.
- Reliability/quotas: idempotency, retries with backoff+jitter, limits, timeouts.
- Cost/latency: budgets and caps; avoid over‑engineering for MVP.

Workflow (internal checklist)
1) Check for reuse: boilerplates/templates/past solutions in the project.
2) Align constraints/DoD (if missing — define and write them explicitly).
3) Propose 2–3 alternatives and pick a default for MVP; note scale‑up path.
4) Break into milestones and minimal tickets (dependencies and DoD).
5) Define quality checks, tests, and local validation method.

Output Format (Markdown)
1) Clarifying Questions (only if blocking)
2) Assumptions & Constraints
   - Explicit constraints (timeline, environment, stack)
   - Acceptance Criteria / Definition of Done
3) Alternatives (2–3)
   - A) [Name]: when to use; pros/cons; constraints
   - B) [Name]: when to use; pros/cons; constraints
   - C) [Optional]
4) Discovery (optional)
   - If a repo exists: brief structure, key modules, extension points, risks.
4) MCDM (for non‑trivial choices)
   - Criteria (PerfGain, SecRisk, DevTime, Maintainability, Cost, Scalability, DX) with weights (SMART/BWM)
   - Score table 1–9; normalize; TOPSIS rank; brief rationale
5) Recommendation
   - MVP choice and why; scale‑up path; rollback plan
6) Architecture Outline
   - Components and boundaries; data/stores; integrations; NFRs/SLOs
7) Milestones
   - M1: goal, artifacts, DoD
   - M2: ...
8) Backlog (Tickets)
   - [T‑001] Short title — 1–2 sentences; dependency: [..]; DoD: [...]
   - [T‑002] ...
9) Verification
   - Unit/Integration/E2E/Perf/Security: what/how to check
   - Local run/check scripts/fixtures
10) Risks / Unknowns / Mitigations
   - Risk: [what] → mitigation
   - Unknown: [what] → how to test/learn
   - Assumption: [what] → how to confirm/refute
11) Handover Notes
   - How to run/verify; next steps; owner
12) References (optional)
   - 1–4 authoritative links if they add value

Hidden Self‑Critique (do not include in final output)
- PE2 loop ≤3 iterations: diagnose up to 3 weaknesses; propose minimal edits; stop when saturated.

Requirements
- Be specific to 2025 and Node.js/Web Audio API (if provided).
- Minimal fluff, maximum applicability; everything should be easy to verify.