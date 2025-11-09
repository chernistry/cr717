# Ticket 16: Per-Voice Parameter Controls (Knobs!)

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 6-8 hours  
**Dependencies:** Ticket 15

---

## Objective

Add real-time parameter controls for each voice - the "knobs" that make CR-717 legendary. Each voice needs tunable parameters that affect synthesis in real-time.

---

## Background

Original CR-717 has these controls per voice:

**Bass Drum:**
- Level (volume)
- Tone (filter cutoff)
- Decay (envelope length)
- Tuning (pitch)

**Snare Drum:**
- Level
- Tone (noise/tone balance)
- Snappy (noise amount)
- Tuning

**Toms (LT/MT/HT):**
- Level
- Tuning
- Decay

**Rimshot:**
- Level

**Clap:**
- Level

**Hi-Hats (CH/OH):**
- Level
- Decay (OH only)

**Cymbals (CY/RD):**
- Level
- Tone (filter)
- Decay

**Cowbell:**
- Level

---

## Acceptance Criteria

- ✅ Each voice has 2-4 knobs for real-time control
- ✅ Knobs update synthesis parameters immediately
- ✅ Knob UI: rotary SVG knobs with value display
- ✅ Mouse drag to adjust (vertical drag)
- ✅ Alt+drag for fine control (0.1x speed)
- ✅ Double-click to reset to default
- ✅ MIDI CC mappable (future-proof)
- ✅ Parameters saved in pattern
- ✅ Smooth parameter interpolation (no clicks)

---

## Implementation

### 1. Create Knob Component

**src/ui/controls/Knob.ts:**
```typescript
export class Knob {
  private element: HTMLElement;
  private value: number;
  private min: number;
  private max: number;
  private default: number;
  private onChange: (value: number) => void;
  private isDragging = false;
  private startY = 0;
  private startValue = 0;

  constructor(
    container: HTMLElement,
    label: string,
    min: number,
    max: number,
    defaultValue: number,
    onChange: (value: number) => void
  ) {
    this.min = min;
    this.max = max;
    this.default = defaultValue;
    this.value = defaultValue;
    this.onChange = onChange;

    this.element = document.createElement('div');
    this.element.className = 'knob-control';
    this.element.innerHTML = `
      <div class="knob-label">${label}</div>
      <svg class="knob-svg" viewBox="0 0 100 100" width="60" height="60">
        <circle cx="50" cy="50" r="40" fill="var(--color-bg-tertiary)" 
                stroke="var(--color-border)" stroke-width="2"/>
        <path class="knob-arc" d="" fill="none" 
              stroke="var(--color-accent)" stroke-width="4" stroke-linecap="round"/>
        <line class="knob-pointer" x1="50" y1="50" x2="50" y2="20" 
              stroke="var(--color-text-primary)" stroke-width="3" stroke-linecap="round"/>
      </svg>
      <div class="knob-value">${defaultValue.toFixed(1)}</div>
    `;

    container.appendChild(this.element);
    this.updateVisual();
    this.attachEvents();
  }

  private attachEvents(): void {
    const svg = this.element.querySelector('.knob-svg') as SVGElement;

    svg.addEventListener('mousedown', (e) => {
      this.isDragging = true;
      this.startY = e.clientY;
      this.startValue = this.value;
      e.preventDefault();
    });

    document.addEventListener('mousemove', (e) => {
      if (!this.isDragging) return;

      const sensitivity = e.altKey ? 0.1 : 1.0;
      const delta = (this.startY - e.clientY) * sensitivity;
      const range = this.max - this.min;
      const newValue = this.startValue + (delta / 100) * range;

      this.setValue(Math.max(this.min, Math.min(this.max, newValue)));
    });

    document.addEventListener('mouseup', () => {
      this.isDragging = false;
    });

    svg.addEventListener('dblclick', () => {
      this.setValue(this.default);
    });
  }

  setValue(value: number): void {
    this.value = value;
    this.updateVisual();
    this.onChange(value);
  }

  private updateVisual(): void {
    const percent = (this.value - this.min) / (this.max - this.min);
    const angle = -135 + percent * 270; // -135° to +135°

    // Update pointer rotation
    const pointer = this.element.querySelector('.knob-pointer') as SVGLineElement;
    pointer.setAttribute('transform', `rotate(${angle} 50 50)`);

    // Update arc
    const startAngle = -135;
    const endAngle = angle;
    const arc = this.describeArc(50, 50, 35, startAngle, endAngle);
    const arcPath = this.element.querySelector('.knob-arc') as SVGPathElement;
    arcPath.setAttribute('d', arc);

    // Update value display
    const valueDisplay = this.element.querySelector('.knob-value') as HTMLElement;
    valueDisplay.textContent = this.value.toFixed(1);
  }

  private describeArc(x: number, y: number, radius: number, startAngle: number, endAngle: number): string {
    const start = this.polarToCartesian(x, y, radius, endAngle);
    const end = this.polarToCartesian(x, y, radius, startAngle);
    const largeArcFlag = endAngle - startAngle <= 180 ? '0' : '1';
    return `M ${start.x} ${start.y} A ${radius} ${radius} 0 ${largeArcFlag} 0 ${end.x} ${end.y}`;
  }

  private polarToCartesian(centerX: number, centerY: number, radius: number, angleInDegrees: number) {
    const angleInRadians = ((angleInDegrees - 90) * Math.PI) / 180.0;
    return {
      x: centerX + radius * Math.cos(angleInRadians),
      y: centerY + radius * Math.sin(angleInRadians),
    };
  }
}
```

