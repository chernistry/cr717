let audioContext: AudioContext | null = null;

export async function initAudio(): Promise<AudioContext> {
  if (!audioContext) {
    audioContext = new AudioContext({ latencyHint: 'interactive' });
    await audioContext.resume();
  }
  return audioContext;
}

export function getAudioContext(): AudioContext | null {
  return audioContext;
}
