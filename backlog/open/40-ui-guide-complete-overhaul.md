# UI Best-Practices Guide — tr808-synth (2025)

## 1) TL;DR

* Dark theme by default; declare `color-scheme: dark` and respect `prefers-color-scheme` and `prefers-reduced-motion`. ([MDN Web Docs][1])
* Grid-first layout. Fixed 16 columns for steps. Use CSS Grid; align labels and cells with `subgrid` when available, fall back to a regular grid. ([MDN Web Docs][2])
* Modern CSS stack: container queries, `@layer`, `:has()`, nesting. Lower specificity and avoid viewport-only breakpoints. ([MDN Web Docs][3])
* AV sync from the audio clock. Compute visual position from `AudioContext.getOutputTimestamp()` and render via `requestAnimationFrame`. ([MDN Web Docs][4])
* Animate only `transform` and `opacity`; batch DOM writes; verify in DevTools Performance. ([MDN Web Docs][5])
* Accessibility AA: keyboardable grid pattern; visible focus; pointer targets ≥24×24 CSS px. ([W3C][6])
* Desktop-first with tablet optimization via **container queries** rather than viewport media queries. ([MDN Web Docs][3])
* Use CSS custom properties for design tokens; keep cascade predictable with `@layer`. ([MDN Web Docs][7])
* Detect input modality: handle `hover`/`pointer` media features for touch. ([MDN Web Docs][8])
* Color contrast meets WCAG AA (4.5:1 normal, 3:1 large). ([W3C][9])

---

## 2) Landscape — What’s new in 2025

* **Container queries** and style queries are stable in modern engines, enabling component-scoped responsiveness. Prefer `container-type: inline-size` and `@container` over viewport queries. ([MDN Web Docs][3])
* **`@layer`** is widely supported. Layer tokens/base/components/utilities to control cascade deterministically. ([MDN Web Docs][10])
* **`:has()`** enables parent-state styling (e.g., highlight a row if any step is active). Use selectively. ([MDN Web Docs][11])
* **CSS nesting** now broadly supported; SCSS for nesting alone is unnecessary. ([MDN Web Docs][12])
* **Subgrid** allows precise column alignment across rows; keep a fallback for older browsers. ([MDN Web Docs][2])
* **Web Audio timing** clarified: `getOutputTimestamp()` exposes audio and performance clocks for tight AV sync. ([MDN Web Docs][4])
* **WCAG 2.2** adds Target Size (Minimum) 24×24 px; adopt for all interactive controls. ([W3C][13])

---

## 3) UI Architecture Patterns (TypeScript/Vanilla JS + Web Audio)

### Pattern A — Vanilla TS Modules + CSS Tokens (MVP)

**When**: ≤20 components; performance-sensitive; zero framework overhead.
**Steps**

1. Modules: `SequencerGrid`, `Transport`, `InstrumentStrip`.
2. Tokens via CSS custom properties; organize with `@layer`.
3. Component responsiveness with container queries.
4. rAF loop for visual highlighting tied to audio clock. ([MDN Web Docs][7])
   **Pros**: tiny bundle, direct DOM control. **Cons**: manual state management.
   **Later**: storybook-like catalog; theme switcher.

### Pattern B — Modular Components + Subgrid (Scale-up)

**When**: many panels/states; shared alignment across rows.
**How**: define 16-column subgrid for step cells; keep fallback grid for non-subgrid browsers. ([MDN Web Docs][2])
**Migration**: incrementally replace row layouts with subgrid; keep tokens and container queries unchanged.

---

## 4) Priority 1 — Design System / Visual Design

**Why**: reduce cognitive load; match pro audio UI expectations.
**Scope**: colors, type, spacing, radii, states; out-of-scope: audio DSP.
**Decisions**

* Dark default; declare `color-scheme: dark` and `<meta name="color-scheme" content="dark light">`. Honor user theme preferences. ([MDN Web Docs][1])
* HSL tokens for easy theming; minimum text contrast per WCAG AA. ([W3C][9])

**Tokens**

```css
@layer tokens {
  :root {
    /* Color */
    --bg-0: hsl(0 0% 10%);    /* app base */
    --bg-1: hsl(0 0% 15%);    /* panels */
    --bg-2: hsl(0 0% 20%);
    --text-0: hsl(0 0% 98%);
    --text-1: hsl(0 0% 65%);
    --accent: hsl(18 100% 60%);   /* 808 orange */
    --accent-weak: hsl(18 100% 70%);
    --ok: hsl(120 100% 45%);
    --border: hsl(0 0% 28%);

    /* Type & spacing */
    --ff: system-ui, sans-serif;
    --fs-xs: 11px; --fs-sm: 12px; --fs-md: 14px; --fs-lg: 16px; --fs-xl: 20px;
    --space-1: 4px; --space-2: 8px; --space-3: 12px; --space-4: 16px; --space-6: 24px; --space-8: 32px;
    --radius-1: 2px; --radius-2: 4px; --radius-3: 8px;

    /* Controls */
    --step-size: 44px; /* ≥ WCAG 2.2 target size */
  }
}
@layer base {
  :root { color-scheme: dark; background: var(--bg-0); color: var(--text-0); }
  body  { font-family: var(--ff); }
}
```

