export function playCB(ctx: AudioContext, time: number, _params?: any): void {
  const osc1 = new OscillatorNode(ctx, { type: 'square', frequency: 540 });
  const osc2 = new OscillatorNode(ctx, { type: 'square', frequency: 800 });

  const bp = new BiquadFilterNode(ctx, {
    type: 'bandpass',
    frequency: 800,
    Q: 5,
  });

  const gain = new GainNode(ctx, { gain: 0.7 });

  osc1.connect(bp);
  osc2.connect(bp);
  bp.connect(gain).connect(ctx.destination);

  gain.gain.setValueAtTime(0.7, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.2);

  osc1.start(time);
  osc2.start(time);
  osc1.stop(time + 0.25);
  osc2.stop(time + 0.25);
}
