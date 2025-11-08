export function playCH(ctx: AudioContext, time: number, _params?: any): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.1, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const hp = new BiquadFilterNode(ctx, {
    type: 'highpass',
    frequency: 8000,
    Q: 1,
  });
  const gain = new GainNode(ctx, { gain: 0.7 });

  src.connect(hp).connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.7, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.07);

  src.start(time);
}
