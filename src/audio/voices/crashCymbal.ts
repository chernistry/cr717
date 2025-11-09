export interface CYParams {
  level?: number;
  accent?: number;
}

const defaults: Required<CYParams> = {
  level: 0.7,
  accent: 0,
};

// Six square-wave oscillators at TR-808 spec frequencies
const OSC_FREQS = [205.3, 304.4, 369.6, 522.7, 540, 800];

export function playCY(ctx: AudioContext, time: number, params: CYParams = {}): void {
  const p = { ...defaults, ...params };
  const accentGain = 1 + p.accent * 0.4;

  // Create 6 square oscillators
  const oscs = OSC_FREQS.map((freq) => new OscillatorNode(ctx, { type: 'square', frequency: freq }));
  const oscGain = new GainNode(ctx, { gain: 0.15 });

  // Dual 3rd-order BPF at 3.44 kHz and 7.10 kHz
  const bp1 = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 3440, Q: 2 });
  const bp2 = new BiquadFilterNode(ctx, { type: 'bandpass', frequency: 7100, Q: 2 });
  // Additional HP for brightness
  const hp = new BiquadFilterNode(ctx, { type: 'highpass', frequency: 5000, Q: 1 });
  const gain = new GainNode(ctx, { gain: p.level * accentGain });

  oscs.forEach((osc) => osc.connect(oscGain));
  oscGain.connect(bp1).connect(bp2).connect(hp).connect(gain).connect(ctx.destination);

  // Expo decay: 1.2s
  gain.gain.setValueAtTime(p.level * accentGain, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 1.2);

  oscs.forEach((osc) => {
    osc.start(time);
    osc.stop(time + 1.3);
  });
}
