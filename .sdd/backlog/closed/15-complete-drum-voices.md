# Ticket 15: Complete CR-717 Drum Voice Set

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 4-6 hours  
**Dependencies:** Tickets 03-06

---

## Objective

Add missing CR-717 voices to match original: Low/Mid/High Toms, Rimshot, Cowbell, Cymbal, Accent.

---

## Background

Original CR-717 has 16 voices total:
- ✅ Bass Drum (BD)
- ✅ Snare Drum (SD)
- ✅ Low Tom (LT) - MISSING
- ✅ Mid Tom (MT) - MISSING
- ✅ High Tom (HT) - MISSING
- ✅ Rimshot (RS) - MISSING
- ✅ Clap (CP)
- ✅ Closed Hi-Hat (CH)
- ✅ Open Hi-Hat (OH)
- ✅ Crash Cymbal (CY) - MISSING
- ✅ Ride Cymbal (RD) - MISSING
- ✅ Cowbell (CB) - MISSING
- ✅ Accent (AC) - MISSING (per-step velocity)

Currently we only have 5 voices. Need 11 more.

---

## Acceptance Criteria

- ✅ Low Tom: 65Hz sine with pitch decay, 400ms
- ✅ Mid Tom: 90Hz sine with pitch decay, 350ms
- ✅ High Tom: 130Hz sine with pitch decay, 300ms
- ✅ Rimshot: 2 oscillators (180Hz + 330Hz) + noise burst, 50ms
- ✅ Cowbell: 540Hz + 800Hz square waves, bandpass filter, 200ms
- ✅ Crash Cymbal: filtered noise (highpass 5kHz), 800ms decay
- ✅ Ride Cymbal: filtered noise (bandpass 8kHz), 400ms decay
- ✅ Accent: per-step velocity multiplier (0.5x to 2x)
- ✅ All voices unit tested with OfflineAudioContext
- ✅ UI updated with 16-row grid

---

## Implementation

### 1. Create Tom Voices

**src/audio/voices/lowTom.ts:**
```typescript
export function playLT(ctx: AudioContext, time: number): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 65 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  // Pitch envelope: 65Hz → 45Hz
  osc.frequency.setValueAtTime(65, time);
  osc.frequency.exponentialRampToValueAtTime(45, time + 0.1);

  // Amp envelope
  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.4);

  osc.start(time);
  osc.stop(time + 0.5);
}
```

**src/audio/voices/midTom.ts:**
```typescript
export function playMT(ctx: AudioContext, time: number): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 90 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  osc.frequency.setValueAtTime(90, time);
  osc.frequency.exponentialRampToValueAtTime(60, time + 0.08);

  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.35);

  osc.start(time);
  osc.stop(time + 0.4);
}
```

**src/audio/voices/highTom.ts:**
```typescript
export function playHT(ctx: AudioContext, time: number): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 130 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  osc.frequency.setValueAtTime(130, time);
  osc.frequency.exponentialRampToValueAtTime(90, time + 0.06);

  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.3);

  osc.start(time);
  osc.stop(time + 0.35);
}
```

### 2. Create Rimshot

**src/audio/voices/rimshot.ts:**
```typescript
export function playRS(ctx: AudioContext, time: number): void {
  // Two oscillators for metallic sound
  const osc1 = new OscillatorNode(ctx, { type: 'square', frequency: 180 });
  const osc2 = new OscillatorNode(ctx, { type: 'square', frequency: 330 });
  
  // Noise burst
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.05, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }
  const noise = new AudioBufferSourceNode(ctx, { buffer });
  
  const gain = new GainNode(ctx, { gain: 0.8 });
  
  osc1.connect(gain);
  osc2.connect(gain);
  noise.connect(gain);
  gain.connect(ctx.destination);
  
  gain.gain.setValueAtTime(0.8, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.05);
  
  osc1.start(time);
  osc2.start(time);
  noise.start(time);
  osc1.stop(time + 0.06);
  osc2.stop(time + 0.06);
}
```

