# Architect Prompt Template (Improved)

Instruction for AI: based on the project description and best practices, prepare an implementation‑ready architecture specification.

Context:
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
- Tech stack: Node.js/Web Audio API
- Year: 2025
- Best practices (inlined below):

```md
### 1. TL;DR

* Use **AudioContext + look-ahead scheduler** for MVP timing; avoid `ScriptProcessorNode` which is deprecated. Move to **AudioWorklet** only if you hit jitter/CPU limits. ([MDN Web Docs][1])
* Gate audio start behind a **user gesture** to satisfy modern autoplay policies; show a clear Play button that calls `audioCtx.resume()`.
* Persist patterns in **localStorage** now; consider **IndexedDB** later if you add more data. localStorage is synchronous and quota-limited. ([MDN Web Docs][2])
* Target **Node.js 22 LTS** for tooling and CI; supported to 2027. ([NodeSource][3])
* Testing: render sounds in **OfflineAudioContext** for unit checks; use **Playwright** for E2E in real browsers. ([Playwright][4])
* Observability: track **Long Tasks** and custom **User Timing** marks; aim for zero >50 ms tasks during playback. ([MDN Web Docs][5])
* Security: pin deps, run **`npm audit`** in CI, and prefer hosts where you can set **COOP/COEP** headers if you later need Worklets with `SharedArrayBuffer`. ([npm Documentation][6])
* SLOs: jitter ≤2 ms at 120 BPM; 0 audio dropouts; TTI-sound ≤150 ms after gesture; 0 long tasks during playback. Validate via `getOutputTimestamp`, Performance APIs, and headless E2E runs. ([MDN Web Docs][5])
* 2025 changes relevant here: **Web Audio 1.1 TR**, **Autoplay Policy Detection**, **Vitest 4**, **Vite 7**, and tightened npm security practices. ([W3C][7])
* Keep the UI minimal, desktop+tablet responsive; no backend; no third-party audio libs.

---

### 2. Landscape — What’s new in 2025

* **Web Audio**: Spec progressed (Web Audio API 1.1 TR, Nov-2024). Use **AudioWorklet** for custom DSP; `ScriptProcessorNode` remains **deprecated** and should be avoided. ([W3C][7])
* **Autoplay**: Formalized **Autoplay Policy Detection**; Safari still requires user gestures to start sound. Plan explicit user initiation.
* **Tooling**: **Node 22 is LTS**; **Vitest 4** and **Vite 7** matured; Playwright continues to be standard for E2E. ([NodeSource][3])
* **Observability**: **Long Tasks API** and **User Timing** remain recommended for responsiveness diagnostics; LoAF provides frame-level view where supported. ([MDN Web Docs][5])
* **Security & Supply chain**: Harden npm usage; enable automated scans in CI; npm is tightening auth/token policies. ([npm Documentation][6])
* **Hosting**: If you later need `SharedArrayBuffer` or Worklet+WASM, you must be **cross-origin isolated** via **COOP**/**COEP** headers; some static hosts (e.g., GitHub Pages) don’t let you set them. ([MDN Web Docs][8])
* **Alternatives**: Sample-based drum machines are simpler but off-scope since you require synthesis; keep this as a P3 option if authenticity beats purity.

---

### 3. Architecture Patterns (Node.js tooling + Web Audio in browser)

#### Pattern A — “Main-thread Web Audio + Look-ahead Scheduler” (MVP)

**When**: Small app, ≤5 instruments, 16-step grid, Chrome/Firefox/Safari support, minimal CPU.
**Steps**

1. Build a Web Audio graph with `OscillatorNode`/`Noise` + `GainNode` + `BiquadFilterNode` per voice.
2. Implement a **look-ahead scheduler** on the main thread: run `setInterval` every ~25 ms, schedule notes into the future window (e.g., 0.1 s) using `context.currentTime`.
3. Use `getOutputTimestamp()` to align UI tick marks with actual audio time.
4. Render tests with `OfflineAudioContext`.
   **Pros**: Simple; no Worklet build step; good enough for 16-step 808 at 60–240 BPM.
   **Cons**: Main-thread jitter if UI blocks; not ideal for heavy DSP.
   **Later**: Swap scheduler to Worker/Worklet; add swing.

#### Pattern B — “AudioWorklet-driven Clock/Synthesis” (Scale-up)

**When**: You need tighter timing under UI load, add swing/shuffle, or add more voices.
**How**: Put timebase and synthesis in an **AudioWorkletProcessor**; control via `AudioWorkletNode.port.postMessage`, keep UI on main thread. ([MDN Web Docs][9])
**Migrate from A**: Move envelopes/DSP into Worklet; keep pattern editor unchanged; add COOP/COEP if using `SharedArrayBuffer`. ([MDN Web Docs][8])

---

### 4. Priority 1 — Timing & Synthesis Core

**Why** → Ensures 808-like sound and stable groove; mitigates dropouts/jitter risks.
**Scope** → In: BD/SD/CH/OH/CP synthesis, 16-step scheduler, Play/Stop, BPM. Out: swing, samples, FX.
**Decisions**

* Use **look-ahead scheduler** with 0.1 s window; tick every 25 ms; schedule with `currentTime`. Alternative: Worklet clock from start.
* User gesture to start/resume `AudioContext`.
* Avoid `ScriptProcessorNode`; prefer native nodes or Worklet. ([MDN Web Docs][1])
  **Implementation (sketch)**

1. `const ctx = new AudioContext({latencyHint:'interactive'});`
2. On Play button: `await ctx.resume(); startScheduler();` (gesture-gated).
3. Scheduler: every 25 ms, schedule steps whose time < `ctx.currentTime + scheduleAhead`.
4. Instruments: build BD/SD via oscillators + envelopes; HH/CP via filtered noise bursts (see §7 snippet). ([joesul.li][10])
5. Visuals: compute active step from `getOutputTimestamp().contextTime`.
   **Guardrails & SLOs**

* Jitter ≤2 ms @120 BPM; **0 dropouts**; first sound ≤150 ms after click; 0 long tasks during playback. ([MDN Web Docs][5])
  **Failure modes → Recovery**
* **Autoplay blocked** → show inline CTA to click Play; call `ctx.resume()` again.
* **Jitter spikes** → reduce look-ahead to 0.05–0.08 s and interval to 15 ms; if persistent, migrate to Worklet clock (Priority 2).

---

### 5. Priority 2 — Persistence & UX polish

**Why** → Patterns survive reload; minimal friction.
**Scope** → In: save/load with `localStorage`; visual step highlight; responsive layout.
**Decisions**

* Use `localStorage` key `tr808:v1:pattern`; size budget <100 KB. IndexedDB is a later migration if needed. ([MDN Web Docs][2])
  **Implementation**

1. Serialize steps as compact RLE JSON.
2. Save on change and on Stop.
3. On load, validate version and length (16).
   **Guardrails & SLOs**

* Save <10 ms; load <20 ms; no blocking during playback; keep writes outside scheduler tick.
  **Failure modes**
* Quota/full → show “Export JSON” button; allow manual download. ([MDN Web Docs][2])

---

### 6. Priority 3 — Swing/Shuffle and Optional Worklet

**Why** → Groove authenticity; timing resilience.
**Scope** → In: swing % per pair; optional migration to AudioWorklet.
**Decisions**

* Apply swing by delaying odd steps by `(swing% * stepDuration/2)`.
* If moving to Worklet, keep UI messages idempotent; version node params. ([MDN Web Docs][9])
  **Guardrails**
* Maintain jitter SLO with swing enabled; no audible flams.

---

### 7. Testing Strategy

**Unit**

* Use **OfflineAudioContext** to render each drum; assert envelope peaks and decay times. Example: check BD frequency drop over 60–100 ms.
  **Integration**
* Fire Play → verify first buffer non-silent within 150 ms after resume; sample `getOutputTimestamp`.
  **E2E**
* **Playwright** scripts click Play, toggle steps, change BPM, and visually confirm step highlighting. Run in Chromium/WebKit/Firefox. ([Playwright][4])
  **Coverage targets**
* ≥80% statements; ≥90% on scheduler and envelope helpers.
  **Frameworks**
* **Vitest 4** for unit (fast, Vite-native); **Playwright** for E2E. ([Vitest][11])

---

### 8. Observability & Operations

* **Metrics**: long-task count/duration; “play-to-first-sound” (User Timing mark/measure); average step skew vs audio clock. ([MDN Web Docs][5])
* **Logging**: scheduler tick drift and late scheduling events to `console.debug` in dev.
* **Dashboards**: simple in-app debug panel showing BPM, jitter, long-tasks in last minute.
* **Alerting**: none server-side; surface red badge if long-task rate >0 during playback.
* **DevTools**: Profiles with Performance panel and User Timing lanes. ([Chrome for Developers][12])

---

### 9. Security Best Practices

* **Dependencies**: lockfile commits, run `npm ci` in CI, and **`npm audit --production`** on every build; fail on high/critical. ([npm Documentation][6])
* **Headers**: if you later adopt AudioWorklet with `SharedArrayBuffer`/WASM, deploy on a host where you can set **COOP/COEP** for cross-origin isolation. ([MDN Web Docs][8])
* **CSP (basic)**: `default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'` for static app.
* **No secrets**: keep all state client-side; no API keys.
* **Supply chain**: beware typosquats; verify names/maintainers. ([TechRadar][13])

