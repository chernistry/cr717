export interface Pattern {
  name: string;
  bpm: number;
  steps: {
    BD: boolean[];
    SD: boolean[];
    CH: boolean[];
    OH: boolean[];
    CP: boolean[];
  };
}

export interface PlaybackState {
  isPlaying: boolean;
  currentStep: number;
  bpm: number;
}

export interface AppState {
  pattern: Pattern;
  playback: PlaybackState;
}
