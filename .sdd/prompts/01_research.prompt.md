# Best Practices Research Template (Improved)

Instruction for AI: produce a practical, evidence‑backed best practices guide tailored to this project and stack.

---

## Project Context
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
- Tech stack: Node.js/Web Audio API
- Domain: audio-synthesis
- Year: 2025

## Task
Create a comprehensive best‑practices guide for tr808-synth that is:
1) Current — relevant to 2025; mark deprecated/outdated items.
2) Specific — tailored to Node.js/Web Audio API and audio-synthesis.
3) Practical — include concrete commands/config/code.
4) Complete — cover architecture, quality, ops, and security.

## Output Structure (Markdown)
### 1. TL;DR (≤10 bullets)
- Key decisions and patterns (why, trade‑offs, MVP vs later)
- Observability posture; Security posture; CI/CD; Performance & Cost guardrails
- What changed in 2025; SLOs summary

### 2. Landscape — What’s new in 2025
For Node.js/Web Audio API:
- Standards/framework updates; deprecations/EOL; pricing changes
- Tooling maturity: testing, observability, security
- Cloud/vendor updates
- Alternative approaches and when to choose them

### 3. Architecture Patterns (2–4 for audio-synthesis with Node.js/Web Audio API)
Pattern A — [NAME] (MVP)
- When to use; Steps; Pros/Cons; Optional later features

Pattern B — [NAME] (Scale‑up)
- When to use; Migration from A

### 4. Priority 1 — [AREA]
Why → relation to goals and mitigated risks
Scope → In/Out
Decisions → with rationale and alternatives
Implementation outline → 3–6 concrete steps
Guardrails & SLOs → metrics and limits/quotas
Failure Modes & Recovery → detection→remediation→rollback

### 5–6. Priority 2/3 — [AREA]
Repeat the structure from 4.

### 7. Testing Strategy (for Node.js/Web Audio API)
- Unit / Integration / E2E / Performance / Security
- Frameworks, patterns, coverage targets

### 8. Observability & Operations
- Metrics, Logging, Tracing, Alerting, Dashboards

### 9. Security Best Practices
- AuthN/AuthZ, Data protection (PII, encryption), Secrets, Dependency security
- OWASP Top 10 (2025) coverage; Compliance (if any)

### 10. Performance & Cost
- Budgets (concrete numbers), optimization techniques, cost monitoring, resource limits

### 11. CI/CD Pipeline
- Build/Test/Deploy; quality gates; environments

### 12. Code Quality Standards
- Style, linters/formatters, typing, docs, review, refactoring

### 13. Reading List (with dates and gists)
- [Source] (Last updated: YYYY‑MM‑DD) — gist

### 14. Decision Log (ADR style)
- [ADR‑001] [Choice] over [alternatives] because [reason]

### 15. Anti‑Patterns to Avoid
- For Node.js/Web Audio API/audio-synthesis with “what, why bad, what instead”

### 16. Evidence & Citations
- List sources inline near claims; add links; include “Last updated” dates when possible.

### 17. Verification
- Self‑check: how to validate key recommendations (scripts, smoke tests, benchmarks)
- Confidence: [High/Medium/Low] per section

## Requirements
1) No chain‑of‑thought. Provide final answers with short, verifiable reasoning.
2) If browsing is needed, state what to check and why; produce a provisional answer with TODOs.
3) Keep it implementable today; prefer defaults that reduce complexity.

## Additional Context
{{ADDITIONAL_CONTEXT}}

---
Start the research now and produce the guide for tr808-synth.