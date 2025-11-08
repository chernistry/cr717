# Agent Prompt Template

You are the Implementing Agent (CLI/IDE). Work strictly from specifications.

Project Context:
- Project: tr808-synth
- Stack: Node.js/Web Audio API
- Domain: audio-synthesis
- Year: 2025

Inline attachments to read:

## Project Description
```md
# TR-808 Drum Machine - Project Description

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
```

## Best Practices
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

## Architecture Spec
```md
# TR-808 Drum Machine - Architecture Specification

**Project:** tr808-synth  
**Domain:** audio-synthesis  
**Stack:** Node.js/Web Audio API  
**Year:** 2025

---

## Goals & Non-Goals

### Goals
1. **Authentic 808 sound** - Synthesize BD, SD, CH, OH, CP using Web Audio API oscillators and noise
2. **Stable timing** - 16-step sequencer with <2ms jitter at 60-240 BPM
3. **Browser-native** - No backend, no external audio libraries, works offline
4. **Responsive UI** - Desktop + tablet, visual step highlighting during playback
5. **Pattern persistence** - Save/load patterns via localStorage

### Non-Goals
1. **Sample-based playback** - Pure synthesis only (no .wav files)
2. **Backend/cloud sync** - Fully client-side application
3. **Mobile optimization** - Focus on desktop/tablet (mobile is P3)
4. **Advanced effects** - No reverb, delay, compression (keep it minimal)
5. **Multi-pattern sequencing** - Single pattern at a time (no song mode)

---

## Architecture Overview

### High-Level Diagram
```
┌─────────────────────────────────────────────────────────────┐
│                        Browser                               │
│  ┌───────────────────────────────────────────────────────┐  │
│  │                    UI Layer                            │  │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐           │  │
│  │  │ Sequencer│  │ Controls │  │ Pattern  │           │  │
│  │  │   Grid   │  │ (BPM/etc)│  │ Manager  │           │  │
│  │  └────┬─────┘  └────┬─────┘  └────┬─────┘           │  │
│  └───────┼─────────────┼─────────────┼──────────────────┘  │
│          │             │             │                      │
│  ┌───────▼─────────────▼─────────────▼──────────────────┐  │
│  │              Application State                        │  │
│  │  - Current pattern (16x5 grid)                       │  │
│  │  - Playback state (playing/stopped)                  │  │
│  │  - BPM, current step                                 │  │
│  └───────┬──────────────────────────┬───────────────────┘  │
│          │                          │                      │
│  ┌───────▼──────────┐      ┌────────▼──────────────────┐  │
│  │   Scheduler      │      │   Storage Manager         │  │
│  │  (Look-ahead)    │      │   (localStorage)          │  │
│  └───────┬──────────┘      └───────────────────────────┘  │
│          │                                                 │
│  ┌───────▼─────────────────────────────────────────────┐  │
│  │            Audio Engine (Web Audio API)             │  │
│  │  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐  ┌──────┐ │  │
│  │  │  BD  │  │  SD  │  │  CH  │  │  OH  │  │  CP  │ │  │
│  │  │Voice │  │Voice │  │Voice │  │Voice │  │Voice │ │  │
│  │  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘  └──┬───┘ │  │
│  │     └─────────┴─────────┴─────────┴─────────┘      │  │
│  │                    AudioContext                     │  │
│  │                    (destination)                    │  │
│  └─────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────────┘
```

### Data Schema
```typescript
// Pattern data structure
interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[16];  // Bass Drum
    SD: boolean[16];  // Snare Drum
    CH: boolean[16];  // Closed Hi-Hat
    OH: boolean[16];  // Open Hi-Hat
    CP: boolean[16];  // Clap
  };
}

// Playback state
interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  nextNoteTime: number;
  scheduleAheadTime: number; // 0.1s
}
```

### External Integrations
- **None** - Fully self-contained browser application
- **localStorage** - Browser API for pattern persistence (quota: ~5-10MB)

---

## MCDM for Major Choices

### Decision: Timing Architecture

**Criteria & Weights** (BWM method):
- **PerfGain** (0.25) - Timing accuracy and jitter reduction
- **DevTime** (0.20) - Implementation complexity
- **Maintainability** (0.20) - Code clarity and debuggability
- **SecRisk** (0.10) - Security implications
- **Cost** (0.10) - Hosting/runtime costs
- **Scalability** (0.10) - Future expansion capability
- **DX** (0.05) - Developer experience

**Alternatives:**

| Alternative | PerfGain | DevTime | Maintainability | SecRisk | Cost | Scalability | DX | Weighted Score |
|-------------|----------|---------|-----------------|---------|------|-------------|----|----|
| **A) Main-thread Look-ahead** | 7 | 9 | 8 | 9 | 9 | 6 | 8 | **7.85** ✅ |
| B) AudioWorklet Clock | 9 | 5 | 6 | 9 | 8 | 9 | 5 | 7.35 |
| C) Web Worker Scheduler | 8 | 6 | 7 | 9 | 9 | 7 | 6 | 7.45 |

**Recommendation:** **Alternative A - Main-thread Look-ahead Scheduler**

**Rationale:**
- **Sufficient for MVP**: 16-step 808 at 240 BPM = ~156ms per step, well within main-thread capabilities
- **Simpler implementation**: No Worklet build complexity, easier debugging
- **Good enough timing**: <2ms jitter achievable with 25ms tick interval + 100ms look-ahead
- **Migration path**: Can upgrade to Worklet (Alt B) if jitter becomes issue

**Trade-offs:**
- ✅ Faster to implement and test
- ✅ No COOP/COEP header requirements
- ⚠️ Vulnerable to UI blocking (mitigated by keeping UI lightweight)
- ⚠️ Less suitable for >16 steps or complex DSP (not in scope)

**Rollback plan:** If jitter >2ms observed in production, migrate scheduler to Web Worker (Alt C) first, then Worklet (Alt B) if needed.

---

## Key Decisions (ADR-style)

### [ADR-001] Look-ahead Scheduler on Main Thread
**Context:** Need stable timing for 16-step sequencer at 60-240 BPM.  
**Decision:** Use main-thread `setInterval` with 100ms look-ahead window.  
**Alternatives:** AudioWorklet clock, Web Worker scheduler.  
**Rationale:** Simpler for MVP, sufficient timing accuracy, easier debugging.  
**Consequences:** Must keep UI lightweight to avoid blocking scheduler.

### [ADR-002] Pure Synthesis (No Samples)
**Context:** Need authentic 808 sounds.  
**Decision:** Synthesize all sounds using Web Audio API oscillators and noise.  
**Alternatives:** Use sample playback, hybrid approach.  
**Rationale:** Meets project requirement, smaller bundle size, more authentic to original 808 analog circuits.  
**Consequences:** More complex synthesis code, requires careful envelope tuning.

### [ADR-003] localStorage for Persistence
**Context:** Need to save/load patterns.  
**Decision:** Use localStorage with JSON serialization.  
**Alternatives:** IndexedDB, cloud storage, file download/upload.  
**Rationale:** Simplest API, synchronous, sufficient for single pattern (~1KB).  
**Consequences:** 5-10MB quota limit (not an issue), no cross-device sync.

### [ADR-004] Vanilla JavaScript (No Framework)
**Context:** Need responsive UI for sequencer grid.  
**Decision:** Use vanilla JS with direct DOM manipulation.  
**Alternatives:** React, Vue, Svelte.  
**Rationale:** Minimal bundle size, no build complexity, sufficient for simple UI.  
**Consequences:** More manual DOM updates, but UI is simple enough.

### [ADR-005] Gesture-Gated Audio Start
**Context:** Browser autoplay policies require user interaction.  
**Decision:** Show Play button that calls `audioContext.resume()`.  
**Alternatives:** Auto-start on page load (blocked), muted start.  
**Rationale:** Required by Safari/Chrome autoplay policies.  
**Consequences:** User must click Play before hearing sound (expected UX).

---

## Components

### 1. Audio Engine (`src/audio/`)
**Responsibility:** Web Audio API management, sound synthesis, timing.

**Modules:**
- `audioContext.ts` - AudioContext singleton, initialization
- `voices/` - Individual drum voice synthesizers
  - `bassDrum.ts` - BD synthesis (oscillator + pitch envelope)
  - `snareDrum.ts` - SD synthesis (oscillator + noise + filter)
  - `closedHiHat.ts` - CH synthesis (filtered noise burst)
  - `openHiHat.ts` - OH synthesis (longer filtered noise)
  - `clap.ts` - CP synthesis (noise bursts with delay)
- `scheduler.ts` - Look-ahead scheduler, step timing
- `types.ts` - Audio-related TypeScript interfaces

**Interfaces:**
```typescript
interface Voice {
  play(context: AudioContext, time: number): void;
}

