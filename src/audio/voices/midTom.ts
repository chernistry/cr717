export interface MTParams {
  level?: number;
  tuning?: number;
  decay?: number;
  accent?: number;
}

const defaults: Required<MTParams> = {
  level: 1.0,
  tuning: 1.0,
  decay: 0.28,
  accent: 0,
};

export function playMT(ctx: AudioContext, time: number, params: MTParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.5;

  // Resonant band-pass at 200 Hz
  const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 200 * p.tuning });
  const filter = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 200 * p.tuning, Q: 10 });
  const gain = new GainNode(ctx, { gain: p.level * accentGain });

  osc.connect(filter).connect(gain).connect(ctx.destination);

  // Small downward pitch bend
  osc.frequency.setValueAtTime(200 * p.tuning, time);
  osc.frequency.exponentialRampToValueAtTime(185 * p.tuning, time + 0.015);

  // Expo decay ~280ms
  gain.gain.setValueAtTime(p.level * accentGain, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + p.decay);

  osc.start(time);
  osc.stop(time + p.decay + 0.1);
}
