export interface SDParams {
  level?: number;
  tone?: number;
  snappy?: number;
  tuning?: number;
}

const defaults: Required<SDParams> = {
  level: 1.0,
  tone: 0.5,
  snappy: 0.5,
  tuning: 1.0,
};

export function playSD(ctx: AudioContext, time: number, params: SDParams = {}): void {
  const p = { ...defaults, ...params };

  const osc = new OscillatorNode(ctx, { type: 'triangle', frequency: 180 * p.tuning });
  const oscGain = new GainNode(ctx, { gain: 0.7 * p.tone });

  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.2, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const noise = new AudioBufferSourceNode(ctx, { buffer });
  const noiseFilter = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 1000,
    Q: 1,
  });
  const noiseGain = new GainNode(ctx, { gain: 0.5 * p.snappy });

  osc.connect(oscGain);
  noise.connect(noiseFilter).connect(noiseGain);

  const master = new GainNode(ctx, { gain: p.level });
  oscGain.connect(master);
  noiseGain.connect(master);
  master.connect(ctx.destination);

  master.gain.setValueAtTime(p.level, time);
  master.gain.exponentialRampToValueAtTime(0.0001, time + 0.15);

  osc.start(time);
  noise.start(time);
  osc.stop(time + 0.2);
}
