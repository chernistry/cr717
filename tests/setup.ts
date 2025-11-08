// Mock AudioContext for tests
class MockAudioContext {
  currentTime = 0;
  state = 'running';
  destination = {};

  async resume(): Promise<void> {
    this.state = 'running';
  }

  async close(): Promise<void> {
    this.state = 'closed';
  }
}

// Mock OfflineAudioContext for rendering tests
class MockOfflineAudioContext extends MockAudioContext {
  private length: number;
  private sampleRate: number;

  constructor(_channels: number, length: number, sampleRate: number) {
    super();
    this.length = length;
    this.sampleRate = sampleRate;
  }

  async startRendering(): Promise<AudioBuffer> {
    const buffer = {
      duration: this.length / this.sampleRate,
      length: this.length,
      numberOfChannels: 1,
      sampleRate: this.sampleRate,
      getChannelData: () => {
        const data = new Float32Array(this.length);
        for (let i = 0; i < this.length; i++) {
          data[i] = Math.sin((i / this.sampleRate) * 2 * Math.PI * 100) * 0.5;
        }
        return data;
      },
    };
    return buffer as AudioBuffer;
  }
}

// @ts-expect-error - Mocking global
global.AudioContext = MockAudioContext;
// @ts-expect-error - Mocking global
global.OfflineAudioContext = MockOfflineAudioContext;
// @ts-expect-error - Mocking global
global.OscillatorNode = class {
  frequency = { setValueAtTime: () => {}, exponentialRampToValueAtTime: () => {} };
  connect() {
    return this;
  }
  start() {}
  stop() {}
};
// @ts-expect-error - Mocking global
global.GainNode = class {
  gain = { setValueAtTime: () => {}, exponentialRampToValueAtTime: () => {} };
  connect() {
    return this;
  }
};
