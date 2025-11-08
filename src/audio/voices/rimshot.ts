export function playRS(ctx: AudioContext, time: number, _params?: any): void {
  const osc1 = new OscillatorNode(ctx, { type: 'square', frequency: 180 });
  const osc2 = new OscillatorNode(ctx, { type: 'square', frequency: 330 });

  const buffer = ctx.createBuffer(1, ctx.sampleRate * 0.05, ctx.sampleRate);
  const data = buffer.getChannelData(0);
  for (let i = 0; i < data.length; i++) {
    data[i] = Math.random() * 2 - 1;
  }
  const noise = new AudioBufferSourceNode(ctx, { buffer });

  const gain = new GainNode(ctx, { gain: 0.8 });

  osc1.connect(gain);
  osc2.connect(gain);
  noise.connect(gain);
  gain.connect(ctx.destination);

  gain.gain.setValueAtTime(0.8, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.05);

  osc1.start(time);
  osc2.start(time);
  noise.start(time);
  osc1.stop(time + 0.06);
  osc2.stop(time + 0.06);
}
