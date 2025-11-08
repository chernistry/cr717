export function playMT(ctx: AudioContext, time: number, _params?: any): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 90 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  osc.frequency.setValueAtTime(90, time);
  osc.frequency.exponentialRampToValueAtTime(60, time + 0.08);

  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.35);

  osc.start(time);
  osc.stop(time + 0.4);
}