---

### 10. Performance & Cost

* **Budgets**: 0 long tasks >50 ms during playback; main-thread CPU <30% p95 while playing; memory <100 MB. Track via Long Tasks API. ([MDN Web Docs][5])
* **Optimization**: pre-compute envelopes; reuse nodes; avoid re-allocations inside scheduler; throttle UI updates.
* **Cost**: static hosting only; free tiers OK. If COOP/COEP needed, prefer Netlify/Vercel where headers are configurable.

---

### 11. CI/CD Pipeline

* **Node**: 22 LTS. **Vite 7** build. **Vitest 4** + **Playwright**. ([NodeSource][3])
* **Quality gates**: lint, type-check, unit, E2E.
* **Example scripts**

  ```json
  {
    "scripts": {
      "dev": "vite",
      "build": "vite build",
      "preview": "vite preview",
      "lint": "eslint .",
      "typecheck": "tsc -p tsconfig.json --noEmit",
      "test": "vitest run",
      "e2e": "playwright test",
      "ci": "npm run lint && npm run typecheck && npm run test && npm run e2e"
    }
  }
  ```
* **Netlify headers** (if Worklet+SAB later):

  ```
  /* 
    Cross-Origin-Opener-Policy: same-origin
    Cross-Origin-Embedder-Policy: credentialless
  ```

  ([MDN Web Docs][8])

