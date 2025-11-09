export interface CBParams {
  level?: number;
  decay?: number;
  accent?: number;
}

const defaults: Required<CBParams> = {
  level: 1.0,
  decay: 0.35,
  accent: 0,
};

export function playCB(ctx: AudioContext, time: number, params: CBParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.5;

  // Two square oscillators at 540 Hz and 800 Hz (factory-trimmed)
  const osc1 = new OscillatorNode(ctx, { type: 'square', frequency: 540 });
  const osc2 = new OscillatorNode(ctx, { type: 'square', frequency: 800 });

  // Band-pass emphasis around 1-2 kHz
  const bp = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 1500, Q: 2 });
  const gain = new GainNode(ctx, { gain: p.level * accentGain });

  osc1.connect(bp);
  osc2.connect(bp);
  bp.connect(gain).connect(ctx.destination);

  // Expo decay ~350ms
  gain.gain.setValueAtTime(p.level * accentGain, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + p.decay);

  osc1.start(time);
  osc2.start(time);
  osc1.stop(time + p.decay + 0.1);
  osc2.stop(time + p.decay + 0.1);
}
