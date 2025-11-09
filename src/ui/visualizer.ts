import { getAudioContext } from '../audio/audioContext';

export class Visualizer {
  private rafId: number | null = null;
  private onStepChange: (step: number) => void;
  private bpm: number;

  constructor(onStepChange: (step: number) => void) {
    this.onStepChange = onStepChange;
    this.bpm = 120;
  }

  start(bpm: number): void {
    this.bpm = bpm;
    this.tick();
  }

  stop(): void {
    if (this.rafId !== null) {
      cancelAnimationFrame(this.rafId);
      this.rafId = null;
    }
  }

  private tick = (): void => {
    const ctx = getAudioContext();
    if (!ctx) return;

    // Use getOutputTimestamp() for precise audio-visual sync
    const timestamp = ctx.getOutputTimestamp();
    const contextTime = timestamp.contextTime ?? ctx.currentTime;
    const stepDuration = 60 / this.bpm / 4;
    const currentStep = Math.floor(contextTime / stepDuration) % 16;

    this.onStepChange(currentStep);

    this.rafId = requestAnimationFrame(this.tick);
  };
}
