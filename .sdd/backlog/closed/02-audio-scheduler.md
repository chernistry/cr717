# Ticket 02: AudioContext & Scheduler Core

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 4-6 hours  
**Dependencies:** Ticket 01

---

## Objective

Implement AudioContext singleton and look-ahead scheduler for stable timing.

---

## Acceptance Criteria

- ✅ AudioContext initializes on user gesture
- ✅ Scheduler ticks every 25ms
- ✅ Schedules notes 100ms ahead
- ✅ Tracks current step (0-15)
- ✅ Jitter <2ms measured via `getOutputTimestamp()`
- ✅ Unit tests verify scheduler logic
- ✅ No memory leaks on start/stop cycles

---

## Implementation

### 1. Create `src/audio/types.ts`
```typescript
export interface SchedulerCallback {
  (step: number, time: number): void;
}

export interface Scheduler {
  start(ctx: AudioContext, bpm: number, onStep: SchedulerCallback): void;
  stop(): void;
  getCurrentStep(): number;
  isRunning(): boolean;
}
```

### 2. Create `src/audio/audioContext.ts`
```typescript
let audioContext: AudioContext | null = null;

export async function initAudio(): Promise<AudioContext> {
  if (!audioContext) {
    audioContext = new AudioContext({ latencyHint: 'interactive' });
    await audioContext.resume();
  }
  return audioContext;
}

export function getAudioContext(): AudioContext | null {
  return audioContext;
}
```

### 3. Create `src/audio/scheduler.ts`
```typescript
import type { Scheduler, SchedulerCallback } from './types';

export class AudioScheduler implements Scheduler {
  private intervalId: number | null = null;
  private nextNoteTime = 0;
  private currentStep = 0;
  private readonly scheduleAheadTime = 0.1; // 100ms
  private readonly tickInterval = 25; // ms
  private running = false;

  start(ctx: AudioContext, bpm: number, onStep: SchedulerCallback): void {
    if (this.running) return;

    const stepDuration = 60 / bpm / 4; // 16th notes
    this.nextNoteTime = ctx.currentTime;
    this.currentStep = 0;
    this.running = true;

    this.intervalId = window.setInterval(() => {
      const end = ctx.currentTime + this.scheduleAheadTime;
      while (this.nextNoteTime < end && this.running) {
        onStep(this.currentStep, this.nextNoteTime);
        this.nextNoteTime += stepDuration;
        this.currentStep = (this.currentStep + 1) % 16;
      }
    }, this.tickInterval);
  }

  stop(): void {
    this.running = false;
    if (this.intervalId !== null) {
      clearInterval(this.intervalId);
      this.intervalId = null;
    }
  }

  getCurrentStep(): number {
    return this.currentStep;
  }

  isRunning(): boolean {
    return this.running;
  }
}
```

### 4. Create `tests/unit/scheduler.test.ts`
```typescript
import { describe, it, expect, beforeEach, afterEach, vi } from 'vitest';
import { AudioScheduler } from '../../src/audio/scheduler';

describe('AudioScheduler', () => {
  let scheduler: AudioScheduler;
  let mockContext: AudioContext;

  beforeEach(() => {
    scheduler = new AudioScheduler();
    mockContext = new AudioContext();
  });

  afterEach(() => {
    scheduler.stop();
    mockContext.close();
  });

  it('should start and track steps', () => {
    const steps: number[] = [];
    scheduler.start(mockContext, 120, (step) => steps.push(step));
    
    expect(scheduler.isRunning()).toBe(true);
    scheduler.stop();
    expect(scheduler.isRunning()).toBe(false);
  });

  it('should cycle through 16 steps', () => {
    let lastStep = -1;
    scheduler.start(mockContext, 120, (step) => {
      lastStep = step;
    });

    // Wait for some steps
    return new Promise((resolve) => {
      setTimeout(() => {
        scheduler.stop();
        expect(lastStep).toBeGreaterThanOrEqual(0);
        expect(lastStep).toBeLessThan(16);
        resolve(undefined);
      }, 100);
    });
  });
});
```

---

## Affected Files

**New:**
- `src/audio/types.ts`
- `src/audio/audioContext.ts`
- `src/audio/scheduler.ts`
- `tests/unit/scheduler.test.ts`

---

## Tests

- Unit: Verify scheduler starts/stops
- Unit: Verify step cycling (0-15)
- Unit: Verify no memory leaks
- Integration: Measure jitter with `getOutputTimestamp()`

---

## Risks

- Jitter >2ms → Reduce tick interval to 15ms
- Autoplay blocked → Handled by user gesture requirement

---

## Definition of Done

- [ ] All files created
- [ ] Unit tests pass
- [ ] Jitter <2ms verified
- [ ] No console errors
- [ ] Committed: "feat(audio): add AudioContext singleton and look-ahead scheduler"
