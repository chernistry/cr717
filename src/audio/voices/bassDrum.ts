export function playBD(ctx: AudioContext, time: number): void {
  const osc = new OscillatorNode(ctx, { type: 'sine', frequency: 150 });
  const gain = new GainNode(ctx, { gain: 1.0 });

  osc.connect(gain).connect(ctx.destination);

  // Pitch envelope: 150Hz → 50Hz over 80ms
  osc.frequency.setValueAtTime(150, time);
  osc.frequency.exponentialRampToValueAtTime(50, time + 0.08);

  // Amplitude envelope: 1.0 → 0.0001 over 350ms
  gain.gain.setValueAtTime(1.0, time);
  gain.gain.exponentialRampToValueAtTime(0.0001, time + 0.35);

  osc.start(time);
  osc.stop(time + 0.5);
}