interface Scheduler {
  start(bpm: number, pattern: Pattern): void;
  stop(): void;
  getCurrentStep(): number;
}
```

**Dependencies:** Web Audio API (browser native)

---

### 2. UI Layer (`src/ui/`)
**Responsibility:** User interface, event handling, visual feedback.

**Modules:**
- `sequencerGrid.ts` - 16x5 step grid, click handlers
- `controls.ts` - Play/Stop, BPM slider, pattern buttons
- `visualizer.ts` - Current step highlighting
- `dom.ts` - DOM utilities, element creation

**Interfaces:**
```typescript
interface SequencerGrid {
  render(pattern: Pattern): void;
  onStepToggle(callback: (instrument: string, step: number) => void): void;
  highlightStep(step: number): void;
}
```

**Dependencies:** DOM API

---

### 3. State Management (`src/state/`)
**Responsibility:** Application state, pattern data, playback state.

**Modules:**
- `store.ts` - Central state store (simple observable)
- `pattern.ts` - Pattern data structure and operations
- `playback.ts` - Playback state management

**Interfaces:**
```typescript
interface Store {
  getState(): AppState;
  setState(partial: Partial<AppState>): void;
  subscribe(listener: (state: AppState) => void): () => void;
}
```

---

### 4. Storage Manager (`src/storage/`)
**Responsibility:** Pattern persistence via localStorage.

**Modules:**
- `localStorage.ts` - Save/load patterns
- `serialization.ts` - JSON serialization/deserialization

**Interfaces:**
```typescript
interface StorageManager {
  savePattern(pattern: Pattern): void;
  loadPattern(): Pattern | null;
  clearPattern(): void;
}
```

---

## API Contracts

### Audio Engine API
```typescript
// Initialize audio context (call on user gesture)
function initAudio(): Promise<AudioContext>

// Play individual voice
function playVoice(
  voice: 'BD' | 'SD' | 'CH' | 'OH' | 'CP',
  context: AudioContext,
  time: number
): void

// Scheduler control
function startScheduler(
  context: AudioContext,
  bpm: number,
  pattern: Pattern,
  onStep: (step: number) => void
): void

function stopScheduler(): void
```

### Storage API
```typescript
// Save pattern to localStorage
function savePattern(pattern: Pattern): void
// Returns: void
// Throws: QuotaExceededError if storage full

// Load pattern from localStorage
function loadPattern(): Pattern | null
// Returns: Pattern or null if not found
// Throws: SyntaxError if corrupted data
```

---

## Data Model

### Pattern Model
```typescript
interface Pattern {
  name: string;           // Pattern name
  bpm: number;            // 60-240
  steps: {
    BD: boolean[16];
    SD: boolean[16];
    CH: boolean[16];
    OH: boolean[16];
    CP: boolean[16];
  };
}

