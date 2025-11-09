// Hat choke logic for TR-808
// OH mutes CH when triggered

let chActiveGain: GainNode | null = null;

// Register CH gain node for choke control
export function registerCHGain(gain: GainNode): void {
  chActiveGain = gain;
}

// Choke CH when OH triggers (fast release 2-5ms)
export function chokeCH(_ctx: AudioContext, time: number): void {
  if (chActiveGain) {
    chActiveGain.gain.cancelScheduledValues(time);
    chActiveGain.gain.setValueAtTime(chActiveGain.gain.value, time);
    chActiveGain.gain.exponentialRampToValueAtTime(0.0001, time + 0.003);
  }
}

// Clear CH gain reference
export function clearCHGain(): void {
  chActiveGain = null;
}
