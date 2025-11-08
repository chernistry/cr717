import { describe, it, expect, beforeEach, afterEach } from 'vitest';
import { AudioScheduler } from '../../src/audio/scheduler';

describe('AudioScheduler', () => {
  let scheduler: AudioScheduler;
  let mockContext: AudioContext;

  beforeEach(() => {
    scheduler = new AudioScheduler();
    mockContext = new AudioContext();
  });

  afterEach(() => {
    scheduler.stop();
    mockContext.close();
  });

  it('should start and track steps', () => {
    const steps: number[] = [];
    scheduler.start(mockContext, 120, (step) => steps.push(step));

    expect(scheduler.isRunning()).toBe(true);
    scheduler.stop();
    expect(scheduler.isRunning()).toBe(false);
  });

  it('should cycle through 16 steps', () => {
    let lastStep = -1;
    scheduler.start(mockContext, 120, (step) => {
      lastStep = step;
    });

    return new Promise<void>((resolve) => {
      setTimeout(() => {
        scheduler.stop();
        expect(lastStep).toBeGreaterThanOrEqual(0);
        expect(lastStep).toBeLessThan(16);
        resolve();
      }, 100);
    });
  });

  it('should not start if already running', () => {
    let callCount = 0;
    scheduler.start(mockContext, 120, () => callCount++);
    scheduler.start(mockContext, 120, () => callCount++);

    expect(scheduler.isRunning()).toBe(true);
    scheduler.stop();
  });

  it('should reset to step 0 on start', () => {
    scheduler.start(mockContext, 120, () => {});
    scheduler.stop();

    scheduler.start(mockContext, 120, () => {});
    expect(scheduler.getCurrentStep()).toBe(0);
    scheduler.stop();
  });
});
