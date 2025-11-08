# Ticket 11: localStorage Persistence

**Status:** 🟡 Open  
**Priority:** P2 (Should have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** Ticket 10

---

## Objective

Save/load patterns via localStorage with error handling.

---

## Acceptance Criteria

- ✅ Save pattern on change
- ✅ Load pattern on page load
- ✅ Handle quota errors gracefully
- ✅ Validate JSON on load
- ✅ Unit tests verify round-trip

---

## Implementation

### Create `src/storage/localStorage.ts`
```typescript
import type { Pattern } from '../state/types';

const STORAGE_KEY = 'tr808:v1:pattern';

export function savePattern(pattern: Pattern): void {
  try {
    const json = JSON.stringify(pattern);
    localStorage.setItem(STORAGE_KEY, json);
  } catch (error) {
    if (error instanceof DOMException && error.name === 'QuotaExceededError') {
      console.error('localStorage quota exceeded');
    } else {
      console.error('Failed to save pattern:', error);
    }
  }
}

export function loadPattern(): Pattern | null {
  try {
    const json = localStorage.getItem(STORAGE_KEY);
    if (!json) return null;

    const pattern = JSON.parse(json) as Pattern;

    // Validate structure
    if (
      !pattern.name ||
      !pattern.bpm ||
      !pattern.steps ||
      !Array.isArray(pattern.steps.BD) ||
      pattern.steps.BD.length !== 16
    ) {
      console.error('Invalid pattern structure');
      return null;
    }

    return pattern;
  } catch (error) {
    console.error('Failed to load pattern:', error);
    return null;
  }
}

export function clearPattern(): void {
  localStorage.removeItem(STORAGE_KEY);
}
```

### Create `tests/unit/storage.test.ts`
```typescript
import { describe, it, expect, beforeEach } from 'vitest';
import { savePattern, loadPattern, clearPattern } from '../../src/storage/localStorage';
import { createEmptyPattern } from '../../src/state/pattern';

describe('localStorage', () => {
  beforeEach(() => {
    clearPattern();
  });

  it('should save and load pattern', () => {
    const pattern = createEmptyPattern();
    pattern.name = 'Test Pattern';
    pattern.bpm = 140;

    savePattern(pattern);
    const loaded = loadPattern();

    expect(loaded).not.toBeNull();
    expect(loaded?.name).toBe('Test Pattern');
    expect(loaded?.bpm).toBe(140);
  });

  it('should return null if no pattern saved', () => {
    const loaded = loadPattern();
    expect(loaded).toBeNull();
  });

  it('should handle corrupted data', () => {
    localStorage.setItem('tr808:v1:pattern', 'invalid json');
    const loaded = loadPattern();
    expect(loaded).toBeNull();
  });
});
```

---

## Affected Files

**New:**
- `src/storage/localStorage.ts`
- `tests/unit/storage.test.ts`

---

## Tests

- Unit: Save/load round-trip
- Unit: Handle corrupted data
- Unit: Handle quota exceeded

---

## Definition of Done

- [ ] localStorage implemented
- [ ] Unit tests pass
- [ ] Error handling works
- [ ] Committed: "feat(storage): add localStorage persistence"
