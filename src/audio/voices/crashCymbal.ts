export function playCY(ctx: AudioContext, time: number, _params?: any): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 1.0, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const hp = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 5000,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: 0.6 });

  src.connect(hp).connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.6, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.8);

  src.start(time);
}