**Guardrails & SLOs**: Text contrast ≥ AA; interactive targets ≥24×24 CSS px. ([W3C][9])
**Failure modes → Recovery**: token drift → review for hardcoded values; low contrast → adjust HSL lightness until AA.

---

## 5) Priority 1 — UX / Interaction

**Why**: fast pattern entry; minimal friction.
**Decisions**

* Click toggles step; double-click sets accent; current step highlights.
* Keyboard: arrows move focus in grid; `Enter`/Space toggle; Space on transport plays/stops; `1–5` selects instrument; announce state via `aria-live="polite"`. ([W3C][6])
  **Implementation**

```html
<section class="seq" role="grid" aria-label="808 sequencer">
  <div class="row" role="row" aria-label="Bass Drum">
    <span class="label">BD</span>
    <div class="cells">
      <button class="step" role="gridcell" aria-pressed="false" data-step="0"></button>
      <!-- ... 16 steps -->
    </div>
  </div>
</section>
```

**Guardrails & SLOs**: ≤150 ms state transitions; AV drift ≤1 audio frame for highlight.
**Failure modes**: desync → compute visual index from audio clock; sluggish highlight → move animation to `transform/opacity`. ([MDN Web Docs][4])

---

## 6) Priority 2 — Accessibility

**Why**: inclusive creation; faster power-user flows.
**Decisions**

* ARIA Grid semantics (`role="grid" → row → gridcell`) plus explicit keyboard navigation. ([W3C][6])
* Visible `:focus-visible` rings; `aria-live="polite"` for BPM/transport changes. ([MDN Web Docs][14])
* Pointer targets ≥24×24; respect motion and color-scheme preferences. ([W3C][13])
  **Guardrails**: axe run passes; keyboard-only navigation works.

---

## 7) Priority 3 — Responsive Design

**Decisions**

* Desktop-first; tablet via container queries on panels.
* Keep 16 columns fixed; wrap instrument rows when space is tight.
  **Example**

```css
.seq { container-type: inline-size; }
@container (width < 920px) {
  .cells { gap: var(--space-1); }
  .toolbar { grid-template-columns: 1fr 1fr; }
}
```

([MDN Web Docs][3])

---

## 8) Component Patterns

### Step button

```css
@layer components {
  .step {
    inline-size: var(--step-size);
    block-size: var(--step-size);
    border-radius: var(--radius-2);
    border: 1px solid var(--border);
    background: var(--bg-2);
    transition: transform .12s, opacity .12s, background-color .12s;
  }
  .step[aria-pressed="true"] { background: var(--accent); }
  .step:focus-visible { outline: 2px solid var(--accent-weak); outline-offset: 2px; }
  .row:has(.step[aria-pressed="true"]) {
    outline: 1px solid color-mix(in srgb, var(--accent) 40%, transparent);
  }
}
```

Uses `:has()` for parent highlighting. ([MDN Web Docs][11])

### Grid with subgrid (fallback included)

```css
.seq { display: grid; grid-template-columns: auto 1fr; gap: var(--space-2); }
.row { display: grid; grid-template-columns: subgrid; grid-column: 1 / -1; }
@supports not (grid-template-columns: subgrid) {
  .row { grid-template-columns: auto repeat(16, var(--step-size)); }
}
```

([MDN Web Docs][2])

---

## 9) Audio-Visual Sync Pattern

```ts
// scheduler.ts
export function startVisualSync(
  ctx: AudioContext,
  stepDurSec: number,
  setActive: (i: number) => void
) {
  let rafId = 0;
  const frame = () => {
    const { contextTime } = ctx.getOutputTimestamp();
    const pos = Math.floor((contextTime / stepDurSec) % 16);
    setActive(pos);
    rafId = requestAnimationFrame(frame);
  };
  rafId = requestAnimationFrame(frame);
  return () => cancelAnimationFrame(rafId);
}
```

Use `getOutputTimestamp()` for audio-clock truth and render with `requestAnimationFrame`. ([MDN Web Docs][4])

---

## 10) Performance & UX Responsiveness

* Animate only `transform`/`opacity`; avoid layout/paint churn during playback. ([MDN Web Docs][5])
* Measure with User Timing and inspect Long Tasks (>50 ms). ([MDN Web Docs][15])
* rAF cadence adapts to display refresh and throttles in background tabs; do not use timers for the playhead. ([MDN Web Docs][16])