// Default empty pattern
const EMPTY_PATTERN: Pattern = {
  name: 'Untitled',
  bpm: 120,
  steps: {
    BD: Array(16).fill(false),
    SD: Array(16).fill(false),
    CH: Array(16).fill(false),
    OH: Array(16).fill(false),
    CP: Array(16).fill(false),
  }
};
```

### localStorage Schema
```
Key: "tr808:v1:pattern"
Value: JSON.stringify(Pattern)
Size: ~1KB per pattern
```

**Migration Policy:** Version prefix (`v1`) allows future schema changes. On load, check version and migrate if needed.

---

## Quality & Operations

### Testing Strategy

**Unit Tests** (Vitest)
- Voice synthesis: render with OfflineAudioContext, verify envelope shapes
- Scheduler: mock time, verify step advancement
- Storage: save/load round-trip, quota handling
- Pattern operations: toggle steps, BPM validation

**Integration Tests**
- Audio + Scheduler: verify first sound within 150ms of start
- UI + State: click step → state updates → UI reflects change

**E2E Tests** (Playwright)
- Load page → click Play → verify audio starts
- Toggle steps → verify visual feedback
- Change BPM → verify tempo changes
- Save/load pattern → verify persistence

**Performance Tests**
- Long Tasks: 0 tasks >50ms during playback
- Jitter: measure step timing variance <2ms
- Memory: <100MB during 5min playback

### Observability

**Metrics:**
- `scheduler.jitter` - Step timing variance (ms)
- `scheduler.drift` - Cumulative drift from ideal time (ms)
- `longtasks.count` - Number of >50ms tasks during playback
- `audio.latency` - Time from click to first sound (ms)

**Logging:**
- Console debug logs for scheduler ticks (dev only)
- Error logs for audio context failures
- Warning logs for localStorage quota issues

**Monitoring:**
- In-app debug panel showing real-time jitter/drift
- Performance.mark/measure for key operations

### Security

**Threats & Mitigations:**
- **XSS**: No user-generated content, static HTML
- **Supply chain**: Lock dependencies, `npm audit` in CI
- **Data exfiltration**: No network requests, fully offline
- **localStorage poisoning**: Validate JSON on load, catch errors

**CSP Header:**
```
Content-Security-Policy: default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'
```

### CI/CD Pipeline

**Build:**
```bash
npm run build  # Vite build
```

**Quality Gates:**
```bash
npm run lint        # ESLint
npm run typecheck   # TypeScript
npm run test        # Vitest unit tests
npm run e2e         # Playwright E2E tests
```

**Deployment:**
- Static hosting (Netlify/Vercel/GitHub Pages)
- No server-side logic required
- Atomic deploys (no downtime)

---

## Stop Rules & Preconditions

### Go Preconditions
✅ Modern browser (Chrome 90+, Firefox 88+, Safari 14+)  
✅ JavaScript enabled  
✅ Web Audio API support  
✅ localStorage available (not in private mode)

### No-Go Conditions
❌ Web Audio API not supported → Show error message  
❌ localStorage disabled → Disable save/load, show warning  
❌ Autoplay blocked → Require user gesture (expected)

### Halt & Escalate If:
- Jitter consistently >5ms → Investigate scheduler, consider Worklet migration
- Memory leak detected (>200MB) → Profile and fix before release
- Audio glitches/dropouts → Check for blocking operations in scheduler

---

## SLOs & Guardrails

### Service Level Objectives

| Metric | Target | Measurement |
|--------|--------|-------------|
| **Timing Jitter** | ≤2ms @ 120 BPM | `getOutputTimestamp()` vs scheduled time |
| **Audio Latency** | ≤150ms | Time from Play click to first sound |
| **Long Tasks** | 0 during playback | PerformanceObserver('longtask') |
| **Memory** | <100MB | performance.memory (Chrome) |
| **Load Time** | <2s on 3G | Lighthouse |

### Performance Budgets
- **Bundle size**: <50KB gzipped
- **Main thread CPU**: <30% p95 during playback
- **Audio dropouts**: 0 per session
- **localStorage**: <10KB per pattern

### Cost Guardrails
- **Hosting**: Free tier (static hosting)
- **No runtime costs**: Fully client-side

---

# TR-808 Architecture - Implementation Plan & Backlog

## Affected Modules/Files

### Files to Create

**Core Structure:**
```
src/
├── main.ts                    # Entry point, initialization
├── audio/
│   ├── audioContext.ts        # AudioContext singleton
│   ├── scheduler.ts           # Look-ahead scheduler
│   ├── types.ts               # Audio interfaces
│   └── voices/
│       ├── bassDrum.ts        # BD synthesis
│       ├── snareDrum.ts       # SD synthesis
│       ├── closedHiHat.ts     # CH synthesis
│       ├── openHiHat.ts       # OH synthesis
│       └── clap.ts            # CP synthesis
├── ui/
│   ├── sequencerGrid.ts       # 16x5 grid UI
│   ├── controls.ts            # Play/Stop/BPM controls
│   ├── visualizer.ts          # Step highlighting
│   └── dom.ts                 # DOM utilities
├── state/
│   ├── store.ts               # State management
│   ├── pattern.ts             # Pattern data structure
│   └── playback.ts            # Playback state
└── storage/
    ├── localStorage.ts        # Pattern persistence
    └── serialization.ts       # JSON serialization

tests/
├── unit/
│   ├── voices.test.ts         # Voice synthesis tests
│   ├── scheduler.test.ts      # Scheduler tests
│   └── storage.test.ts        # Storage tests
└── e2e/
    └── playback.spec.ts       # Playwright E2E tests

