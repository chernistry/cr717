export interface VoiceParams {
  BD: { level: number; tone: number; decay: number; tuning: number };
  SD: { level: number; tone: number; snappy: number; tuning: number };
  LT: { level: number; tuning: number; decay: number };
  MT: { level: number; tuning: number; decay: number };
  HT: { level: number; tuning: number; decay: number };
  RS: { level: number };
  CP: { level: number };
  CH: { level: number; decay: number };
  OH: { level: number; decay: number };
  CY: { level: number; tone: number; decay: number };
  RD: { level: number; tone: number; decay: number };
  CB: { level: number };
}

export const defaultVoiceParams: VoiceParams = {
  BD: { level: 1.0, tone: 0.5, decay: 0.35, tuning: 1.0 },
  SD: { level: 1.0, tone: 0.5, snappy: 0.5, tuning: 1.0 },
  LT: { level: 0.9, tuning: 1.0, decay: 0.4 },
  MT: { level: 0.9, tuning: 1.0, decay: 0.35 },
  HT: { level: 0.9, tuning: 1.0, decay: 0.3 },
  RS: { level: 0.8 },
  CP: { level: 0.6 },
  CH: { level: 0.7, decay: 0.07 },
  OH: { level: 0.8, decay: 0.2 },
  CY: { level: 0.6, tone: 0.5, decay: 0.8 },
  RD: { level: 0.5, tone: 0.5, decay: 0.4 },
  CB: { level: 0.7 },
};

// Global mutable params (will be updated by UI)
export const voiceParams: VoiceParams = JSON.parse(
  JSON.stringify(defaultVoiceParams)
);