---

## 11) Code Quality Standards

* CSS organization: `@layer tokens, base, components, utilities`; use nesting sparingly to keep specificity low. ([MDN Web Docs][10])
* Design tokens with custom properties for all color, spacing, and type values. ([MDN Web Docs][7])
* JS: module-scoped components; event delegation on grid; pure render helpers.
* CI check: lint style tokens usage; forbid hardcoded spacing/color values.

---

## 12) Input & Cross-Platform

* Respect input capabilities: use `@media (hover: none)` and `@media (pointer: coarse)` to disable hover-only affordances and increase spacing. ([MDN Web Docs][8])
* Declare `color-scheme` to integrate browser chrome with dark UI. ([MDN Web Docs][1])

---

## 13) Reading List (with dates and gist)

* MDN: **Container queries** (Baseline 2024–2025). Component-scoped responsive design. ([MDN Web Docs][3])
* MDN: **`@layer`** (2023–2025). Deterministic cascade control. ([MDN Web Docs][10])
* MDN: **`:has()`** (2023–2025). Parent-state styling. ([MDN Web Docs][11])
* MDN: **CSS nesting** (2024–2025). Replace SCSS for nesting. ([MDN Web Docs][12])
* MDN: **Subgrid** (2024–2025). Cross-row alignment. ([MDN Web Docs][2])
* MDN: **Web Audio `getOutputTimestamp()`** (2025-05-23). AV sync. ([MDN Web Docs][4])
* MDN: **Animation performance** (2025-08-19). Prefer transform/opacity. ([MDN Web Docs][5])
* W3C: **WCAG 2.2** (2024-12-12). Target size and AA baseline. ([W3C][17])

---

## 14) Decision Log (ADR style)

* **ADR-001**: Container queries over viewport media queries, because components must respond to their containers, not the page. ([MDN Web Docs][3])
* **ADR-002**: Visual playhead driven by audio clock (`getOutputTimestamp()` + rAF) over timer-based loops, to prevent drift and jitter. ([MDN Web Docs][4])
* **ADR-003**: Animate `transform`/`opacity` only during playback to sustain 60 fps. ([MDN Web Docs][5])
* **ADR-004**: Enforce ≥24×24 targets and visible focus to align with WCAG 2.2 AA. ([W3C][13])

---

## 15) Anti-Patterns to Avoid (and what instead)

* **Timer-driven playhead** (`setInterval`) → drifts vs audio. **Use** audio clock + rAF. ([MDN Web Docs][4])
* **Animating layout properties** (`top/left/width/height`) → layout thrash. **Use** `transform/opacity`. ([MDN Web Docs][5])
* **Viewport-only breakpoints** in component CSS → brittle layouts. **Use** container queries. ([MDN Web Docs][3])
* **Hover-only cues** on touch. **Use** `hover`/`pointer` queries to adapt. ([MDN Web Docs][8])
* **Hidden focus** for aesthetic reasons. **Keep** `:focus-visible` styles. ([MDN Web Docs][14])

---

## 16) Evidence & Citations

Key claims are backed by primary docs: MDN for platform features and performance; W3C WCAG for accessibility; WAI-ARIA APG for keyboardable grid semantics. See citations inline.

---

## 17) Verification

* **Accessibility**: run axe; manual keyboard-only pass; confirm targets ≥24×24; verify grid keyboard spec from APG. **Confidence**: High. ([W3C][6])
* **Performance**: record during playback; check for Long Tasks; confirm animation properties are compositor-friendly. **Confidence**: High. ([MDN Web Docs][3])
* **AV Sync**: log `(ctx.getOutputTimestamp().contextTime % stepDur)` vs UI index; drift > 1 audio frame is a bug. **Confidence**: High. ([MDN Web Docs][4])

---

### Minimal Scaffold (ready to paste)

**HTML**

```html
<header class="toolbar">
  <button id="play" aria-pressed="false" aria-live="polite">Play</button>
  <label>BPM <input id="bpm" type="range" min="60" max="240" value="120" aria-live="polite"></label>
</header>

<section class="seq" role="grid" aria-label="808 sequencer">
  <div class="row" role="row" aria-label="Bass Drum">
    <span class="label">BD</span>
    <div class="cells">
      <!-- 16 buttons -->
      <button class="step" role="gridcell" aria-pressed="false" data-step="0"></button>
      <!-- … -->
    </div>
  </div>
  <!-- SD, CH, OH, CP -->
</section>
```

**CSS**

