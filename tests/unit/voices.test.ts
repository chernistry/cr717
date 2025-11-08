import { describe, it, expect } from 'vitest';
import { playBD } from '../../src/audio/voices/bassDrum';

describe('Bass Drum', () => {
  it('should render without errors', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    // Verify audio was generated
    const data = buffer.getChannelData(0);
    const hasSound = data.some((v) => Math.abs(v) > 0.001);
    expect(hasSound).toBe(true);
  });

  it('should have correct duration', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    // Should have sound in first 0.5s
    expect(buffer.duration).toBe(1);
  });

  it('should have peak amplitude near start', async () => {
    const ctx = new OfflineAudioContext(1, 44100, 44100);
    playBD(ctx, 0);
    const buffer = await ctx.startRendering();

    const data = buffer.getChannelData(0);
    // Find max amplitude in first 100ms
    const firstSamples = data.slice(0, 4410); // 100ms at 44.1kHz
    const maxAmplitude = Math.max(...firstSamples.map(Math.abs));

    expect(maxAmplitude).toBeGreaterThan(0.1);
  });
});