---

### 12. Code Quality Standards

* **TypeScript** strict; **ESLint (flat config)**; **Prettier** `printWidth: 88`. ([Vitest][14])
* **Docs**: JSDoc for each synth module; ADRs for decisions.
* **Reviews**: small PRs, test first on synth helpers and scheduler.

---

### 13. Reading List (with dates and gists)

* **MDN Web Audio API** (Last updated: 2025-10-30) — overview, nodes, examples. ([MDN Web Docs][15])
* **Using AudioWorklet** (2025-04-07) — how to create processors. ([MDN Web Docs][9])
* **`ScriptProcessorNode` Deprecated** (2025-05-28) — don’t use; migrate. ([MDN Web Docs][1])
* **Autoplay Policy (Safari)** (2016; still applicable) — start on gesture.
* **Autoplay Policy Detection** (2025-07-03) — feature-detect.
* **Timing: Chris Wilson scheduler pattern** — look-ahead scheduling.
* **OfflineAudioContext** — headless audio render for tests.
* **Long Tasks API** (2024-10-08) — >50 ms tasks. ([MDN Web Docs][5])
* **Node 22 LTS** (2024-11-08; LTS through 2027-04) — tooling baseline. ([NodeSource][3])
* **Vite 7** (2025-06-24) and **Vitest 4** (2025-10-22) — modern build/test. ([vitejs][16])

---

### 14. Decision Log (ADR style)

* **ADR-001**: **Look-ahead scheduler** over Worklet for MVP to minimize complexity; revisit if jitter >2 ms or UI jank appears.
* **ADR-002**: **Gesture-gated start** to satisfy autoplay; no auto-start.
* **ADR-003**: **LocalStorage** now; **IndexedDB** later if patterns grow. ([MDN Web Docs][2])
* **ADR-004**: **No ScriptProcessorNode** due to deprecation. ([MDN Web Docs][1])
* **ADR-005**: **Node 22 LTS** for CI compatibility and support window. ([NodeSource][3])

---

### 15. Anti-Patterns to Avoid

* Scheduling notes exactly on UI ticks without look-ahead → audible drift. Use clock-based scheduling.
* Using `ScriptProcessorNode` → jank and deprecated API. Prefer AudioWorklet or native nodes. ([MDN Web Docs][1])
* Starting audio without a user gesture → blocked in Safari/others. Gate with a Play button.
* Writing to `localStorage` inside the scheduler callback → risk of blocking audio timing. ([MDN Web Docs][2])

---

### 16. Evidence & Citations

Key claims in this guide are backed inline. Core references: **MDN Web Docs** for APIs, **W3C TRs** for specs, **Apple/WebKit** for autoplay, **NodeSource** for Node LTS, **Playwright/Vite/Vitest** for tooling. ([MDN Web Docs][15])

---

### 17. Verification

**Self-check scripts (examples)**