```css
@layer tokens, base, components;

@layer tokens { /* tokens from section 4 */ }
@layer base {
  .toolbar { display:flex; gap:var(--space-3); padding:var(--space-3); background:var(--bg-1); }
  .seq { display:grid; grid-template-columns:auto 1fr; gap:var(--space-2); container-type:inline-size; }
  .label { inline-size:3ch; color:var(--text-1); align-self:center; }
  .cells { display:grid; grid-template-columns:repeat(16, var(--step-size)); gap:var(--space-2); }
  .row  { display:grid; grid-template-columns: subgrid; grid-column: 1 / -1; }
  @supports not (grid-template-columns: subgrid) {
    .row { grid-template-columns:auto repeat(16, var(--step-size)); }
  }
}
@layer components {
  .step {
    inline-size:var(--step-size); block-size:var(--step-size);
    border-radius:var(--radius-2); border:1px solid var(--border); background:var(--bg-2);
    transition:transform .12s, opacity .12s, background-color .12s;
  }
  .step[aria-pressed="true"] { background:var(--accent); }
  .step:focus-visible { outline:2px solid var(--accent-weak); outline-offset:2px; }
  .row:has(.step[aria-pressed="true"]) { outline:1px solid color-mix(in srgb, var(--accent) 40%, transparent); }
}
@container (width < 920px) { .cells { gap:var(--space-1); } }
@media (prefers-reduced-motion: reduce) { .step { transition:none; } }
@media (hover: none), (pointer: coarse) { /* tone down hover-only effects */ }
```

**TS**

```ts
const seq = document.querySelector<HTMLElement>('.seq')!;
seq.addEventListener('click', (e) => {
  const btn = (e.target as HTMLElement).closest<HTMLButtonElement>('.step');
  if (!btn) return;
  const next = btn.getAttribute('aria-pressed') !== 'true';
  btn.setAttribute('aria-pressed', String(next));
});

seq.addEventListener('keydown', (e: KeyboardEvent) => {
  const active = document.activeElement as HTMLButtonElement;
  if (!active?.classList.contains('step')) return;
  const row = active.closest('.row')!;
  const cells = Array.from(row.querySelectorAll<HTMLButtonElement>('.step'));
  const i = cells.indexOf(active);
  if (e.key === 'ArrowRight') (cells[i + 1] || cells[0]).focus();
  if (e.key === 'ArrowLeft')  (cells[i - 1] || cells[cells.length - 1]).focus();
  if (e.key === 'Enter' || e.key === ' ') { active.click(); e.preventDefault(); }
});
```

This guide is ready to implement with today’s browser baselines.

[1]: https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Properties/color-scheme "color-scheme - CSS - MDN Web Docs - Mozilla"
[2]: https://developer.mozilla.org/en-US/docs/Web/Performance/Guides/CSS_JavaScript_animation_performance "CSS and JavaScript animation performance - Performance | MDN"
[3]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_containment/Container_queries "CSS container queries - CSS | MDN"
[4]: https://developer.mozilla.org/en-US/docs/Web/API/AudioContext/getOutputTimestamp "AudioContext: getOutputTimestamp() method - Web APIs | MDN"
[5]: https://developer.mozilla.org/en-US/docs/Web/CSS/Guides/Grid_layout/Subgrid "Subgrid - CSS | MDN"
[6]: https://www.w3.org/WAI/ARIA/apg/patterns/grid/ " Grid (Interactive Tabular Data and Layout Containers) Pattern | APG | WAI | W3C"
[7]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_cascading_variables/Using_CSS_custom_properties "Using CSS custom properties (variables) - MDN Web Docs"
[8]: https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/At-rules/%40media/hover "hover - CSS | MDN"
[9]: https://www.w3.org/WAI/WCAG21/Understanding/contrast-minimum.html "Understanding Success Criterion 1.4.3: Contrast (Minimum)"
[10]: https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/At-rules/%40layer "@layer - CSS | MDN"
[11]: https://developer.mozilla.org/en-US/docs/Web/CSS/Reference/Selectors/%3Ahas ":has() - CSS | MDN"
[12]: https://developer.mozilla.org/en-US/docs/Web/API/PerformanceLongTaskTiming "PerformanceLongTaskTiming - Web APIs | MDN"
[13]: https://www.w3.org/WAI/WCAG22/Understanding/target-size-minimum.html "Understanding Success Criterion 2.5.8: Target Size (Minimum) | WAI | W3C"
[14]: https://developer.mozilla.org/en-US/docs/Web/Accessibility/ARIA/Reference/Roles/grid_role "ARIA: grid role - ARIA | MDN"
[15]: https://developer.mozilla.org/en-US/docs/Web/CSS/CSS_nesting "CSS nesting - CSS | MDN"
[16]: https://developer.mozilla.org/en-US/docs/Web/API/Canvas_API/Tutorial/Basic_animations "Basic animations - Canvas API - MDN Web Docs"
[17]: https://www.w3.org/TR/WCAG22/ "Web Content Accessibility Guidelines (WCAG) 2.2"
