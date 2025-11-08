# Ticket 06: Clap (CP) Voice

**Status:** 🟡 Open  
**Priority:** P2 (Should have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** Ticket 02

---

## Objective

Implement clap synthesis with multiple noise bursts.

---

## Acceptance Criteria

- ✅ CP: 3-4 noise bursts with delays
- ✅ Bandpass filter (1-3kHz)
- ✅ Total duration ~100ms
- ✅ Unit tests pass

---

## Implementation

### Create `src/audio/voices/clap.ts`
```typescript
export function playCP(ctx: AudioContext, time: number): void {
  const delays = [0, 0.01, 0.02, 0.03];

  delays.forEach((delay) => {
    const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.05, ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < data.length; i++) {
      data[i] = Math.random() * 2 - 1;
    }

    const src = new AudioBufferSourceNode(ctx, { buffer });
    const bp = new BiquadFilterNode(ctx, {
      type: 'bandpass',
      frequency: 2000,
      Q: 2,
    });
    const gain = new GainNode(ctx, { gain: 0.6 });

    src.connect(bp).connect(gain).connect(ctx.destination);

    const t = time + delay;
    gain.gain.setValueAtTime(0.6, t);
    gain.gain.exponentialRampToValueAtTime(0.0001, t + 0.05);

    src.start(t);
  });
}
```

### Update `tests/unit/voices.test.ts`
```typescript
import { playCP } from '../../src/audio/voices/clap';

describe('Clap', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playCP(ctx, 0);
    const buffer = await ctx.startRendering();
    expect(buffer.getChannelData(0).some(v => Math.abs(v) > 0.001)).toBe(true);
  });
});
```

---

## Affected Files

**New:**
- `src/audio/voices/clap.ts`

**Modified:**
- `tests/unit/voices.test.ts`

---

## Definition of Done

- [ ] CP synthesis implemented
- [ ] Unit tests pass
- [ ] Sound verified
- [ ] Committed: "feat(audio): add clap synthesis"
