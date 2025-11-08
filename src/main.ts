import { getAudioContext } from './audio/audioContext';
import { AudioScheduler } from './audio/scheduler';
import { playBD } from './audio/voices/bassDrum';
import { playSD } from './audio/voices/snareDrum';
import { playCH } from './audio/voices/closedHiHat';
import { playOH } from './audio/voices/openHiHat';
import { playCP } from './audio/voices/clap';
import { Controls } from './ui/controls';
import { SequencerGrid } from './ui/sequencerGrid';
import { Visualizer } from './ui/visualizer';
import { store } from './state/store';
import { toggleStep } from './state/pattern';
import { loadPattern, savePattern } from './storage/localStorage';

const scheduler = new AudioScheduler();
const voiceMap = { BD: playBD, SD: playSD, CH: playCH, OH: playOH, CP: playCP };

// Load saved pattern
const savedPattern = loadPattern();
if (savedPattern) {
  store.setState({ pattern: savedPattern });
}

// Initialize UI
const grid = new SequencerGrid('sequencer', {
  onStepToggle: (instrument, step) => {
    const state = store.getState();
    const updated = toggleStep(
      state.pattern,
      instrument as keyof typeof voiceMap,
      step
    );
    store.setState({ pattern: updated });
    savePattern(updated);
  },
});

// Restore pattern state in UI
const initialState = store.getState();
Object.entries(initialState.pattern.steps).forEach(([instrument, steps]) => {
  steps.forEach((active, step) => {
    if (active) {
      grid.setStepActive(instrument, step, true);
    }
  });
});

const visualizer = new Visualizer((step) => {
  grid.highlightStep(step);
});

const controls = new Controls('controls', {
  onPlay: () => {
    const ctx = getAudioContext();
    if (!ctx) return;

    const state = store.getState();
    const bpm = controls.getBpm();

    scheduler.start(ctx, bpm, (step, time) => {
      Object.entries(state.pattern.steps).forEach(([instrument, steps]) => {
        if (steps[step]) {
          const voice = voiceMap[instrument as keyof typeof voiceMap];
          voice(ctx, time);
        }
      });
    });

    visualizer.start(bpm);
    store.setState({ playback: { isPlaying: true, currentStep: 0, bpm } });
  },

  onStop: () => {
    scheduler.stop();
    visualizer.stop();
    grid.highlightStep(-1);
    store.setState({
      playback: { isPlaying: false, currentStep: 0, bpm: controls.getBpm() },
    });
  },

  onBpmChange: (bpm) => {
    const state = store.getState();
    if (state.playback.isPlaying) {
      scheduler.stop();
      visualizer.stop();
      const ctx = getAudioContext();
      if (ctx) {
        scheduler.start(ctx, bpm, (step, time) => {
          Object.entries(state.pattern.steps).forEach(([instrument, steps]) => {
            if (steps[step]) {
              const voice = voiceMap[instrument as keyof typeof voiceMap];
              voice(ctx, time);
            }
          });
        });
        visualizer.start(bpm);
      }
    }
    store.setState({ playback: { ...state.playback, bpm } });
  },
});

console.log('TR-808 initialized');