### 2. Update Voice Synthesis with Parameters

**Example: Bass Drum with parameters**

**src/audio/voices/bassDrum.ts:**
```typescript
export interface BDParams {
  level: number;    // 0-1
  tone: number;     // 0-1 (filter cutoff)
  decay: number;    // 0.1-1.0 (seconds)
  tuning: number;   // 0.5-2.0 (pitch multiplier)
}

const defaultBDParams: BDParams = {
  level: 1.0,
  tone: 0.5,
  decay: 0.35,
  tuning: 1.0,
};

export function playBD(
  ctx: AudioContext,
  time: number,
  params: Partial<BDParams> = {}
): void {
  const p = { ...defaultBDParams, ...params };

  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 * p.tuning });
  const filter = new BiquadFilterNode(ctx, {
    type: 'lowpass',
    frequency: 200 + p.tone * 800, // 200Hz - 1000Hz
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: p.level });

  osc.connect(filter).connect(gain).connect(ctx.destination);

  // Pitch envelope
  osc.frequency.setValueAtTime(150 * p.tuning, time);
  osc.frequency.exponentialRampToValueAtTime(50 * p.tuning, time + 0.08);

  // Amp envelope with variable decay
  gain.gain.setValueAtTime(p.level, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + p.decay);

  osc.start(time);
  osc.stop(time + p.decay + 0.1);
}
```

### 3. Create Voice Parameter Panel

**src/ui/voiceParams.ts:**
```typescript
import { Knob } from './controls/Knob';

export class VoiceParamsPanel {
  private container: HTMLElement;
  private currentVoice: string | null = null;
  private knobs: Map<string, Knob> = new Map();

  constructor(containerId: string) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
  }

  showVoiceParams(voice: string, params: Record<string, any>): void {
    this.currentVoice = voice;
    this.container.innerHTML = `<h3 class="panel-title">${voice} Parameters</h3>`;
    this.knobs.clear();

    const knobContainer = document.createElement('div');
    knobContainer.className = 'knob-container';

    // Create knobs based on voice type
    if (voice === 'BD') {
      this.addKnob(knobContainer, 'Level', 0, 1, params.level || 1.0, (v) => {
        this.updateParam('level', v);
      });
      this.addKnob(knobContainer, 'Tone', 0, 1, params.tone || 0.5, (v) => {
        this.updateParam('tone', v);
      });
      this.addKnob(knobContainer, 'Decay', 0.1, 1.0, params.decay || 0.35, (v) => {
        this.updateParam('decay', v);
      });
      this.addKnob(knobContainer, 'Tuning', 0.5, 2.0, params.tuning || 1.0, (v) => {
        this.updateParam('tuning', v);
      });
    }
    // ... similar for other voices

    this.container.appendChild(knobContainer);
  }

  private addKnob(
    container: HTMLElement,
    label: string,
    min: number,
    max: number,
    value: number,
    onChange: (value: number) => void
  ): void {
    const knob = new Knob(container, label, min, max, value, onChange);
    this.knobs.set(label, knob);
  }

  private updateParam(param: string, value: number): void {
    // Emit event or call callback
    const event = new CustomEvent('paramchange', {
      detail: { voice: this.currentVoice, param, value },
    });
    this.container.dispatchEvent(event);
  }
}
```

### 4. Update Pattern State

**src/state/types.ts:**
```typescript
export interface VoiceParams {
  BD: { level: number; tone: number; decay: number; tuning: number };
  SD: { level: number; tone: number; snappy: number; tuning: number };
  // ... etc for all voices
}

export interface Pattern {
  name: string;
  bpm: number;
  steps: { /* ... */ };
  params: VoiceParams; // NEW
}
```

### 5. CSS for Knobs

```css
.knob-control {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: 4px;
}

.knob-label {
  font-size: 11px;
  color: var(--color-text-secondary);
  text-transform: uppercase;
}

.knob-svg {
  cursor: ns-resize;
  user-select: none;
}

.knob-svg:hover circle {
  fill: var(--color-bg-elevated);
}

.knob-value {
  font-size: 12px;
  font-weight: bold;
  color: var(--color-accent);
  min-width: 40px;
  text-align: center;
}

.knob-container {
  display: flex;
  gap: 16px;
  flex-wrap: wrap;
  padding: 16px;
  background: var(--color-bg-tertiary);
  border-radius: var(--radius-md);
}
```

---

## Tests

- Unit: Knob value updates correctly
- Unit: Parameter changes affect synthesis
- E2E: Drag knob changes sound
- E2E: Double-click resets to default
- E2E: Alt+drag for fine control

---

## Affected Files

**New:**
- `src/ui/controls/Knob.ts`
- `src/ui/voiceParams.ts`

**Modified:**
- All voice files in `src/audio/voices/` - add parameter support
- `src/state/types.ts` - add VoiceParams
- `src/main.ts` - wire parameter changes
- `styles.css` - knob styles

---

## Definition of Done

- [ ] Knob component working
- [ ] All voices have parameter controls
- [ ] Real-time parameter updates
- [ ] Parameters saved in pattern
- [ ] UI polished and responsive
- [ ] Tests pass
- [ ] Committed: "feat(ui): add per-voice parameter knobs with real-time control"