index.html                     # Main HTML
styles.css                     # Minimal CSS
vite.config.ts                 # Vite configuration
tsconfig.json                  # TypeScript config
playwright.config.ts           # Playwright config
```

---

## Implementation Steps

### Phase 1: Foundation (Days 1-2)
1. **Setup project structure** - Vite + TypeScript + Vitest + Playwright
2. **Create AudioContext singleton** - Initialize on user gesture
3. **Implement basic scheduler** - Look-ahead with 100ms window
4. **Create Pattern data structure** - TypeScript interfaces

### Phase 2: Audio Engine (Days 3-5)
5. **Implement BD voice** - Oscillator + pitch envelope (150→50Hz)
6. **Implement SD voice** - Oscillator + noise + filter
7. **Implement CH voice** - Filtered noise burst (70ms decay)
8. **Implement OH voice** - Longer filtered noise (200ms decay)
9. **Implement CP voice** - Multiple noise bursts with delay
10. **Wire voices to scheduler** - Play on scheduled steps

### Phase 3: UI (Days 6-7)
11. **Create sequencer grid** - 16x5 buttons, toggle on click
12. **Create controls** - Play/Stop button, BPM slider
13. **Implement step highlighting** - Visual feedback during playback
14. **Connect UI to state** - Two-way data binding

### Phase 4: Persistence (Day 8)
15. **Implement localStorage save/load** - JSON serialization
16. **Add pattern name input** - Editable pattern name
17. **Handle quota errors** - Graceful degradation

### Phase 5: Testing & Polish (Days 9-10)
18. **Write unit tests** - Voices, scheduler, storage
19. **Write E2E tests** - Playwright playback scenarios
20. **Performance optimization** - Profile and fix bottlenecks
21. **Cross-browser testing** - Chrome, Firefox, Safari

---

## Backlog (Tickets)

### Ticket 01: Project Setup & Foundation
**File:** `backlog/open/01-project-setup.md`

**Objective:** Initialize project with Vite, TypeScript, and testing tools.

**Acceptance Criteria:**
- ✅ Vite project created with TypeScript
- ✅ Vitest configured for unit tests
- ✅ Playwright configured for E2E tests
- ✅ ESLint + Prettier configured
- ✅ `npm run dev` starts dev server
- ✅ `npm run build` produces optimized bundle
- ✅ `npm run test` runs unit tests
- ✅ `npm run e2e` runs E2E tests

**Steps:**
1. `npm create vite@latest tr808-synth -- --template vanilla-ts`
2. Install dependencies: `vitest`, `@playwright/test`, `eslint`, `prettier`
3. Create config files: `vite.config.ts`, `vitest.config.ts`, `playwright.config.ts`
4. Setup project structure: `src/`, `tests/`, `public/`
5. Create basic `index.html` with canvas/grid placeholder

**Affected Files:**
- `package.json` - Dependencies and scripts
- `vite.config.ts` - Vite configuration
- `tsconfig.json` - TypeScript strict mode
- `playwright.config.ts` - E2E test configuration
- `.eslintrc.json` - Linting rules

**Tests:**
- Verify `npm run dev` starts without errors
- Verify `npm run build` produces `dist/` folder
- Verify `npm test` runs (even if no tests yet)

**Risks:**
- Dependency conflicts → Use exact versions from package-lock.json

**Dependencies:** None

---

### Ticket 02: AudioContext & Scheduler Core
**File:** `backlog/open/02-audio-scheduler.md`

**Objective:** Implement AudioContext singleton and look-ahead scheduler.

**Acceptance Criteria:**
- ✅ AudioContext initializes on user gesture
- ✅ Scheduler ticks every 25ms
- ✅ Schedules notes 100ms ahead
- ✅ Tracks current step (0-15)
- ✅ Jitter <2ms measured via `getOutputTimestamp()`

**Steps:**
1. Create `src/audio/audioContext.ts`:
   ```typescript
   let audioContext: AudioContext | null = null;
   export async function initAudio(): Promise<AudioContext> {
     if (!audioContext) {
       audioContext = new AudioContext({ latencyHint: 'interactive' });
       await audioContext.resume();
     }
     return audioContext;
   }
   ```

2. Create `src/audio/scheduler.ts`:
   ```typescript
   export class Scheduler {
     private intervalId: number | null = null;
     private nextNoteTime: number = 0;
     private currentStep: number = 0;
     private scheduleAheadTime: number = 0.1;
     
     start(ctx: AudioContext, bpm: number, onStep: (step: number, time: number) => void) {
       const stepDuration = 60 / bpm / 4; // 16th notes
       this.nextNoteTime = ctx.currentTime;
       
       this.intervalId = window.setInterval(() => {
         const end = ctx.currentTime + this.scheduleAheadTime;
         while (this.nextNoteTime < end) {
           onStep(this.currentStep, this.nextNoteTime);
           this.nextNoteTime += stepDuration;
           this.currentStep = (this.currentStep + 1) % 16;
         }
       }, 25);
     }
     
     stop() {
       if (this.intervalId) clearInterval(this.intervalId);
     }
   }
   ```

3. Create unit test in `tests/unit/scheduler.test.ts`
4. Measure jitter using `performance.mark()` and `getOutputTimestamp()`

**Affected Files:**
- `src/audio/audioContext.ts` (new)
- `src/audio/scheduler.ts` (new)
- `src/audio/types.ts` (new) - TypeScript interfaces
- `tests/unit/scheduler.test.ts` (new)

**Tests:**
- Unit: Mock time, verify step advancement
- Unit: Verify jitter calculation
- Integration: Start/stop scheduler, verify no memory leaks

**Risks:**
- Autoplay blocked → Mitigated by user gesture requirement
- Jitter >2ms → Profile and optimize interval timing

**Dependencies:** Ticket 01

---

### Ticket 03: Bass Drum (BD) Voice
**File:** `backlog/open/03-voice-bass-drum.md`

**Objective:** Implement BD synthesis with pitch envelope.

**Acceptance Criteria:**
- ✅ BD plays on trigger with authentic 808 sound
- ✅ Pitch drops from 150Hz to 50Hz over 80ms
- ✅ Amplitude envelope: attack 0ms, decay 350ms
- ✅ Unit test verifies envelope shape in OfflineAudioContext

**Steps:**
1. Create `src/audio/voices/bassDrum.ts`:
   ```typescript
   export function playBD(ctx: AudioContext, time: number): void {
     const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 });
     const gain = new GainNode(ctx, { gain: 1.0 });
     
     osc.connect(gain).connect(ctx.destination);
     
     // Pitch envelope: 150Hz → 50Hz over 80ms
     osc.frequency.setValueAtTime(150, time);
     osc.frequency.exponentialRampToValueAtTime(50, time + 0.08);
     
     // Amplitude envelope: 1.0 → 0.0001 over 350ms
     gain.gain.setValueAtTime(1.0, time);
     gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.35);
     
     osc.start(time);
     osc.stop(time + 0.5);
   }
   ```

2. Create unit test using OfflineAudioContext:
   ```typescript
   test('BD has correct pitch envelope', async () => {
     const ctx = new OfflineAudioContext(1, 44100, 44100);
     playBD(ctx, 0);
     const buffer = await ctx.startRendering();
     // Verify frequency content at t=0 and t=0.08
   });
   ```

**Affected Files:**
- `src/audio/voices/bassDrum.ts` (new)
- `tests/unit/voices.test.ts` (new)

**Tests:**
- Unit: Render with OfflineAudioContext, verify envelope
- Integration: Play BD via scheduler, verify timing

**Risks:**
- Sound not authentic → Adjust envelope parameters iteratively

**Dependencies:** Ticket 02

---

### Ticket 04: Snare Drum (SD) Voice
**File:** `backlog/open/04-voice-snare-drum.md`

**Objective:** Implement SD synthesis with oscillator + noise.

**Acceptance Criteria:**
- ✅ SD plays with tonal + noise components
- ✅ Oscillator: 180Hz triangle wave
- ✅ Noise: filtered white noise (highpass 1kHz)
- ✅ Envelope: 150ms decay

**Steps:**
1. Create `src/audio/voices/snareDrum.ts`:
   ```typescript
   export function playSD(ctx: AudioContext, time: number): void {
     // Tonal component (oscillator)
     const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 });
     const oscGain = new GainNode(ctx, { gain: 0.7 });
     
     // Noise component
     const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
     const data = buffer.getChannelData(0);
     for (let i = 0; i < data.length; i++) data[i] = Math.random() * 2 - 1;
     
     const noise = new AudioBufferSourceNode(ctx, { buffer });
     const noiseFilter = new BiquadFilterNode(ctx, { 
       type: 'highpass', 
       frequency: 1000, 
       Q: 1 
     });
     const noiseGain = new GainNode(ctx, { gain: 0.5 });
     
     // Connect
     osc.connect(oscGain);
     noise.connect(noiseFilter).connect(noiseGain);
     
     const master = new GainNode(ctx, { gain: 1.0 });
     oscGain.connect(master);
     noiseGain.connect(master);
     master.connect(ctx.destination);
     
     // Envelope
     master.gain.setValueAtTime(1.0, time);
     master.gain.exponentialRampToValueAtTime(0.0001, time + 0.15);
     
     osc.start(time);
     noise.start(time);
     osc.stop(time + 0.2);
   }
   ```

**Affected Files:**
- `src/audio/voices/snareDrum.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify oscillator + noise mix
- Integration: Play SD via scheduler

