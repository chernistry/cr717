export function playRD(ctx: AudioContext, time: number, _params?: any): void {
  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.5, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }

  const src = new AudioBufferSourceNode(ctx, { buffer });
  const bp = new BiquadFilterNode(ctx, {
    type: 'bandpass',
    frequency: 8000,
    Q: 2,
  });
  const gain = new GainNode(ctx, { gain: 0.5 });

  src.connect(bp).connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.5, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.4);

  src.start(time);
}
