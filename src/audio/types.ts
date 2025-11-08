export interface SchedulerCallback {
  (step: number, time: number): void;
}

export interface Scheduler {
  start(ctx: AudioContext, bpm: number, onStep: SchedulerCallback): void;
  stop(): void;
  getCurrentStep(): number;
  isRunning(): boolean;
}