**Risks:**
- Noise generation performance → Pre-generate noise buffer

**Dependencies:** Ticket 02

---

### Ticket 05: Hi-Hat Voices (CH & OH)
**File:** `backlog/open/05-voice-hi-hats.md`

**Objective:** Implement closed and open hi-hat synthesis.

**Acceptance Criteria:**
- ✅ CH: Short filtered noise burst (70ms decay)
- ✅ OH: Longer filtered noise burst (200ms decay)
- ✅ Both use highpass filter (8kHz)

**Steps:**
1. Create `src/audio/voices/closedHiHat.ts` and `openHiHat.ts`
2. Use filtered white noise with different decay times
3. Add unit tests for both voices

**Affected Files:**
- `src/audio/voices/closedHiHat.ts` (new)
- `src/audio/voices/openHiHat.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify decay times (70ms vs 200ms)
- Integration: Play both via scheduler

**Risks:**
- Sounds too similar → Adjust filter Q and decay

**Dependencies:** Ticket 02

---

### Ticket 06: Clap (CP) Voice
**File:** `backlog/open/06-voice-clap.md`

**Objective:** Implement clap synthesis with multiple noise bursts.

**Acceptance Criteria:**
- ✅ CP: 3-4 noise bursts with slight delay
- ✅ Bandpass filter (1-3kHz)
- ✅ Total duration ~100ms

**Steps:**
1. Create `src/audio/voices/clap.ts`
2. Generate 3-4 short noise bursts at t, t+0.01, t+0.02, t+0.03
3. Apply bandpass filter and envelope

**Affected Files:**
- `src/audio/voices/clap.ts` (new)
- `tests/unit/voices.test.ts` (update)

**Tests:**
- Unit: Verify multiple bursts timing
- Integration: Play CP via scheduler

**Risks:**
- Sounds unnatural → Adjust burst timing and filter

**Dependencies:** Ticket 02

---

### Ticket 07: Sequencer Grid UI
**File:** `backlog/open/07-sequencer-grid-ui.md`

**Objective:** Create 16x5 step grid with click handlers.

**Acceptance Criteria:**
- ✅ 16 columns (steps) × 5 rows (instruments)
- ✅ Click toggles step on/off
- ✅ Visual feedback (active/inactive states)
- ✅ Responsive layout (desktop + tablet)

**Steps:**
1. Create `src/ui/sequencerGrid.ts`
2. Generate 80 buttons (16×5) with data attributes
3. Add click event listeners
4. Style with CSS grid

**Affected Files:**
- `src/ui/sequencerGrid.ts` (new)
- `src/ui/dom.ts` (new) - DOM utilities
- `styles.css` (update)
- `index.html` (update)

**Tests:**
- E2E: Click step → verify state change
- E2E: Verify responsive layout on different viewports

**Risks:**
- Performance with 80 buttons → Use event delegation

**Dependencies:** Ticket 01

---

### Ticket 08: Playback Controls
**File:** `backlog/open/08-playback-controls.md`

**Objective:** Create Play/Stop button and BPM slider.

**Acceptance Criteria:**
- ✅ Play button starts playback (gesture-gated)
- ✅ Stop button stops playback
- ✅ BPM slider (60-240) updates tempo in real-time
- ✅ Visual feedback (playing/stopped state)

**Steps:**
1. Create `src/ui/controls.ts`
2. Add Play/Stop button with click handler
3. Add BPM slider with input handler
4. Connect to scheduler start/stop

**Affected Files:**
- `src/ui/controls.ts` (new)
- `index.html` (update)
- `styles.css` (update)

**Tests:**
- E2E: Click Play → verify audio starts
- E2E: Change BPM → verify tempo changes
- E2E: Click Stop → verify audio stops

**Risks:**
- Autoplay blocked → Show clear CTA to click Play

**Dependencies:** Tickets 02, 07

---

### Ticket 09: Step Highlighting
**File:** `backlog/open/09-step-highlighting.md`

**Objective:** Visual feedback showing current step during playback.

**Acceptance Criteria:**
- ✅ Current step column highlighted
- ✅ Highlighting synced with audio (via `getOutputTimestamp()`)
- ✅ Smooth animation (no jank)

**Steps:**
1. Create `src/ui/visualizer.ts`
2. Use `requestAnimationFrame` to update highlight
3. Calculate current step from `audioContext.getOutputTimestamp()`
4. Add/remove CSS class for highlighting

**Affected Files:**
- `src/ui/visualizer.ts` (new)
- `styles.css` (update)

**Tests:**
- E2E: Verify highlighting advances during playback
- Performance: Verify no long tasks during highlighting

**Risks:**
- Highlighting lag → Optimize RAF callback

**Dependencies:** Tickets 02, 07, 08

---

### Ticket 10: State Management
**File:** `backlog/open/10-state-management.md`

**Objective:** Central state store for pattern and playback state.

**Acceptance Criteria:**
- ✅ Store holds pattern data (16×5 grid)
- ✅ Store holds playback state (playing/stopped, BPM, current step)
- ✅ Subscribe/notify pattern for state changes
- ✅ TypeScript interfaces for all state

**Steps:**
1. Create `src/state/store.ts` - Simple observable store
2. Create `src/state/pattern.ts` - Pattern data structure
3. Create `src/state/playback.ts` - Playback state
4. Connect UI and audio engine to store

**Affected Files:**
- `src/state/store.ts` (new)
- `src/state/pattern.ts` (new)
- `src/state/playback.ts` (new)
- `tests/unit/state.test.ts` (new)

**Tests:**
- Unit: State updates trigger subscribers
- Unit: Pattern operations (toggle step, set BPM)

**Risks:**
- Over-engineering → Keep it simple (no Redux/MobX)

**Dependencies:** Ticket 01

---

### Ticket 11: localStorage Persistence
**File:** `backlog/open/11-localstorage-persistence.md`

**Objective:** Save/load patterns via localStorage.

**Acceptance Criteria:**
- ✅ Save pattern on change
- ✅ Load pattern on page load
- ✅ Handle quota errors gracefully
- ✅ Validate JSON on load

**Steps:**
1. Create `src/storage/localStorage.ts`
2. Implement save/load with JSON serialization
3. Add error handling for quota exceeded
4. Add validation for loaded data

**Affected Files:**
- `src/storage/localStorage.ts` (new)
- `src/storage/serialization.ts` (new)
- `tests/unit/storage.test.ts` (new)

**Tests:**
- Unit: Save/load round-trip
- Unit: Handle corrupted data
- Unit: Handle quota exceeded

**Risks:**
- localStorage disabled → Show warning, disable save/load

**Dependencies:** Ticket 10

---

### Ticket 12: Integration & E2E Tests
**File:** `backlog/open/12-integration-e2e-tests.md`

**Objective:** Comprehensive test coverage for full user flows.

**Acceptance Criteria:**
- ✅ E2E: Load page → click Play → verify audio
- ✅ E2E: Toggle steps → verify pattern changes
- ✅ E2E: Change BPM → verify tempo changes
- ✅ E2E: Save/load pattern → verify persistence
- ✅ Performance: 0 long tasks during playback

**Steps:**
1. Write Playwright tests in `tests/e2e/playback.spec.ts`
2. Test all user flows
3. Add performance assertions (Long Tasks API)
4. Test cross-browser (Chrome, Firefox, Safari)

**Affected Files:**
- `tests/e2e/playback.spec.ts` (new)
- `playwright.config.ts` (update)

**Tests:**
- E2E: Full user journey
- Performance: Long tasks, jitter, memory

**Risks:**
- Flaky tests → Add proper waits and retries

**Dependencies:** All previous tickets

---

### Ticket 13: Performance Optimization
**File:** `backlog/open/13-performance-optimization.md`

**Objective:** Meet all performance SLOs.

**Acceptance Criteria:**
- ✅ Bundle size <50KB gzipped
- ✅ Jitter <2ms @ 120 BPM
- ✅ 0 long tasks during playback
- ✅ Memory <100MB during 5min playback
- ✅ Load time <2s on 3G

**Steps:**
1. Profile with Chrome DevTools
2. Optimize bundle (tree-shaking, code-splitting)
3. Optimize scheduler (reduce interval if needed)
4. Optimize UI updates (throttle, RAF)
5. Run Lighthouse audit

**Affected Files:**
- `vite.config.ts` (optimization settings)
- Various files (performance fixes)

**Tests:**
- Performance: Lighthouse score >90
- Performance: Bundle size check
- Performance: Memory profiling

**Risks:**
- SLOs not met → May need Worklet migration

**Dependencies:** Ticket 12

---

### Ticket 14: Cross-Browser Testing & Polish
**File:** `backlog/open/14-cross-browser-polish.md`

**Objective:** Ensure compatibility and polish UX.

**Acceptance Criteria:**
- ✅ Works in Chrome 90+, Firefox 88+, Safari 14+
- ✅ Responsive on desktop and tablet
- ✅ Accessible (keyboard navigation, ARIA labels)
- ✅ Error messages for unsupported browsers
- ✅ Loading states and feedback

**Steps:**
1. Test in all target browsers
2. Add keyboard shortcuts (Space = Play/Stop)
3. Add ARIA labels for accessibility
4. Add error handling for unsupported features
5. Polish CSS and animations

**Affected Files:**
- Various files (browser compatibility fixes)
- `styles.css` (polish)
- `index.html` (ARIA labels)

**Tests:**
- E2E: Run in all browsers
- Manual: Keyboard navigation
- Manual: Screen reader testing

**Risks:**
- Safari quirks → May need workarounds

**Dependencies:** Ticket 13

---

## Ticket Dependency Graph

```
01 (Setup)
  ├─→ 02 (Audio/Scheduler)
  │     ├─→ 03 (BD Voice)
  │     ├─→ 04 (SD Voice)
  │     ├─→ 05 (HH Voices)
  │     └─→ 06 (CP Voice)
  ├─→ 07 (Grid UI)
  │     ├─→ 08 (Controls)
  │     └─→ 09 (Highlighting)
  └─→ 10 (State)
        └─→ 11 (Storage)

