# Ticket 08: Playback Controls

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Tickets 02, 07

---

## Objective

Create Play/Stop button and BPM slider with gesture-gated audio.

---

## Acceptance Criteria

- ✅ Play button starts playback (gesture-gated)
- ✅ Stop button stops playback
- ✅ BPM slider (60-240) updates tempo
- ✅ Visual feedback (playing/stopped state)
- ✅ AudioContext resumes on first play

---

## Implementation

### Create `src/ui/controls.ts`
```typescript
import { initAudio } from '../audio/audioContext';

export interface ControlsCallbacks {
  onPlay: () => void;
  onStop: () => void;
  onBpmChange: (bpm: number) => void;
}

export class Controls {
  private playBtn: HTMLButtonElement;
  private stopBtn: HTMLButtonElement;
  private bpmSlider: HTMLInputElement;
  private bpmDisplay: HTMLElement;

  constructor(containerId: string, callbacks: ControlsCallbacks) {
    const container = document.getElementById(containerId);
    if (!container) throw new Error(`Container ${containerId} not found`);

    container.innerHTML = `
      <div class="controls-group">
        <button id="play-btn" class="control-btn">▶ Play</button>
        <button id="stop-btn" class="control-btn">■ Stop</button>
      </div>
      <div class="controls-group">
        <label for="bpm-slider">BPM: <span id="bpm-display">120</span></label>
        <input type="range" id="bpm-slider" min="60" max="240" value="120" />
      </div>
    `;

    this.playBtn = container.querySelector('#play-btn')!;
    this.stopBtn = container.querySelector('#stop-btn')!;
    this.bpmSlider = container.querySelector('#bpm-slider')!;
    this.bpmDisplay = container.querySelector('#bpm-display')!;

    this.playBtn.addEventListener('click', async () => {
      await initAudio(); // Gesture-gated
      callbacks.onPlay();
      this.setPlaying(true);
    });

    this.stopBtn.addEventListener('click', () => {
      callbacks.onStop();
      this.setPlaying(false);
    });

    this.bpmSlider.addEventListener('input', () => {
      const bpm = parseInt(this.bpmSlider.value, 10);
      this.bpmDisplay.textContent = bpm.toString();
      callbacks.onBpmChange(bpm);
    });
  }

  setPlaying(playing: boolean): void {
    this.playBtn.disabled = playing;
    this.stopBtn.disabled = !playing;
  }

  getBpm(): number {
    return parseInt(this.bpmSlider.value, 10);
  }
}
```

### Update `styles.css`
```css
.controls-group {
  margin-bottom: 20px;
}

.control-btn {
  padding: 12px 24px;
  font-size: 16px;
  margin-right: 10px;
  background: #ff6b35;
  border: none;
  color: white;
  cursor: pointer;
  border-radius: 4px;
}

.control-btn:hover:not(:disabled) {
  background: #ff8555;
}

.control-btn:disabled {
  opacity: 0.5;
  cursor: not-allowed;
}

#bpm-slider {
  width: 300px;
  margin-left: 10px;
}
```

---

## Affected Files

**New:**
- `src/ui/controls.ts`

**Modified:**
- `styles.css`

---

## Tests

- E2E: Click Play → verify audio starts
- E2E: Change BPM → verify callback
- E2E: Click Stop → verify audio stops

---

## Definition of Done

- [ ] Controls render
- [ ] Play/Stop work
- [ ] BPM slider works
- [ ] Gesture-gated audio
- [ ] Committed: "feat(ui): add playback controls"
