import { describe, it, expect } from 'vitest';
import { createEmptyPattern, toggleStep } from '../../src/state/pattern';
import { store } from '../../src/state/store';

describe('Pattern', () => {
  it('should create empty pattern', () => {
    const pattern = createEmptyPattern();
    expect(pattern.bpm).toBe(120);
    expect(pattern.steps.BD).toHaveLength(16);
    expect(pattern.steps.BD.every((s) => s === false)).toBe(true);
  });

  it('should toggle step', () => {
    const pattern = createEmptyPattern();
    const updated = toggleStep(pattern, 'BD', 0);
    expect(updated.steps.BD[0]).toBe(true);
    expect(updated.steps.BD[1]).toBe(false);
  });

  it('should toggle step back', () => {
    const pattern = createEmptyPattern();
    const updated = toggleStep(pattern, 'BD', 0);
    const toggled = toggleStep(updated, 'BD', 0);
    expect(toggled.steps.BD[0]).toBe(false);
  });
});

describe('Store', () => {
  it('should have initial state', () => {
    const state = store.getState();
    expect(state.pattern.bpm).toBe(120);
    expect(state.playback.isPlaying).toBe(false);
  });

  it('should update state', () => {
    store.setState({ playback: { isPlaying: true, currentStep: 5, bpm: 140 } });
    const state = store.getState();
    expect(state.playback.isPlaying).toBe(true);
    expect(state.playback.bpm).toBe(140);
  });

  it('should notify subscribers', () => {
    let called = false;
    const unsubscribe = store.subscribe(() => {
      called = true;
    });
    store.setState({ playback: { isPlaying: false, currentStep: 0, bpm: 120 } });
    expect(called).toBe(true);
    unsubscribe();
  });
});
