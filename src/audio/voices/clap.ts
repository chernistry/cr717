export interface CPParams {
  level?: number;
  tone?: number;
  decay?: number;
  accent?: number;
}

const defaults: Required<CPParams> = {
  level: 1.0,
  tone: 0.5,
  decay: 0.15,
  accent: 0,
};

export function playCP(ctx: AudioContext, time: number, params: CPParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.5;

  // Four noise bursts at 0, 12, 24, 36 ms
  const pulseTimes = [0, 0.012, 0.024, 0.036];

  pulseTimes.forEach((offset) => {
    const buffer = ctx.createBuffer(1, 500, ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < 500; i++) {
      data[i] = (Math.random() * 2 - 1) * Math.exp(-i / 50);
    }

    const src = new AudioBufferSourceNode(ctx, { buffer });
    const bp = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 1500 + p.tone * 1000, Q: 2 });
    const gain = new GainNode(ctx, { gain: 0.25 * p.level * accentGain });

    src.connect(bp).connect(gain).connect(ctx.destination);

    // Envelope for each burst
    gain.gain.setValueAtTime(0.25 * p.level * accentGain, time + offset);
    gain.gain.exponentialRampToValueAtTime(0.0001, time + offset + p.decay);

    src.start(time + offset);
  });
}
