import type { Pattern } from './types';

export function createEmptyPattern(): Pattern {
  return {
    name: 'Untitled',
    bpm: 120,
    steps: {
      BD: Array(16).fill(false),
      SD: Array(16).fill(false),
      LT: Array(16).fill(false),
      MT: Array(16).fill(false),
      HT: Array(16).fill(false),
      RS: Array(16).fill(false),
      CP: Array(16).fill(false),
      CH: Array(16).fill(false),
      OH: Array(16).fill(false),
      CY: Array(16).fill(false),
      RD: Array(16).fill(false),
      CB: Array(16).fill(false),
    },
  };
}

export function createDefaultPattern(): Pattern {
  // Classic 2‑step UK Garage feel: swung hats, syncopated kicks, clap/snare on 2 & 4
  return {
    name: 'Atmospheric UK Garage — Classic 2‑Step',
    bpm: 134,
    steps: {
      // BD (kick): 1, syncopation on the & of 2 and & of 3
      // idx: 0 (1), 6 (2&), 10 (3&)
      BD: [true, false, false, false, false, false, true, false, false, false, true, false, false, false, false, false],

      // SD (snare): backbeat on 2 and 4
      // idx: 4, 12
      SD: [false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false],

      // LT/MT/HT: off by default (leave space for sub/bassline); use later as fills
      LT: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
      MT: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
      HT: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],

      // RS (rimshot): light ghost before 4
      // idx: 11
      RS: [false, false, false, false, false, false, false, false, false, false, false, true, false, false, false, false],

      // CP (clap): layer with snare on 2 and 4
      CP: [false, false, false, false, true, false, false, false, false, false, false, false, true, false, false, false],

      // CH (closed hat): swung 16ths with gaps — swing affects odd steps
      // pattern: 0,2,3,6,8,10,11,14
      CH: [true, false, true, true, false, false, true, false, true, false, true, true, false, false, true, false],

      // OH (open hat): off‑beats with air — keep sparse
      // idx: 7, 10
      OH: [false, false, false, false, false, false, false, true, false, false, true, false, false, false, false, false],

      // CY/RD/CB: empty by default for this style
      CY: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
      RD: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
      CB: [false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false],
    },
  };
}

export function toggleStep(
  pattern: Pattern,
  instrument: keyof Pattern['steps'],
  step: number
): Pattern {
  return {
    ...pattern,
    steps: {
      ...pattern.steps,
      [instrument]: pattern.steps[instrument].map((active, i) =>
        i === step ? !active : active
      ),
    },
  };
}