### 3. Create Cowbell

**src/audio/voices/cowbell.ts:**
```typescript
export function playCB(ctx: AudioContext, time: number): void {
  const osc1 = new OscillatorNode(ctx, { type: 'square', frequency: 540 });
  const osc2 = new OscillatorNode(ctx, { type: 'square', frequency: 800 });
  
  const bp = new BiquadFilterNode(ctx, {
    type: 'bandpass',
    frequency: 800,
    Q: 5,
  });
  
  const gain = new GainNode(ctx, { gain: 0.7 });
  
  osc1.connect(bp);
  osc2.connect(bp);
  bp.connect(gain).connect(ctx.destination);
  
  gain.gain.setValueAtTime(0.7, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.2);
  
  osc1.start(time);
  osc2.start(time);
  osc1.stop(time + 0.25);
  osc2.stop(time + 0.25);
}
```

### 4. Create Cymbals

**src/audio/voices/crashCymbal.ts:**
```typescript
export function playCY(ctx: AudioContext, time: number): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 1.0, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }
  
  const src = new AudioBufferSourceNode(ctx, { buffer });
  const hp = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 5000,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: 0.6 });
  
  src.connect(hp).connect(gain).connect(ctx.destination);
  
  gain.gain.setValueAtTime(0.6, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.8);
  
  src.start(time);
}
```

**src/audio/voices/rideCymbal.ts:**
```typescript
export function playRD(ctx: AudioContext, time: number): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.5, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }
  
  const src = new AudioBufferSourceNode(ctx, { buffer });
  const bp = new BiquadFilterNode(ctx, {
    type: 'bandpass',
    frequency: 8000,
    Q: 2,
  });
  const gain = new GainNode(ctx, { gain: 0.5 });
  
  src.connect(bp).connect(gain).connect(ctx.destination);
  
  gain.gain.setValueAtTime(0.5, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.4);
  
  src.start(time);
}
```

### 5. Update State Types

**src/state/types.ts:**
```typescript
export interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[];
    SD: boolean[];
    LT: boolean[];  // NEW
    MT: boolean[];  // NEW
    HT: boolean[];  // NEW
    RS: boolean[];  // NEW
    CP: boolean[];
    CH: boolean[];
    OH: boolean[];
    CY: boolean[];  // NEW
    RD: boolean[];  // NEW
    CB: boolean[];  // NEW
  };
  accents: number[]; // NEW: 0-127 per step
}
```

### 6. Update UI Grid

- Expand grid to 12 rows (or scrollable)
- Add accent row with velocity bars
- Update CSS for compact layout

---

## Tests

- Unit: Each new voice renders without errors
- Unit: Accent multiplier affects gain correctly
- E2E: All 12 voices playable
- E2E: Accent changes volume

---

## Affected Files

**New:**
- `src/audio/voices/lowTom.ts`
- `src/audio/voices/midTom.ts`
- `src/audio/voices/highTom.ts`
- `src/audio/voices/rimshot.ts`
- `src/audio/voices/cowbell.ts`
- `src/audio/voices/crashCymbal.ts`
- `src/audio/voices/rideCymbal.ts`

**Modified:**
- `src/state/types.ts` - Add new voices to Pattern
- `src/state/pattern.ts` - Update createEmptyPattern
- `src/ui/sequencerGrid.ts` - Expand to 12 rows
- `src/main.ts` - Wire new voices
- `tests/unit/voices.test.ts` - Add tests

---

## Definition of Done

- [ ] All 7 new voices implemented
- [ ] Accent system working
- [ ] UI shows all 12 voices
- [ ] Unit tests pass
- [ ] E2E tests pass
- [ ] Sounds authentic to CR-717
- [ ] Committed: "feat(audio): add complete CR-717 voice set (12 voices + accent)"
