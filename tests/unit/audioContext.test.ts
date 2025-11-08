import { describe, it, expect } from 'vitest';
import { initAudio, getAudioContext } from '../../src/audio/audioContext';

describe('AudioContext', () => {
  it('should initialize AudioContext', async () => {
    const ctx = await initAudio();
    expect(ctx).toBeInstanceOf(AudioContext);
    expect(ctx.state).toBe('running');
  });

  it('should return same instance on multiple calls', async () => {
    const ctx1 = await initAudio();
    const ctx2 = await initAudio();
    expect(ctx1).toBe(ctx2);
  });

  it('should return context via getter', async () => {
    await initAudio();
    const ctx = getAudioContext();
    expect(ctx).toBeInstanceOf(AudioContext);
  });
});
