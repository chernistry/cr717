# Ticket 19: MIDI Input/Output

**Status:** 🟡 Open  
**Priority:** P3 (Nice to have)  
**Estimated Time:** 6-8 hours  
**Dependencies:** Ticket 16

---

## Objective

MIDI support for external controllers and DAW integration. Essential for professional use.

---

## Background

Web MIDI API allows:
- MIDI input: trigger voices, control parameters
- MIDI output: send clock, notes for recording in DAW
- MIDI learn: map any CC to any parameter

Standard MIDI mapping for TR-808:
- Notes 36-47: Trigger voices (C2-B2)
- CC 1-127: Parameter controls
- Clock: 24 PPQN (pulses per quarter note)

---

## Acceptance Criteria

- ✅ MIDI input device selection
- ✅ Note-on triggers voices (36-47)
- ✅ CC controls parameters (MIDI learn)
- ✅ MIDI clock sync (slave mode)
- ✅ MIDI output: send notes + clock
- ✅ Visual MIDI activity indicator
- ✅ MIDI mapping saved per pattern
- ✅ Works with common controllers (Launchpad, MPK, etc)

---

## Implementation

### 1. MIDI Manager

**src/midi/midiManager.ts:**
```typescript
export class MIDIManager {
  private midiAccess: MIDIAccess | null = null;
  private inputs: Map<string, MIDIInput> = new Map();
  private outputs: Map<string, MIDIOutput> = new Map();
  private noteMap: Map<number, string> = new Map([
    [36, 'BD'], // C2
    [37, 'SD'], // C#2
    [38, 'LT'], // D2
    [39, 'MT'], // D#2
    [40, 'HT'], // E2
    [41, 'RS'], // F2
    [42, 'CP'], // F#2
    [43, 'CH'], // G2
    [44, 'OH'], // G#2
    [45, 'CY'], // A2
    [46, 'RD'], // A#2
    [47, 'CB'], // B2
  ]);

  async init(): Promise<void> {
    if (!navigator.requestMIDIAccess) {
      throw new Error('Web MIDI API not supported');
    }

    this.midiAccess = await navigator.requestMIDIAccess();

    // List inputs
    this.midiAccess.inputs.forEach((input) => {
      this.inputs.set(input.id, input);
      console.log(`MIDI Input: ${input.name}`);
    });

    // List outputs
    this.midiAccess.outputs.forEach((output) => {
      this.outputs.set(output.id, output);
      console.log(`MIDI Output: ${output.name}`);
    });
  }

  connectInput(
    inputId: string,
    onNote: (voice: string, velocity: number) => void,
    onCC: (cc: number, value: number) => void
  ): void {
    const input = this.inputs.get(inputId);
    if (!input) return;

    input.onmidimessage = (event) => {
      const [status, data1, data2] = event.data;
      const command = status & 0xf0;

      if (command === 0x90 && data2 > 0) {
        // Note On
        const voice = this.noteMap.get(data1);
        if (voice) {
          onNote(voice, data2 / 127);
        }
      } else if (command === 0xb0) {
        // Control Change
        onCC(data1, data2 / 127);
      }
    };
  }

  sendNote(outputId: string, note: number, velocity: number, duration: number): void {
    const output = this.outputs.get(outputId);
    if (!output) return;

    // Note On
    output.send([0x90, note, Math.floor(velocity * 127)]);

    // Note Off after duration
    setTimeout(() => {
      output.send([0x80, note, 0]);
    }, duration * 1000);
  }

  sendClock(outputId: string): void {
    const output = this.outputs.get(outputId);
    if (!output) return;

    // MIDI Clock (0xF8)
    output.send([0xf8]);
  }

  sendStart(outputId: string): void {
    const output = this.outputs.get(outputId);
    if (!output) return;

    output.send([0xfa]); // Start
  }

  sendStop(outputId: string): void {
    const output = this.outputs.get(outputId);
    if (!output) return;

    output.send([0xfc]); // Stop
  }
}
```

### 2. MIDI Settings Panel

