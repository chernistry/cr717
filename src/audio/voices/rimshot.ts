export interface RSParams {
  level?: number;
  accent?: number;
}

const defaults: Required<RSParams> = {
  level: 1.0,
  accent: 0,
};

export function playRS(ctx: AudioContext, time: number, params: RSParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.5;

  // Short pulse/impulse into band-pass network
  const buffer = ctx.createBuffer(1, 200, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < 200; i++) {
    data[i] = (Math.random() * 2 - 1) * Math.exp(-i / 20);
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const bp = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 2500, Q: 5 });
  const gain = new GainNode(ctx, { gain: p.level * accentGain });

  src.connect(bp).connect(gain).connect(ctx.destination);

  // Expo decay: 30ms
  gain.gain.setValueAtTime(p.level * accentGain, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.03);

  src.start(time);
}
