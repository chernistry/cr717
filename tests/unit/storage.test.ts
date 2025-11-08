import { describe, it, expect, beforeEach } from 'vitest';
import {
  saveBank,
  loadBank,
  clearBank,
} from '../../src/storage/localStorage';
import { createEmptyPattern, createDefaultPattern } from '../../src/state/pattern';
import type { PatternBank } from '../../src/state/types';

describe('localStorage', () => {
  beforeEach(() => {
    clearBank();
  });

  it('should save and load bank', () => {
    const bank: PatternBank = {
      patterns: {
        A: createDefaultPattern(),
        B: createEmptyPattern(),
        C: createEmptyPattern(),
        D: createEmptyPattern(),
        E: createEmptyPattern(),
        F: createEmptyPattern(),
        G: createEmptyPattern(),
        H: createEmptyPattern(),
      },
      currentPattern: 'A',
    };

    saveBank(bank);
    const loaded = loadBank();

    expect(loaded).not.toBeNull();
    expect(loaded?.currentPattern).toBe('A');
    expect(loaded?.patterns.A.name).toBe('Atmospheric UK Garage — Classic 2‑Step');
  });

  it('should return null if no bank saved', () => {
    const loaded = loadBank();
    expect(loaded).toBeNull();
  });

  it('should handle corrupted data', () => {
    localStorage.setItem('tr808:v1:bank', 'invalid json');
    const loaded = loadBank();
    expect(loaded).toBeNull();
  });

  it('should clear bank', () => {
    const bank: PatternBank = {
      patterns: {
        A: createDefaultPattern(),
        B: createEmptyPattern(),
        C: createEmptyPattern(),
        D: createEmptyPattern(),
        E: createEmptyPattern(),
        F: createEmptyPattern(),
        G: createEmptyPattern(),
        H: createEmptyPattern(),
      },
      currentPattern: 'A',
    };
    saveBank(bank);
    clearBank();
    const loaded = loadBank();
    expect(loaded).toBeNull();
  });
});
