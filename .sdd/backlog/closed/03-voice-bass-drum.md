# Ticket 03: Bass Drum (BD) Voice

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** Ticket 02

---

## Objective

Implement authentic 808 bass drum synthesis with pitch envelope.

---

## Acceptance Criteria

- ✅ BD plays on trigger with 808-like sound
- ✅ Pitch drops from 150Hz to 50Hz over 80ms
- ✅ Amplitude envelope: attack 0ms, decay 350ms
- ✅ Unit test verifies envelope in OfflineAudioContext
- ✅ No clicks or pops

---

## Implementation

### Create `src/audio/voices/bassDrum.ts`
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

### Create `tests/unit/voices.test.ts`
```typescript
import { describe, it, expect } from 'vitest';
import { playBD } from '../../src/audio/voices/bassDrum';

describe('Bass Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();
    
    // Verify audio was generated
    const data = buffer.getChannelData(0);
    const hasSound = data.some(v => Math.abs(v) > 0.001);
    expect(hasSound).toBe(true);
  });

  it('should have correct duration', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();
    
    // Should have sound in first 0.5s
    expect(buffer.duration).toBe(1);
  });
});
```

---

## Affected Files

**New:**
- `src/audio/voices/bassDrum.ts`
- `tests/unit/voices.test.ts`

---

## Tests

- Unit: Render with OfflineAudioContext
- Unit: Verify audio output exists
- Integration: Play via scheduler

---

## Definition of Done

- [ ] BD synthesis implemented
- [ ] Unit tests pass
- [ ] Sound verified manually
- [ ] No clicks/pops
- [ ] Committed: "feat(audio): add bass drum synthesis"
