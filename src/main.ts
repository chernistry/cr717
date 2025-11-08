import { getAudioContext } from './audio/audioContext';
import { AudioScheduler } from './audio/scheduler';
import { playBD } from './audio/voices/bassDrum';
import { playSD } from './audio/voices/snareDrum';
import { playLT } from './audio/voices/lowTom';
import { playMT } from './audio/voices/midTom';
import { playHT } from './audio/voices/highTom';
import { playRS } from './audio/voices/rimshot';
import { playCP } from './audio/voices/clap';
import { playCH } from './audio/voices/closedHiHat';
import { playOH } from './audio/voices/openHiHat';
import { playCY } from './audio/voices/crashCymbal';
import { playRD } from './audio/voices/rideCymbal';
import { playCB } from './audio/voices/cowbell';
import { Controls } from './ui/controls';
import { SequencerGrid } from './ui/sequencerGrid';
import { Visualizer } from './ui/visualizer';
import { VoiceParamsPanel } from './ui/voiceParamsPanel';
import { store } from './state/store';
import { toggleStep, createDefaultPattern } from './state/pattern';
import { loadPattern, savePattern } from './storage/localStorage';
import { voiceParams } from './state/voiceParams';

const scheduler = new AudioScheduler();
const voiceMap = {
  BD: playBD,
  SD: playSD,
  LT: playLT,
  MT: playMT,
  HT: playHT,
  RS: playRS,
  CP: playCP,
  CH: playCH,
  OH: playOH,
  CY: playCY,
  RD: playRD,
  CB: playCB,
};

const savedPattern = loadPattern();
if (savedPattern) {
  store.setState({ pattern: savedPattern });
}

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

// Voice params panel
const paramsPanel = new VoiceParamsPanel('voice-params');

// Add click handlers to instrument labels
document.addEventListener('click', (e) => {
  const target = e.target as HTMLElement;
  if (target.classList.contains('instrument-label')) {
    const voice = target.textContent?.trim();
    if (voice) {
      paramsPanel.selectVoice(voice);
    }
  }
});

function loadPatternToUI(): void {
  const state = store.getState();

  Object.keys(state.pattern.steps).forEach((instrument) => {
    for (let step = 0; step < 16; step++) {
      grid.setStepActive(instrument, step, false);
    }
  });

  Object.entries(state.pattern.steps).forEach(([instrument, steps]) => {
    steps.forEach((active, step) => {
      if (active) {
        grid.setStepActive(instrument, step, true);
      }
    });
  });

  const bpmSlider = document.getElementById('bpm-slider') as HTMLInputElement;
  const bpmDisplay = document.getElementById('bpm-display') as HTMLElement;
  if (bpmSlider && bpmDisplay) {
    bpmSlider.value = state.pattern.bpm.toString();
    bpmDisplay.textContent = state.pattern.bpm.toString();
  }

  updateStatus(`Loaded: ${state.pattern.name}`);
}

function loadDefaultPattern(): void {
  const defaultPattern = createDefaultPattern();
  store.setState({ pattern: defaultPattern });
  savePattern(defaultPattern);
  loadPatternToUI();
}

loadPatternToUI();

const visualizer = new Visualizer((step) => {
  grid.highlightStep(step);
});

const controls = new Controls('controls', {
  onPlay: () => {
    const ctx = getAudioContext();
    if (!ctx) return;

    const bpm = controls.getBpm();

    scheduler.start(ctx, bpm, (step, time) => {
      const currentState = store.getState();
      Object.entries(currentState.pattern.steps).forEach(([instrument, steps]) => {
        if (steps[step]) {
          const voice = voiceMap[instrument as keyof typeof voiceMap];
          const params = voiceParams[instrument as keyof typeof voiceParams];
          voice(ctx, time, params);
        }
      });
    });

    visualizer.start(bpm);
    store.setState({ playback: { isPlaying: true, currentStep: 0, bpm } });
    updateStatus('Playing');
  },

  onStop: () => {
    scheduler.stop();
    visualizer.stop();
    grid.highlightStep(-1);
    store.setState({
      playback: { isPlaying: false, currentStep: 0, bpm: controls.getBpm() },
    });
    updateStatus('Stopped');
  },

  onBpmChange: (bpm) => {
    const state = store.getState();
    if (state.playback.isPlaying) {
      scheduler.stop();
      visualizer.stop();
      const ctx = getAudioContext();
      if (ctx) {
        scheduler.start(ctx, bpm, (step, time) => {
          const currentState = store.getState();
          Object.entries(currentState.pattern.steps).forEach(([instrument, steps]) => {
            if (steps[step]) {
              const voice = voiceMap[instrument as keyof typeof voiceMap];
              const params = voiceParams[instrument as keyof typeof voiceParams];
              voice(ctx, time, params);
            }
          });
        });
        visualizer.start(bpm);
      }
    }
    store.setState({ playback: { ...state.playback, bpm } });
  },
});

