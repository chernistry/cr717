# Ticket 05: Hi-Hat Voices (CH & OH)

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** Ticket 02

---

## Objective

Implement closed and open hi-hat synthesis.

---

## Acceptance Criteria

- ✅ CH: Short filtered noise (70ms decay)
- ✅ OH: Longer filtered noise (200ms decay)
- ✅ Both use highpass filter (8kHz)
- ✅ Unit tests pass

---

## Implementation

### Create `src/audio/voices/closedHiHat.ts`
```typescript
export function playCH(ctx: AudioContext, time: number): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.1, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const hp = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 8000,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: 0.7 });

  src.connect(hp).connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.7, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.07);

  src.start(time);
}
```

### Create `src/audio/voices/openHiHat.ts`
```typescript
export function playOH(ctx: AudioContext, time: number): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.3, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const hp = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 8000,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: 0.8 });

  src.connect(hp).connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.8, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.2);

  src.start(time);
}
```

### Update `tests/unit/voices.test.ts`
```typescript
import { playCH } from '../../src/audio/voices/closedHiHat';
import { playOH } from '../../src/audio/voices/openHiHat';

describe('Closed Hi-Hat', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCH(ctx, 0);
    const buffer = await ctx.startRendering();
    expect(buffer.getChannelData(0).some(v => Math.abs(v) > 0.001)).toBe(true);
  });
});

describe('Open Hi-Hat', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playOH(ctx, 0);
    const buffer = await ctx.startRendering();
    expect(buffer.getChannelData(0).some(v => Math.abs(v) > 0.001)).toBe(true);
  });
});
```

---

## Affected Files

**New:**
- `src/audio/voices/closedHiHat.ts`
- `src/audio/voices/openHiHat.ts`

**Modified:**
- `tests/unit/voices.test.ts`

---

## Definition of Done

- [ ] CH & OH implemented
- [ ] Unit tests pass
- [ ] Sounds verified
- [ ] Committed: "feat(audio): add hi-hat voices"
