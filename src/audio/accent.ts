// Accent bus logic for TR-808
// Per-voice accent sensitivity (k values)
export const ACCENT_SENSITIVITY = {
  BD: 0.6,
  SD: 0.5,
  LT: 0.5,
  MT: 0.5,
  HT: 0.5,
  RS: 0.5,
  CP: 0.5,
  CH: 0.4,
  OH: 0.4,
  CY: 0.4,
  RD: 0.4,
  CB: 0.5,
};

export type VoiceName = keyof typeof ACCENT_SENSITIVITY;

// Calculate accent gain for a voice
// AccentGain = 1.0 + k * Accent
export function getAccentGain(voice: VoiceName, accentValue: number): number {
  return 1.0 + ACCENT_SENSITIVITY[voice] * accentValue;
}
