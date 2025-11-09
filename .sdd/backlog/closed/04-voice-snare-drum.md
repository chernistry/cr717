# Ticket 04: Snare Drum (SD) Voice

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 02

---

## Objective

Implement snare drum synthesis with oscillator + noise components.

---

## Acceptance Criteria

- ✅ SD plays with tonal + noise mix
- ✅ Oscillator: 180Hz triangle wave
- ✅ Noise: highpass filtered (1kHz)
- ✅ Envelope: 150ms decay
- ✅ Unit tests pass

---

## Implementation

### Create `src/audio/voices/snareDrum.ts`
```typescript
export function playSD(ctx: AudioContext, time: number): void {
  // Tonal component
  const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 });
  const oscGain = new GainNode(ctx, { gain: 0.7 });

  // Noise component
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const noise = new AudioBufferSourceNode(ctx, { buffer });
  const noiseFilter = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 1000,
    Q: 1,
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

### Update `tests/unit/voices.test.ts`
```typescript
import { playSD } from '../../src/audio/voices/snareDrum';

describe('Snare Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playSD(ctx, 0);
    const buffer = await ctx.startRendering();
    
    const data = buffer.getChannelData(0);
    const hasSound = data.some(v => Math.abs(v) > 0.001);
    expect(hasSound).toBe(true);
  });
});
```

---

## Affected Files

**New:**
- `src/audio/voices/snareDrum.ts`

**Modified:**
- `tests/unit/voices.test.ts`

---

## Definition of Done

- [ ] SD synthesis implemented
- [ ] Unit tests pass
- [ ] Sound verified
- [ ] Committed: "feat(audio): add snare drum synthesis"
