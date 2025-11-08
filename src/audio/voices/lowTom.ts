export function playLT(ctx: AudioContext, time: number, _params?: any): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 65 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  // Pitch envelope: 65Hz → 45Hz
  osc.frequency.setValueAtTime(65, time);
  osc.frequency.exponentialRampToValueAtTime(45, time + 0.1);

  // Amp envelope
  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.4);

  osc.start(time);
  osc.stop(time + 0.5);
}