All → 12 (Integration Tests)
  └─→ 13 (Performance)
    └─→ 14 (Polish)
```

**Parallel Work Possible:**
- Tickets 03-06 (voices) can be done in parallel
- Tickets 07-09 (UI) can be done in parallel with voices
- Ticket 10-11 (state/storage) can be done in parallel with UI

**Critical Path:** 01 → 02 → 08 → 12 → 13 → 14

---

## Summary

**Total Tickets:** 14  
**Estimated Time:** 10-14 days  
**Critical Path:** 7 tickets  
**Parallel Work:** 6 tickets can be parallelized  

**Next Steps:**
1. Create ticket files in `backlog/open/`
2. Start with Ticket 01 (Project Setup)
3. Follow dependency order
4. Move completed tickets to `backlog/closed/`
```

## Coding Rules
```md
# TR-808 Coding Rules

**Project:** tr808-synth  
**Stack:** Node.js/Web Audio API  
**Domain:** audio-synthesis  
**Year:** 2025

---

## Language & Style

**TypeScript:**
- Version: 5.3+
- Strict mode: `"strict": true` in tsconfig.json
- No `any` types (use `unknown` if needed)
- Explicit return types on public functions

**Naming:**
- Files: `kebab-case.ts` (e.g., `bass-drum.ts`)
- Classes/Interfaces: `PascalCase` (e.g., `AudioScheduler`)
- Functions/Variables: `camelCase` (e.g., `playBassDrum`)
- Constants: `UPPER_SNAKE_CASE` (e.g., `SAMPLE_RATE`)

**Formatting:**
- Prettier: `printWidth: 88`, `singleQuote: true`, `semi: true`
- ESLint: `@typescript-eslint/recommended`
- Line length: 88 characters max

**Commands:**
```bash
npm run format  # prettier --write "src/**/*.ts"
npm run lint    # eslint . --ext .ts
```

---

## Framework & Project Layout

**Structure:**
```
src/
├── audio/              # Web Audio API logic
│   ├── audioContext.ts # Singleton AudioContext
│   ├── scheduler.ts    # Look-ahead scheduler
│   ├── types.ts        # Audio interfaces
│   └── voices/         # Drum voice synthesizers
├── ui/                 # DOM manipulation
│   ├── sequencerGrid.ts
│   ├── controls.ts
│   └── visualizer.ts
├── state/              # State management
│   ├── store.ts
│   └── pattern.ts
├── storage/            # localStorage
│   └── localStorage.ts
└── main.ts             # Entry point
```

**Module Rules:**
- One class/interface per file
- Export only what's needed (no `export *`)
- Imports: absolute from `src/` (via tsconfig paths)
- No circular dependencies

**Environment:**
- Dev: `npm run dev` (Vite dev server)
- Build: `npm run build` (production bundle)
- No env vars needed (fully client-side)

---

## API & Contracts

**Internal APIs:**
```typescript
// Audio Engine API
interface Voice {
  play(context: AudioContext, time: number): void;
}

