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

// @ts-expect-error - Mocking global
global.AudioContext = MockAudioContext;
