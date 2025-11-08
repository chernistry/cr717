// Mock AudioContext for tests
class MockAudioContext {
  currentTime = 0;
  state = 'running';
  destination = {};
  sampleRate = 44100;

  async resume(): Promise<void> {
    this.state = 'running';
  }

  async close(): Promise<void> {
    this.state = 'closed';
  }

  createBuffer(channels: number, length: number, sampleRate: number): AudioBuffer {
    return {
      duration: length / sampleRate,
      length,
      numberOfChannels: channels,
      sampleRate,
      getChannelData: () => new Float32Array(length),
    } as AudioBuffer;
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

// Mock localStorage
const storage = new Map<string, string>();
global.localStorage = {
  getItem: (key: string) => storage.get(key) || null,
  setItem: (key: string, value: string) => storage.set(key, value),
  removeItem: (key: string) => storage.delete(key),
  clear: () => storage.clear(),
  length: 0,
  key: () => null,
};

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
// @ts-expect-error - Mocking global
global.AudioBufferSourceNode = class {
  connect() {
    return this;
  }
  start() {}
  stop() {}
};
// @ts-expect-error - Mocking global
global.BiquadFilterNode = class {
  connect() {
    return this;
  }
};
// @ts-expect-error - Mocking global
global.DOMException = class extends Error {
  name: string;
  constructor(message: string, name: string) {
    super(message);
    this.name = name;
  }
};
