export interface BDParams {
  level?: number;
  tone?: number;
  decay?: number;
  tuning?: number;
  accent?: number;
}

const defaults: Required<BDParams> = {
  level: 1.0,
  tone: 0.5,
  decay: 0.5,
  tuning: 1.0,
  accent: 0,
};

export function playBD(ctx: AudioContext, time: number, params: BDParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.6;

  // Bridged-T resonator at 56 Hz
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 56 * p.tuning });
  const filter = new BiquadFilterNode(ctx, {
    type: 'lowpass',
    frequency: 1500 + p.tone * 500,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: p.level * accentGain });

  // Click injection (HP-filtered pulse)
  const clickBuffer = ctx.createBuffer(1, 100, ctx.sampleRate);
  const clickData = clickBuffer.getChannelData(0);
  for (let i = 0; i < 100; i++) {
    clickData[i] = (Math.random() * 2 - 1) * Math.exp(-i / 10);
  }
  const click = new AudioBufferSourceNode(ctx, { buffer: clickBuffer });
  const clickFilter = new BiquadFilterNode(ctx, { type: 'highpass', frequency: 2000 });
  const clickGain = new GainNode(ctx, { gain: 0.3 * accentGain });

  osc.connect(filter).connect(gain).connect(ctx.destination);
  click.connect(clickFilter).connect(clickGain).connect(ctx.destination);

  // Pitch envelope: brief overshoot then settle to 56 Hz
  osc.frequency.setValueAtTime(56 * p.tuning, time);
  osc.frequency.exponentialRampToValueAtTime(50 * p.tuning, time + 0.01);

  // Amplitude envelope
  const decayTime = 0.1 + p.decay * 0.9; // 100-1000ms range
  gain.gain.setValueAtTime(p.level * accentGain, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + decayTime);

  osc.start(time);
  click.start(time);
  osc.stop(time + decayTime + 0.1);
}
