# Ticket 09: Step Highlighting

**Status:** 🟡 Open  
**Priority:** P2 (Should have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** Tickets 02, 07, 08

---

## Objective

Visual feedback showing current step during playback.

---

## Acceptance Criteria

- ✅ Current step column highlighted
- ✅ Highlighting synced with audio via `getOutputTimestamp()`
- ✅ Smooth animation (no jank)
- ✅ No long tasks during playback

---

## Implementation

### Create `src/ui/visualizer.ts`
```typescript
import { getAudioContext } from '../audio/audioContext';

export class Visualizer {
  private rafId: number | null = null;
  private onStepChange: (step: number) => void;
  private bpm: number;
  private startTime: number = 0;

  constructor(onStepChange: (step: number) => void) {
    this.onStepChange = onStepChange;
    this.bpm = 120;
  }

  start(bpm: number): void {
    this.bpm = bpm;
    const ctx = getAudioContext();
    if (!ctx) return;

    this.startTime = ctx.currentTime;
    this.tick();
  }

  stop(): void {
    if (this.rafId !== null) {
      cancelAnimationFrame(this.rafId);
      this.rafId = null;
    }
  }

  private tick = (): void => {
    const ctx = getAudioContext();
    if (!ctx) return;

    const stepDuration = 60 / this.bpm / 4; // 16th notes
    const elapsed = ctx.currentTime - this.startTime;
    const currentStep = Math.floor(elapsed / stepDuration) % 16;

    this.onStepChange(currentStep);

    this.rafId = requestAnimationFrame(this.tick);
  };
}
```

---

## Affected Files

**New:**
- `src/ui/visualizer.ts`

---

## Tests

- E2E: Verify highlighting advances during playback
- Performance: Verify no long tasks

---

## Definition of Done

- [ ] Visualizer implemented
- [ ] Highlighting synced with audio
- [ ] No performance issues
- [ ] Committed: "feat(ui): add step highlighting visualizer"
