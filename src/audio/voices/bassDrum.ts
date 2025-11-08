export interface BDParams {
  level?: number;
  tone?: number;
  decay?: number;
  tuning?: number;
}

const defaults: Required<BDParams> = {
  level: 1.0,
  tone: 0.5,
  decay: 0.35,
  tuning: 1.0,
};

export function playBD(ctx: AudioContext, time: number, params: BDParams = {}): void {
  const p = { ...defaults, ...params };

  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 * p.tuning });
  const filter = new BiquadFilterNode(ctx, {
    type: 'lowpass',
    frequency: 200 + p.tone * 800,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: p.level });

  osc.connect(filter).connect(gain).connect(ctx.destination);

  osc.frequency.setValueAtTime(150 * p.tuning, time);
  osc.frequency.exponentialRampToValueAtTime(50 * p.tuning, time + 0.08);

  gain.gain.setValueAtTime(p.level, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + p.decay);

  osc.start(time);
  osc.stop(time + p.decay + 0.1);
}