* **Scheduler drift**: mark `performance.mark('tick')` at each scheduled step; compare with `getOutputTimestamp().contextTime` to ensure ≤2 ms skew. ([MDN Web Docs][17])
* **Audio health**: render BD/SD/HH with `OfflineAudioContext`, compute RMS and decay constants; fail if envelope deviates >10%.
* **Responsiveness**: attach **PerformanceObserver** for `longtask`; fail build if any occur during a 30-second playback E2E run. ([MDN Web Docs][5])
* **E2E**: Playwright clicks Play, toggles a few steps, changes BPM to extremes (60/240), confirms visual step advance matches audio ticks. ([Playwright][4])
  **Confidence**:
* Web Audio timing & APIs — **High**. ([MDN Web Docs][15])
* Autoplay requirements — **High**.
* Tooling versions — **High**. ([NodeSource][3])
* Security supply-chain guidance — **Medium** (ecosystem moves fast). ([TechRadar][13])

---

## Concrete snippets you can drop in

**BD (kick) outline**

```ts
function playBD(ctx: AudioContext, t: number) {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 });
  const gain = new GainNode(ctx, { gain: 1.0 });
  const pitchEnv = osc.frequency;

  osc.connect(gain).connect(ctx.destination);

  // Pitch drop 150→50 Hz over 0.08 s
  pitchEnv.setValueAtTime(150, t);
  pitchEnv.exponentialRampToValueAtTime(50, t + 0.08);

  // Amp env
  gain.gain.setValueAtTime(1.0, t);
  gain.gain.exponentialRampToValueAtTime(0.0001, t + 0.35);

  osc.start(t);
  osc.stop(t + 0.5);
}
```

(Technique: oscillator + pitch and amp envelopes.) ([sonoport.github.io][18])

**HH (closed) outline**

```ts
function playCH(ctx: AudioContext, t: number) {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) data[i] = Math.random() * 2 - 1;

  const src = new AudioBufferSourceNode(ctx, { buffer, loop: false });
  const hp = new BiquadFilterNode(ctx, { type: 'highpass', frequency: 8000, Q: 1 });
  const gain = new GainNode(ctx, { gain: 0.7 });

  src.connect(hp).connect(gain).connect(ctx.destination);
  gain.gain.setValueAtTime(0.7, t);
  gain.gain.exponentialRampToValueAtTime(0.0001, t + 0.07);

  src.start(t);
}
```

(Technique: filtered noise + fast decay.) ([joesul.li][10])

**Look-ahead scheduler core**

```ts
const lookAhead = 0.1;          // seconds
const scheduleEvery = 0.025;    // seconds
let nextTime = ctx.currentTime;

setInterval(() => {
  const end = ctx.currentTime + lookAhead;
  while (nextTime < end) {
    scheduleDueSteps(nextTime); // call playBD/SD/HH per pattern
    nextTime += stepDuration;   // stepDuration = 60/BPM/4
  }
}, scheduleEvery * 1000);
```

(Pattern from established Web Audio scheduling guidance.)

**Playwright sanity test**

```ts
import { test, expect } from '@playwright/test';
test('playback starts after gesture', async ({ page }) => {
  await page.goto('http://localhost:5173');
  await page.getByRole('button', { name: /play/i }).click();
  // Verify UI step highlight advances
  await expect(page.locator('[data-step="0"].active')).toBeVisible();
  await page.waitForTimeout(600);
  await expect(page.locator('[data-step="4"].active')).toBeVisible();
});
```

([Playwright][4])

---

If you want, I can export this as a README.md with the snippets wired for Vite + Vitest + Playwright.

