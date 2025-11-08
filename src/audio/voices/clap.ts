export function playCP(ctx: AudioContext, time: number, _params?: any): void {
  const delays = [0, 0.01, 0.02, 0.03];

  delays.forEach((delay) => {
    const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.05, ctx.sampleRate);
    const data = buffer.getChannelData(0);
    for (let i = 0; i < data.length; i++) {
      data[i] = Math.random() * 2 - 1;
    }

    const src = new AudioBufferSourceNode(ctx, { buffer });
    const bp = new BiquadFilterNode(ctx, {
      type: 'bandpass',
      frequency: 2000,
      Q: 2,
    });
    const gain = new GainNode(ctx, { gain: 0.6 });

    src.connect(bp).connect(gain).connect(ctx.destination);

    const t = time + delay;
    gain.gain.setValueAtTime(0.6, t);
    gain.gain.exponentialRampToValueAtTime(0.0001, t + 0.05);

    src.start(t);
  });
}