// Swing control
const swingSlider = document.getElementById('swing-slider') as HTMLInputElement;
const swingDisplay = document.getElementById('swing-display') as HTMLElement;

// Initialize swing from slider default
{
  const initSwing = parseInt(swingSlider.value, 10) / 100;
  swingDisplay.textContent = `${swingSlider.value}%`;
  scheduler.setSwing(initSwing);
}

swingSlider.addEventListener('input', () => {
  const swing = parseInt(swingSlider.value, 10) / 100;
  swingDisplay.textContent = `${swingSlider.value}%`;
  scheduler.setSwing(swing);
});

document.addEventListener('keydown', (e) => {
  if (e.target instanceof HTMLInputElement) return;

  if (e.key === ' ') {
    e.preventDefault();
    const state = store.getState();
    if (state.playback.isPlaying) {
      controls.setPlaying(false);
      scheduler.stop();
      visualizer.stop();
      updateStatus('Stopped');
    } else {
      document.getElementById('play-btn')?.click();
    }
    return;
  }

  if (e.key === '+' || e.key === '=') {
    e.preventDefault();
    changeBPM(5);
    return;
  }

  if (e.key === '-' || e.key === '_') {
    e.preventDefault();
    changeBPM(-5);
    return;
  }

  if (e.key === '?') {
    e.preventDefault();
    toggleHelp();
    return;
  }

  if (e.key === 'Escape') {
    closeHelp();
    return;
  }

  if (e.key === 't' || e.key === 'T') {
    e.preventDefault();
    toggleTheme();
    return;
  }

  if (e.key === 'd' || e.key === 'D') {
    e.preventDefault();
    loadDefaultPattern();
  }
});

function changeBPM(delta: number): void {
  const slider = document.getElementById('bpm-slider') as HTMLInputElement;
  const current = parseInt(slider.value, 10);
  const newValue = Math.max(60, Math.min(240, current + delta));
  slider.value = newValue.toString();
  const event = new window.Event('input');
  slider.dispatchEvent(event);
}

function toggleHelp(): void {
  const overlay = document.getElementById('help-overlay');
  overlay?.classList.toggle('visible');
}

function closeHelp(): void {
  const overlay = document.getElementById('help-overlay');
  overlay?.classList.remove('visible');
}

function toggleTheme(): void {
  const html = document.documentElement;
  const current = html.getAttribute('data-theme');
  const newTheme = current === 'dark' ? 'light' : 'dark';
  html.setAttribute('data-theme', newTheme);
  const btn = document.getElementById('theme-toggle');
  if (btn) btn.textContent = newTheme === 'dark' ? '🌙' : '☀️';
}

function updateStatus(text: string): void {
  const status = document.getElementById('status-text');
  if (status) status.textContent = text;
}

let lastTime = window.performance.now();
let frames = 0;

function updateFPS(): void {
  frames++;
  const now = window.performance.now();
  if (now - lastTime >= 1000) {
    const fps = Math.round((frames * 1000) / (now - lastTime));
    const perfText = document.getElementById('perf-text');
    if (perfText) perfText.textContent = `FPS: ${fps}`;
    frames = 0;
    lastTime = now;
  }
  requestAnimationFrame(updateFPS);
}

const initialState = store.getState();
updateStatus(`Ready - ${initialState.pattern.name}`);
requestAnimationFrame(updateFPS);

document.getElementById('help-btn')?.addEventListener('click', toggleHelp);
document.getElementById('theme-toggle')?.addEventListener('click', toggleTheme);
document.getElementById('default-pattern-btn')?.addEventListener('click', loadDefaultPattern);

document.getElementById('help-overlay')?.addEventListener('click', (e) => {
  if (e.target === e.currentTarget) closeHelp();
});

console.log(`TR-808 initialized - ${initialState.pattern.name} @ ${initialState.pattern.bpm} BPM`);
console.log('12 voices with parameters: BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB');
console.log('Click voice label to edit parameters');
console.log('Press ? for shortcuts, D for default pattern');
