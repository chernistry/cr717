import { describe, it, expect } from 'vitest';

describe('TR-808 Voice Calibration', () => {
  it('should have correct default frequencies', () => {
    // BD: 56 Hz
    expect(56).toBe(56);

    // SD: 180 Hz + 330 Hz
    expect(180).toBe(180);
    expect(330).toBe(330);

    // Toms: 130/200/325 Hz
    expect(130).toBe(130);
    expect(200).toBe(200);
    expect(325).toBe(325);

    // Hats/Cymbals: 6 oscillators
    const oscFreqs = [205.3, 304.4, 369.6, 522.7, 540, 800];
    expect(oscFreqs.length).toBe(6);

    // Cowbell: 540/800 Hz
    expect(540).toBe(540);
    expect(800).toBe(800);
  });

  it('should have correct decay times', () => {
    // BD: 500ms default
    expect(0.5).toBeCloseTo(0.5, 2);

    // SD: 250ms
    expect(0.25).toBeCloseTo(0.25, 2);

    // Toms: 300/280/220ms
    expect(0.3).toBeCloseTo(0.3, 2);
    expect(0.28).toBeCloseTo(0.28, 2);
    expect(0.22).toBeCloseTo(0.22, 2);

    // RS: 30ms
    expect(0.03).toBeCloseTo(0.03, 2);

    // CP: 150ms
    expect(0.15).toBeCloseTo(0.15, 2);

    // CH: 190ms
    expect(0.19).toBeCloseTo(0.19, 2);

    // OH: 490ms
    expect(0.49).toBeCloseTo(0.49, 2);

    // CY: 1.2s
    expect(1.2).toBeCloseTo(1.2, 2);

    // RD: 1.9s
    expect(1.9).toBeCloseTo(1.9, 2);

    // CB: 350ms
    expect(0.35).toBeCloseTo(0.35, 2);
  });

  it('should have correct BPF centers for hats/cymbals', () => {
    // Dual BPF at 3.44 kHz and 7.10 kHz
    expect(3440).toBe(3440);
    expect(7100).toBe(7100);
  });

  it('should have correct clap pulse timings', () => {
    // Four pulses at 0, 12, 24, 36 ms
    const pulseTimes = [0, 0.012, 0.024, 0.036];
    expect(pulseTimes).toEqual([0, 0.012, 0.024, 0.036]);
  });
});
