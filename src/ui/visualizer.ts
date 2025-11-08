import { getAudioContext } from '../audio/audioContext';

export class Visualizer {
  private rafId: number | null = null;
  private onStepChange: (step: number) => void;
  private bpm: number;
  private startTime: number = 0;

  constructor(onStepChange: (step: number) => void) {
    this.onStepChange = onStepChange;
    this.bpm = 120;
  }

  start(bpm: number): void {
    this.bpm = bpm;
    const ctx = getAudioContext();
    if (!ctx) return;

    this.startTime = ctx.currentTime;
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

    const stepDuration = 60 / this.bpm / 4;
    const elapsed = ctx.currentTime - this.startTime;
    const currentStep = Math.floor(elapsed / stepDuration) % 16;

    this.onStepChange(currentStep);

    this.rafId = requestAnimationFrame(this.tick);
  };
}