[1]: https://developer.mozilla.org/en-US/docs/Web/API/ScriptProcessorNode "ScriptProcessorNode - Web APIs | MDN"
[2]: https://developer.mozilla.org/en-US/docs/Web/API/Storage_API/Storage_quotas_and_eviction_criteria "Storage quotas and eviction criteria - Web APIs | MDN"
[3]: https://nodesource.com/blog/Node.js-v22-Long-Term-Support-LTS/ "Node.js v22 Enters Long Term Support (LTS)"
[4]: https://playwright.dev/docs/running-tests "Running and debugging tests"
[5]: https://developer.mozilla.org/en-US/docs/Web/API/PerformanceLongTaskTiming "PerformanceLongTaskTiming - Web APIs - MDN Web Docs"
[6]: https://docs.npmjs.com/cli/v9/commands/npm-audit/ "npm-audit"
[7]: https://www.w3.org/TR/webaudio-1.1/ "Web Audio API 1.1"
[8]: https://developer.mozilla.org/en-US/docs/Web/HTTP/Reference/Headers/Cross-Origin-Embedder-Policy "Cross-Origin-Embedder-Policy (COEP) header - HTTP | MDN"
[9]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API/Using_AudioWorklet "Background audio processing using AudioWorklet - Web APIs"
[10]: https://joesul.li/van/synthesizing-hi-hats/ "Synthesizing Hi-Hats with Web Audio"
[11]: https://vitest.dev/blog/vitest-4 "Vitest 4.0 is out!"
[12]: https://developer.chrome.com/docs/lighthouse/performance/user-timings "User Timing marks and measures | Lighthouse"
[13]: https://www.techradar.com/pro/security/dangerous-npm-packages-are-targeting-developer-credentials-on-windows-linux-and-mac-heres-what-we-know "Dangerous npm packages are targeting developer credentials on Windows, Linux and Mac - here's what we know"
[14]: https://vitest.dev/config/ "Configuring Vitest"
[15]: https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API "Web Audio API - MDN Web Docs - Mozilla"
[16]: https://vite.dev/blog/announcing-vite7 "Vite 7.0 is out!"
[17]: https://developer.mozilla.org/en-US/docs/Web/API/Performance/mark "Performance: mark() method - Web APIs - MDN Web Docs"
[18]: https://sonoport.github.io/synthesising-sounds-webaudio.html "Synthesising Sounds with Web Audio API -"
```

Task:
Produce architect.md as the source of truth for implementation.

Output Structure (Markdown):
## Goals & Non‑Goals
- Goals: [1–5]
- Non‑Goals: [1–5]

## Architecture Overview
- Diagram (text): components and connections
- Data schema (high‑level)
- External integrations

## Discovery (optional, if a repo is available)
- Map structure, entry points, integration boundaries, and cross‑cutting concerns.
- Identify dead code, high‑complexity modules, and extension points (minimal change surface).
- Output a short tree of key files and where your plan plugs in.

## MCDM for Major Choices
- Criteria: PerfGain, SecRisk, DevTime, Maintainability, Cost, Scalability, DX
- Weights: justify briefly (SMART/BWM)
- Alternatives table: scores 1–9 → normalize → TOPSIS rank
- Recommendation: pick highest closeness; note trade‑offs and rollback plan

### Decision Matrix (template)
| Alternative | PerfGain | SecRisk | DevTime | Maintainability | Cost | Scalability | DX | Notes |
|-------------|----------|---------|---------|-----------------|------|------------|----|-------|
| A           |          |         |         |                 |      |            |    |       |
| B           |          |         |         |                 |      |            |    |       |
| C           |          |         |         |                 |      |            |    |       |

## Key Decisions (ADR‑style)
- [ADR‑001] Choice with rationale (alternatives, trade‑offs)
- [ADR‑002] ...

## Components
- Component A: responsibility, interfaces, dependencies
- Component B: ...

## API Contracts
- Endpoint/Function → contract (input/output, errors)
- Versioning and compatibility

## Data Model
- Models/tables: fields, keys, indexes
- Migration policies

## Quality & Operations
- Testing strategy (unit/integration/e2e/perf/security)
- Observability (metrics/logs/traces, alerts)
- Security (authn/authz, secrets, data protection)
- CI/CD (pipeline, gates, rollbacks)

## Domain Doctrine & Grounding (optional)
- Grounding sources (DBs/APIs/files) and how to cite/verify.
- Policies & prohibitions (e.g., no heuristics for routing, scraping doctrine, robots/ToS).
- Receipts/verification discipline and provenance requirements.

## Affected Modules/Files (if repo is available)
- Files to modify → short rationale.
- Files to create → paths, responsibilities, and initial signatures.

## Implementation Steps
- Numbered, observable plan with concrete function names and signatures.
- Include timeouts, retries, validation, and error shapes.

## Backlog (Tickets)
- Break the work into tickets with clear dependencies and DoD.
- File structure: `backlog/open/<nn>-<kebab>.md`
- Ticket format (each file): Title, Objective, DoD, Steps (concrete), Affected files, Tests, Risks, Dependencies.

## Interfaces & Contracts
- API endpoints/functions: input/output schemas, error shapes, versioning.
- Compatibility strategy and migration notes.

## Stop Rules & Preconditions
- Go/No‑Go prerequisites (secrets, corpora, env flags, licenses).
- Conditions to halt and escalate (security/compliance conflicts, blocked dependencies).

## SLOs & Guardrails
- SLOs: latency/throughput/error rate
- Performance/Cost budgets and limits

Requirements
1) No chain‑of‑thought. Provide final decisions with brief, verifiable reasoning.
2) Be specific to Node.js/Web Audio API and up‑to‑date for 2025; flag outdated items.