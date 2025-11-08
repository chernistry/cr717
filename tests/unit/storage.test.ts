import { describe, it, expect, beforeEach } from 'vitest';
import {
  savePattern,
  loadPattern,
  clearPattern,
} from '../../src/storage/localStorage';
import { createEmptyPattern } from '../../src/state/pattern';

describe('localStorage', () => {
  beforeEach(() => {
    clearPattern();
  });

  it('should save and load pattern', () => {
    const pattern = createEmptyPattern();
    pattern.name = 'Test Pattern';
    pattern.bpm = 140;

    savePattern(pattern);
    const loaded = loadPattern();

    expect(loaded).not.toBeNull();
    expect(loaded?.name).toBe('Test Pattern');
    expect(loaded?.bpm).toBe(140);
  });

  it('should return null if no pattern saved', () => {
    const loaded = loadPattern();
    expect(loaded).toBeNull();
  });

  it('should handle corrupted data', () => {
    localStorage.setItem('tr808:v1:pattern', 'invalid json');
    const loaded = loadPattern();
    expect(loaded).toBeNull();
  });

  it('should clear pattern', () => {
    const pattern = createEmptyPattern();
    savePattern(pattern);
    clearPattern();
    const loaded = loadPattern();
    expect(loaded).toBeNull();
  });
});
