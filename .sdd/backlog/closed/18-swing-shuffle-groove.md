# Ticket 18: Swing/Shuffle & Groove

**Status:** 🟡 Open  
**Priority:** P2 (Should have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 02

---

## Objective

Add swing/shuffle timing to make patterns groove. This is what makes CR-717 patterns feel alive.

---

## Background

Swing delays every other 16th note slightly, creating a "triplet feel":
- 0% swing = straight 16ths (robotic)
- 50% swing = perfect triplets
- 66% swing = heavy shuffle (common in hip-hop)

Formula: `delayedTime = baseTime + (swing% × stepDuration × 0.5)`

---

## Acceptance Criteria

- ✅ Swing control: 0-75% (slider)
- ✅ Applies to odd-numbered steps (1, 3, 5, 7, 9, 11, 13, 15)
- ✅ Real-time adjustment during playback
- ✅ Swing amount saved in pattern
- ✅ Visual indicator on grid (subtle offset)
- ✅ No timing jitter or clicks
- ✅ Works with all BPM ranges

---

## Implementation

### 1. Update Scheduler

**src/audio/scheduler.ts:**
```typescript
export class AudioScheduler implements Scheduler {
  private swing: number = 0; // 0-0.75

  setSwing(amount: number): void {
    this.swing = Math.max(0, Math.min(0.75, amount));
  }

  start(ctx: AudioContext, bpm: number, onStep: SchedulerCallback): void {
    if (this.running) return;

    const stepDuration = 60 / bpm / 4;
    this.nextNoteTime = ctx.currentTime;
    this.currentStep = 0;
    this.running = true;

    this.intervalId = window.setInterval(() => {
      const end = ctx.currentTime + this.scheduleAheadTime;
      while (this.nextNoteTime < end && this.running) {
        // Apply swing to odd steps
        const swingOffset =
          this.currentStep % 2 === 1 ? this.swing * stepDuration * 0.5 : 0;

        onStep(this.currentStep, this.nextNoteTime + swingOffset);

        this.nextNoteTime += stepDuration;
        this.currentStep = (this.currentStep + 1) % 16;
      }
    }, this.tickInterval);
  }
}
```

### 2. Swing Control UI

**Add to header in index.html:**
```html
<div class="control-group">
  <label for="swing-slider">Swing</label>
  <div style="display: flex; align-items: center; gap: 8px;">
    <input type="range" id="swing-slider" min="0" max="75" value="0" 
           aria-label="Swing amount">
    <span id="swing-display" class="value-display">0%</span>
  </div>
</div>
```

**Wire in main.ts:**
```typescript
const swingSlider = document.getElementById('swing-slider') as HTMLInputElement;
const swingDisplay = document.getElementById('swing-display') as HTMLElement;

swingSlider.addEventListener('input', () => {
  const swing = parseInt(swingSlider.value, 10) / 100;
  swingDisplay.textContent = `${swingSlider.value}%`;
  scheduler.setSwing(swing);
});
```

### 3. Visual Swing Indicator

**Update step button CSS:**
```css
.step-button[data-step="1"],
.step-button[data-step="3"],
.step-button[data-step="5"],
.step-button[data-step="7"],
.step-button[data-step="9"],
.step-button[data-step="11"],
.step-button[data-step="13"],
.step-button[data-step="15"] {
  /* Subtle visual offset for swing steps */
  position: relative;
}

.step-button[data-step="1"]::after,
.step-button[data-step="3"]::after,
.step-button[data-step="5"]::after,
.step-button[data-step="7"]::after,
.step-button[data-step="9"]::after,
.step-button[data-step="11"]::after,
.step-button[data-step="13"]::after,
.step-button[data-step="15"]::after {
  content: '';
  position: absolute;
  bottom: 2px;
  left: 50%;
  transform: translateX(-50%);
  width: 4px;
  height: 4px;
  background: var(--color-accent);
  border-radius: 50%;
  opacity: 0.3;
}
```

### 4. Update Pattern State

**src/state/types.ts:**
```typescript
export interface Pattern {
  name: string;
  bpm: number;
  swing: number; // NEW: 0-0.75
  steps: { /* ... */ };
}
```

### 5. Preset Swing Amounts

**Add quick preset buttons:**
```html
<div class="swing-presets">
  <button class="preset-btn" data-swing="0">Straight</button>
  <button class="preset-btn" data-swing="33">Light</button>
  <button class="preset-btn" data-swing="50">Triplet</button>
  <button class="preset-btn" data-swing="66">Heavy</button>
</div>
```

```typescript
document.querySelectorAll('.preset-btn').forEach((btn) => {
  btn.addEventListener('click', () => {
    const swing = parseInt((btn as HTMLElement).dataset.swing!, 10);
    swingSlider.value = swing.toString();
    swingSlider.dispatchEvent(new Event('input'));
  });
});
```

---

## Tests

- Unit: Swing calculation correct for all steps
- Unit: Swing 0% = no offset
- Unit: Swing 50% = triplet timing
- E2E: Swing slider changes timing
- E2E: Preset buttons work
- Performance: No jitter with swing enabled

---

## Affected Files

**Modified:**
- `src/audio/scheduler.ts` - Swing logic
- `src/state/types.ts` - Add swing to Pattern
- `src/main.ts` - Wire swing control
- `index.html` - Swing slider
- `styles.css` - Swing visual indicators

---

## Definition of Done

- [ ] Swing control working
- [ ] Timing accurate
- [ ] Visual indicators
- [ ] Presets work
- [ ] Saved in pattern
- [ ] Tests pass
- [ ] Committed: "feat(audio): add swing/shuffle timing control"