**src/ui/midiSettings.ts:**
```typescript
export class MIDISettings {
  private container: HTMLElement;
  private midiManager: MIDIManager;

  constructor(containerId: string, midiManager: MIDIManager) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.midiManager = midiManager;
    this.render();
  }

  private render(): void {
    this.container.innerHTML = `
      <div class="midi-settings">
        <h3>MIDI Settings</h3>
        
        <div class="midi-section">
          <label>Input Device</label>
          <select id="midi-input-select">
            <option value="">None</option>
          </select>
        </div>
        
        <div class="midi-section">
          <label>Output Device</label>
          <select id="midi-output-select">
            <option value="">None</option>
          </select>
        </div>
        
        <div class="midi-section">
          <label>
            <input type="checkbox" id="midi-clock-sync"> 
            Sync to MIDI Clock
          </label>
        </div>
        
        <div class="midi-section">
          <label>
            <input type="checkbox" id="midi-send-clock"> 
            Send MIDI Clock
          </label>
        </div>
        
        <div class="midi-activity">
          <span class="midi-indicator" id="midi-in-indicator">IN</span>
          <span class="midi-indicator" id="midi-out-indicator">OUT</span>
        </div>
      </div>
    `;

    this.populateDevices();
  }

  private async populateDevices(): Promise<void> {
    await this.midiManager.init();

    const inputSelect = document.getElementById(
      'midi-input-select'
    ) as HTMLSelectElement;
    const outputSelect = document.getElementById(
      'midi-output-select'
    ) as HTMLSelectElement;

    // Populate inputs
    this.midiManager.getInputs().forEach((input) => {
      const option = document.createElement('option');
      option.value = input.id;
      option.textContent = input.name;
      inputSelect.appendChild(option);
    });

    // Populate outputs
    this.midiManager.getOutputs().forEach((output) => {
      const option = document.createElement('option');
      option.value = output.id;
      option.textContent = output.name;
      outputSelect.appendChild(option);
    });
  }

  flashIndicator(type: 'in' | 'out'): void {
    const indicator = document.getElementById(`midi-${type}-indicator`);
    if (!indicator) return;

    indicator.classList.add('active');
    setTimeout(() => {
      indicator.classList.remove('active');
    }, 100);
  }
}
```

### 3. MIDI Learn Mode

**src/midi/midiLearn.ts:**
```typescript
export class MIDILearn {
  private learning = false;
  private currentParam: string | null = null;
  private mappings: Map<number, string> = new Map();

  startLearning(param: string): void {
    this.learning = true;
    this.currentParam = param;
    console.log(`MIDI Learn: waiting for CC for ${param}`);
  }

  handleCC(cc: number, value: number): boolean {
    if (this.learning && this.currentParam) {
      this.mappings.set(cc, this.currentParam);
      console.log(`Mapped CC${cc} → ${this.currentParam}`);
      this.learning = false;
      this.currentParam = null;
      return true;
    }

    // Check if CC is mapped
    const param = this.mappings.get(cc);
    if (param) {
      // Emit parameter change
      document.dispatchEvent(
        new CustomEvent('midiparamchange', {
          detail: { param, value },
        })
      );
      return true;
    }

    return false;
  }

  getMappings(): Map<number, string> {
    return this.mappings;
  }

  clearMapping(cc: number): void {
    this.mappings.delete(cc);
  }
}
```

### 4. CSS

```css
.midi-settings {
  padding: 16px;
  background: var(--color-bg-secondary);
  border-radius: var(--radius-md);
}

.midi-section {
  margin-bottom: 12px;
}

.midi-section label {
  display: block;
  margin-bottom: 4px;
  font-size: 12px;
  color: var(--color-text-secondary);
}

.midi-section select {
  width: 100%;
  padding: 6px;
  background: var(--color-bg-tertiary);
  border: 1px solid var(--color-border);
  color: var(--color-text-primary);
  border-radius: var(--radius-sm);
}

.midi-activity {
  display: flex;
  gap: 8px;
  margin-top: 16px;
}

.midi-indicator {
  padding: 4px 8px;
  background: var(--color-bg-tertiary);
  border: 1px solid var(--color-border);
  border-radius: var(--radius-sm);
  font-size: 10px;
  font-weight: bold;
  color: var(--color-text-muted);
  transition: all 0.1s;
}

.midi-indicator.active {
  background: var(--color-success);
  color: white;
  border-color: var(--color-success);
}
```

---

## Tests

- Unit: MIDI note mapping correct
- Unit: CC values normalized (0-1)
- E2E: MIDI input triggers voices
- E2E: MIDI learn maps CC to parameter
- Manual: Test with real MIDI controller

---

## Affected Files

**New:**
- `src/midi/midiManager.ts`
- `src/midi/midiLearn.ts`
- `src/ui/midiSettings.ts`

**Modified:**
- `src/main.ts` - Wire MIDI
- `index.html` - MIDI settings panel
- `styles.css` - MIDI UI styles

---

## Definition of Done

- [ ] MIDI input working
- [ ] MIDI output working
- [ ] MIDI learn functional
- [ ] Clock sync working
- [ ] Device selection UI
- [ ] Tests pass
- [ ] Committed: "feat(midi): add MIDI I/O with learn mode and clock sync"
