export interface Pattern {
  name: string;
  bpm: number;
  swing?: number; // 0.0–0.75 (optional)
  steps: {
    BD: boolean[];
    SD: boolean[];
    LT: boolean[];
    MT: boolean[];
    HT: boolean[];
    RS: boolean[];
    CP: boolean[];
    CH: boolean[];
    OH: boolean[];
    CY: boolean[];
    RD: boolean[];
    CB: boolean[];
  };
}

export interface PatternBank {
  patterns: Record<string, Pattern>; // A-H
  currentPattern: string; // 'A'
}

export interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  bpm: number;
}

export interface AppState {
  bank: PatternBank;
  playback: PlaybackState;
}
