# Ticket 10: State Management

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 01

---

## Objective

Central state store for pattern and playback state.

---

## Acceptance Criteria

- ✅ Store holds pattern data (16×5 grid)
- ✅ Store holds playback state (playing/stopped, BPM, current step)
- ✅ Subscribe/notify pattern for state changes
- ✅ TypeScript interfaces for all state
- ✅ Unit tests verify state operations

---

## Implementation

### Create `src/state/types.ts`
```typescript
export interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[];
    SD: boolean[];
    CH: boolean[];
    OH: boolean[];
    CP: boolean[];
  };
}

export interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  bpm: number;
}

export interface AppState {
  pattern: Pattern;
  playback: PlaybackState;
}
```

### Create `src/state/pattern.ts`
```typescript
import type { Pattern } from './types';

export function createEmptyPattern(): Pattern {
  return {
    name: 'Untitled',
    bpm: 120,
    steps: {
      BD: Array(16).fill(false),
      SD: Array(16).fill(false),
      CH: Array(16).fill(false),
      OH: Array(16).fill(false),
      CP: Array(16).fill(false),
    },
  };
}

export function toggleStep(
  pattern: Pattern,
  instrument: keyof Pattern['steps'],
  step: number
): Pattern {
  return {
    ...pattern,
    steps: {
      ...pattern.steps,
      [instrument]: pattern.steps[instrument].map((active, i) =>
        i === step ? !active : active
      ),
    },
  };
}
```

### Create `src/state/store.ts`
```typescript
import type { AppState } from './types';
import { createEmptyPattern } from './pattern';

type Listener = (state: AppState) => void;

class Store {
  private state: AppState;
  private listeners: Set<Listener> = new Set();

  constructor() {
    this.state = {
      pattern: createEmptyPattern(),
      playback: {
        isPlaying: false,
        currentStep: 0,
        bpm: 120,
      },
    };
  }

  getState(): AppState {
    return this.state;
  }

  setState(partial: Partial<AppState>): void {
    this.state = { ...this.state, ...partial };
    this.notify();
  }

  subscribe(listener: Listener): () => void {
    this.listeners.add(listener);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach((listener) => listener(this.state));
  }
}

export const store = new Store();
```

### Create `tests/unit/state.test.ts`
```typescript
import { describe, it, expect } from 'vitest';
import { createEmptyPattern, toggleStep } from '../../src/state/pattern';

describe('Pattern', () => {
  it('should create empty pattern', () => {
    const pattern = createEmptyPattern();
    expect(pattern.bpm).toBe(120);
    expect(pattern.steps.BD).toHaveLength(16);
    expect(pattern.steps.BD.every((s) => s === false)).toBe(true);
  });

  it('should toggle step', () => {
    const pattern = createEmptyPattern();
    const updated = toggleStep(pattern, 'BD', 0);
    expect(updated.steps.BD[0]).toBe(true);
    expect(updated.steps.BD[1]).toBe(false);
  });
});
```

---

## Affected Files

**New:**
- `src/state/types.ts`
- `src/state/pattern.ts`
- `src/state/store.ts`
- `tests/unit/state.test.ts`

---

## Tests

- Unit: State updates trigger subscribers
- Unit: Pattern operations work correctly

---

## Definition of Done

- [ ] State management implemented
- [ ] Unit tests pass
- [ ] TypeScript types defined
- [ ] Committed: "feat(state): add state management"
