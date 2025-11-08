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
      await initAudio();
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
