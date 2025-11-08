# Solution Ideation Template (Improved)

Instruction for AI: craft a realistic solution and break it into a concise, verifiable plan without excessive deliberation.

Input:
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

Operating principles:
- Brevity and applicability: minimal fluff, maximum concrete steps.
- Plan → solution: start with a short plan (3–6 steps), then details.
- Hallucination control: flag uncertainty, propose checks.
- Verifiability: always add self‑check and Definition of Done.

Task:
1) Provide 2–3 solution alternatives with comparisons and trade‑offs.
2) Pick a default for MVP with rationale; outline scale‑up path.
3) Draft the Architecture Outline (components, data, integrations, NFRs/SLOs).
4) Split into Milestones and Backlog with brief DoD and dependencies.
5) List Risks/Unknowns/Assumptions and how to test them.

Output format (Markdown):
## Clarifying Questions (only if blocking)

## Assumptions & Constraints
- Acceptance Criteria / Definition of Done: [...]
- Constraints: timeline, environment, stack

## Alternatives
- A) [Name]: when; pros/cons; constraints
- B) [Name]: when; pros/cons; constraints
- C) [Optional]
## Discovery (optional)
- If a repo exists: brief structure, key modules, extension points, risks.

## MCDM (optional but recommended)
- Criteria (PerfGain, SecRisk, DevTime, Maintainability, Cost, Scalability, DX) with weights (SMART/BWM)
- Score table 1–9; normalize; TOPSIS rank; brief rationale

## Recommendation
- MVP choice and why; scale‑up path; rollback plan

## Architecture Outline
- Components: [Component → purpose]
- Data/Stores: [models/tables/indexes]
- Integrations: [services, protocols]
- NFRs: SLOs/latency/throughput/availability

## Milestones
- M1: [goal, artifacts, DoD]
- M2: ...

## Backlog (Tickets)
- [T‑001] Short title — 1–2 sentences; dependency: [..]; DoD: [...]
- [T‑002] ...

## Verification
- Unit/Integration/E2E/Perf/Security: what/how to check
- Local checks with sample inputs/outputs
 - Token/latency/cost budgets (if LLM involved)

## Risks / Unknowns / Assumptions
- Risk: [what] → mitigation
- Unknown: [what] → how to test
- Assumption: [what] → how to confirm/refute

## References (optional)
- 1–4 authoritative links if they add value

Requirements:
- Be specific to 2025 with concrete steps and DoD; avoid genericities.
- Adapt examples and solutions to Node.js/Web Audio API (if provided).