// Scheduler API
interface Scheduler {
  start(bpm: number, pattern: Pattern, onStep: (step: number) => void): void;
  stop(): void;
  getCurrentStep(): number;
}

// Storage API
interface StorageManager {
  savePattern(pattern: Pattern): void;
  loadPattern(): Pattern | null;
}
```

**Error Handling:**
- Use `try/catch` for async operations
- Throw typed errors: `class AudioInitError extends Error`
- Never swallow errors silently
- Log errors to console in dev, silent in prod

**Validation:**
- BPM: 60-240 range
- Pattern steps: exactly 16 per instrument
- localStorage data: validate JSON schema on load

---

## Testing

**Coverage Targets:**
- Overall: ≥80%
- Critical paths (scheduler, voices): ≥90%

**Libraries:**
- Unit: **Vitest** (fast, Vite-native)
- E2E: **Playwright** (cross-browser)
- Mocking: Vitest built-in mocks

**Test Structure:**
```typescript
// tests/unit/voices.test.ts
import { describe, it, expect } from 'vitest';
import { playBD } from '@/audio/voices/bassDrum';

describe('Bass Drum', () => {
  it('should play with correct pitch envelope', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();
    expect(buffer.getChannelData(0).some(v => v !== 0)).toBe(true);
  });
});
```

**Test Types:**
- **Unit:** Voices (OfflineAudioContext), scheduler logic, storage
- **Integration:** Audio + scheduler, UI + state
- **E2E:** Full user flows (Playwright)
- **Performance:** Long Tasks API, jitter measurement

**Commands:**
```bash
npm test              # Run all unit tests
npm run test:watch    # Watch mode
npm run e2e           # Run E2E tests
npm run test:coverage # Generate coverage report
```

---

## Security

**No Backend:** Fully client-side, no auth needed

**Dependencies:**
- Lock file: Commit `package-lock.json`
- Audit: `npm audit --production` in CI
- Updates: Review breaking changes before updating

**localStorage:**
- Validate JSON on load
- Catch `QuotaExceededError`
- No sensitive data (patterns only)

**CSP Header:**
```
Content-Security-Policy: default-src 'self'; script-src 'self'; style-src 'self' 'unsafe-inline'
```

**PII:** None (no user data collected)

---

## Observability

**Metrics:**
- Scheduler jitter: `performance.mark('step')` + `getOutputTimestamp()`
- Long tasks: `PerformanceObserver('longtask')`
- Memory: `performance.memory` (Chrome only)

**Logging:**
```typescript
// Dev only
if (import.meta.env.DEV) {
  console.debug('[Scheduler]', { step, time, drift });
}

// Errors always logged
console.error('[Audio]', error);
```

**Monitoring:**
- In-app debug panel (dev mode)
- No external analytics (privacy-first)

---

## Performance & Cost

**Budgets:**
- Bundle size: <50KB gzipped
- Main thread CPU: <30% p95 during playback
- Memory: <100MB during 5min session
- Long tasks: 0 during playback

**Profiling:**
```bash
npm run build        # Check bundle size
npm run preview      # Test production build
# Chrome DevTools → Performance tab
```

**Optimization:**
- Code splitting: Not needed (small app)
- Tree shaking: Enabled by Vite
- Minification: Enabled in production

**Cost:** $0 (static hosting, no runtime costs)

---

## Git & PR Process

**Branching:**
- Main: `master` (production-ready)
- Features: `feat/<ticket-number>-<description>`
- Fixes: `fix/<issue-description>`

**Commits:**
- Format: Conventional Commits
- Examples:
  - `feat(audio): add bass drum synthesis`
  - `fix(scheduler): correct jitter calculation`
  - `test(voices): add snare drum unit tests`
  - `docs(readme): update setup instructions`

**PR Checklist:**
- [ ] Tests pass (`npm test && npm run e2e`)
- [ ] Linting passes (`npm run lint`)
- [ ] Type checking passes (`tsc --noEmit`)
- [ ] Bundle size within budget
- [ ] No console errors in dev
- [ ] Tested in Chrome, Firefox, Safari
- [ ] Updated docs if needed

---

## Tooling

**Formatters:**
```json
// .prettierrc
{
  "semi": true,
  "singleQuote": true,
  "printWidth": 88,
  "tabWidth": 2,
  "trailingComma": "es5"
}
```

**Linters:**
```json
// .eslintrc.json
{
  "parser": "@typescript-eslint/parser",
  "extends": [
    "eslint:recommended",
    "plugin:@typescript-eslint/recommended",
    "prettier"
  ],
  "rules": {
    "@typescript-eslint/no-unused-vars": "error",
    "@typescript-eslint/explicit-function-return-type": "warn",
    "no-console": ["warn", { "allow": ["error", "warn"] }]
  }
}
```

**Pre-commit:**
```bash
# Optional: husky + lint-staged
npm install -D husky lint-staged
npx husky init
```

**CI Steps:**
```yaml
# .github/workflows/ci.yml
- run: npm ci
- run: npm run lint
- run: npm run typecheck
- run: npm test
- run: npm run build
- run: npm run e2e
```

---

## Frontend Standards

**Vanilla JS (No Framework):**
- Direct DOM manipulation
- Event delegation for performance
- `requestAnimationFrame` for animations

**Performance:**
- Web Vitals targets:
  - LCP: <2.5s
  - INP: <200ms
  - CLS: <0.1
- Bundle: <50KB gzipped
- No render-blocking resources

**Accessibility:**
- Keyboard navigation (Space = Play/Stop)
- ARIA labels on buttons
- Focus indicators
- Screen reader friendly

**Example:**
```typescript
// Good: Event delegation
sequencerGrid.addEventListener('click', (e) => {
  const target = e.target as HTMLElement;
  if (target.dataset.step) {
    toggleStep(Number(target.dataset.step));
  }
});

