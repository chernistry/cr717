export interface SDParams {
  level?: number;
  tone?: number;
  snappy?: number;
  tuning?: number;
  accent?: number;
}

const defaults: Required<SDParams> = {
  level: 1.0,
  tone: 0.5,
  snappy: 0.5,
  tuning: 1.0,
  accent: 0,
};

export function playSD(ctx: AudioContext, time: number, params: SDParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.5;

  // Dual resonators at 180 Hz and 330 Hz
  const osc1 = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 * p.tuning });
  const osc2 = new OscillatorNode(ctx, { type: 'triangle', frequency: 330 * p.tuning });
  const osc1Gain = new GainNode(ctx, { gain: 0.5 * p.tone });
  const osc2Gain = new GainNode(ctx, { gain: 0.3 * p.tone });

  // Noise component (HP/BP 700-3kHz)
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.25, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const noise = new AudioBufferSourceNode(ctx, { buffer });
  const noiseHP = new BiquadFilterNode(ctx, { type: 'highpass', frequency: 700, Q: 1 });
  const noiseBP = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 1500, Q: 1 });
  const noiseGain = new GainNode(ctx, { gain: 0.6 * p.snappy });

  osc1.connect(osc1Gain);
  osc2.connect(osc2Gain);
  noise.connect(noiseHP).connect(noiseBP).connect(noiseGain);

  const master = new GainNode(ctx, { gain: p.level * accentGain });
  osc1Gain.connect(master);
  osc2Gain.connect(master);
  noiseGain.connect(master);
  master.connect(ctx.destination);

  // Body decay: 250ms, Noise decay: 200ms
  master.gain.setValueAtTime(p.level * accentGain, time);
  master.gain.exponentialRampToValueAtTime(0.0001, time + 0.25);

  osc1.start(time);
  osc2.start(time);
  noise.start(time);
  osc1.stop(time + 0.3);
  osc2.stop(time + 0.3);
}