// Bad: Individual listeners
steps.forEach(step => {
  step.addEventListener('click', () => toggleStep(step.id));
});
```

---

## Backend Standards

**N/A** - Fully client-side application

---

## Governance & Decisions (MCDM)

**When to use:**
- Major architectural changes
- Library additions >10KB
- Breaking changes to APIs

**Criteria:**
- PerfGain (0.25)
- Maintainability (0.20)
- DevTime (0.20)
- Security (0.15)
- Cost (0.10)
- Scalability (0.05)
- DX (0.05)

**Example Decision:**
```markdown
## ADR-001: Look-ahead Scheduler vs AudioWorklet

| Alternative | PerfGain | Maintainability | DevTime | Security | Cost | Scalability | DX | Score |
|-------------|----------|-----------------|---------|----------|------|-------------|----|-------|
| Look-ahead  | 7        | 8               | 9       | 9        | 9    | 6           | 8  | 7.85  |
| Worklet     | 9        | 6               | 5       | 9        | 8    | 9           | 5  | 7.35  |

**Decision:** Look-ahead scheduler (simpler, sufficient for MVP)
```

---

## Acceptance Criteria

**Build:**
- [ ] `npm run build` succeeds
- [ ] Bundle size <50KB gzipped
- [ ] No TypeScript errors
- [ ] No ESLint errors

**Tests:**
- [ ] `npm test` passes (≥80% coverage)
- [ ] `npm run e2e` passes
- [ ] No console errors in tests

**Performance:**
- [ ] Jitter <2ms @ 120 BPM
- [ ] 0 long tasks during playback
- [ ] Memory <100MB after 5min

**Security:**
- [ ] `npm audit --production` clean
- [ ] No secrets in code
- [ ] localStorage validation works

**Docs:**
- [ ] README updated if needed
- [ ] JSDoc on public APIs
- [ ] ADRs for major decisions

---

## File Hygiene

**Rules:**
- Max file size: 400 LOC (split if larger)
- One responsibility per file
- Remove dead code immediately
- No commented-out code in commits

**Reusability:**
- Extract helpers to `src/utils/` if used 3+ times
- Keep voices independent (no shared state)
- Scheduler is stateful singleton (acceptable)

**State Management:**
- Avoid global mutable state
- Use store for shared state
- Pass dependencies explicitly

**Example:**
```typescript
// Good: Explicit dependencies
function playBD(ctx: AudioContext, time: number): void {
  // ...
}

// Bad: Hidden global dependency
let globalContext: AudioContext;
function playBD(time: number): void {
  globalContext.createOscillator(); // Where did this come from?
}
```

---

## Quick Reference

**Daily Commands:**
```bash
npm run dev          # Start dev server
npm test             # Run unit tests
npm run lint         # Check code style
npm run typecheck    # Check types
npm run build        # Production build
```

**Before Commit:**
```bash
npm run lint && npm run typecheck && npm test
```

**Before PR:**
```bash
npm run lint && npm run typecheck && npm test && npm run e2e && npm run build
```

---

## Anti-Patterns to Avoid

❌ **Don't:**
- Use `any` type
- Mutate function parameters
- Create global variables
- Write to localStorage in scheduler callback
- Use `ScriptProcessorNode` (deprecated)
- Start audio without user gesture
- Ignore TypeScript errors

✅ **Do:**
- Use strict TypeScript
- Keep functions pure when possible
- Pass dependencies explicitly
- Handle errors gracefully
- Use Web Audio API best practices
- Gate audio behind user interaction
- Write tests for critical paths

---

## Summary

**Core Principles:**
1. **Type Safety** - Strict TypeScript, no `any`
2. **Performance** - Meet all budgets, profile regularly
3. **Simplicity** - Vanilla JS, minimal dependencies
4. **Testing** - ≥80% coverage, E2E for critical flows
5. **Maintainability** - Small files, clear naming, no dead code

**Key Tools:**
- TypeScript 5.3+ (strict)
- Vite (build)
- Vitest (unit tests)
- Playwright (E2E)
- ESLint + Prettier (code quality)

**Success Metrics:**
- Bundle <50KB gzipped
- Jitter <2ms
- 0 long tasks during playback
- ≥80% test coverage
- 0 TypeScript/ESLint errors
```

## Backlog
- Read tickets under `backlog/open/` sorted by prefix `nn-` and dependency order.

Operating rules:
- Always consult architecture and coding rules first.
- Execute backlog tasks by dependency order.
- Write minimal viable code (MVP) with tests.
- Respect formatters, linters, and conventions.
- Update/clarify specs before changes if required.
- No chain‑of‑thought disclosure; provide final results + brief rationale.
 - Keep diffs minimal; refactor only what’s touched unless fixing clear bad practice.

Per‑task process:
1) Read the task → outline a short plan → confirm.
2) Change the minimal surface area.
3) Add/update tests and run local checks.
4) Stable commit with a clear message.

For significant choices:
- Use a lightweight MCDM: define criteria and weights; score alternatives; pick highest; record rationale.

Output:
- Brief summary of what changed.
- Files/diffs, tests, and run instructions (if needed).
- Notes on inconsistencies and proposed spec updates.

Quality Gates (must pass)
- Build succeeds; no type errors.
- Lint/format clean.
- Tests green (unit/integration; E2E/perf as applicable).
- Security checks: no secrets in code/logs; input validation present.
- Performance/observability budgets met (if defined).

Git Hygiene
- Branch: `feat/<ticket-id>-<slug>`.
- Commits: Conventional Commits; imperative; ≤72 chars.
- Reference the ticket in commit/PR.

Stop Rules
- Conflicts with architecture/coding rules.
- Missing critical secrets/inputs that would risk mis‑implementation.
- Required external dependency is down or license‑incompatible (document evidence).
- Violates security/compliance constraints.

Quota Awareness (optional)
- Document relevant API quotas and backoff strategies; prefer batch operations.