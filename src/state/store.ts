import type { AppState, PatternBank, Pattern } from './types';
import { DEFAULT_BANK, createDefaultPattern } from './pattern';
import { loadBank, saveBank } from '../storage/localStorage';

type Listener = (state: AppState) => void;

class Store {
  private state: AppState;
  private listeners: Set<Listener> = new Set();

  constructor() {
    const loaded: PatternBank | null = loadBank();
    const initial: PatternBank = loaded ?? {
      patterns: JSON.parse(JSON.stringify(DEFAULT_BANK)),
      currentPattern: 'A',
    };
    const current = initial.patterns[initial.currentPattern] ?? createDefaultPattern();

    this.state = {
      bank: initial,
      playback: {
        isPlaying: false,
        currentStep: 0,
        bpm: current.bpm,
      },
    };
  }

  getState(): AppState {
    return this.state;
  }

  getCurrentPattern() {
    return this.state.bank.patterns[this.state.bank.currentPattern];
  }

  setCurrentPattern(letter: string): void {
    if (!this.state.bank.patterns[letter]) return;
    this.state = {
      ...this.state,
      bank: { ...this.state.bank, currentPattern: letter },
      playback: { ...this.state.playback, bpm: this.state.bank.patterns[letter].bpm },
    };
    saveBank(this.state.bank);
    this.notify();
  }

  setBank(bank: PatternBank): void {
    this.state = { ...this.state, bank };
    saveBank(bank);
    this.notify();
  }

  toggleStep(instrument: keyof Pattern['steps'], step: number): void {
    const letter = this.state.bank.currentPattern;
    const pat = this.state.bank.patterns[letter];
    if (!pat) return;
    const steps = pat.steps[instrument].slice();
    steps[step] = !steps[step];
    const updated: Pattern = { ...pat, steps: { ...pat.steps, [instrument]: steps } };
    const bank = {
      ...this.state.bank,
      patterns: { ...this.state.bank.patterns, [letter]: updated },
    };
    this.state = { ...this.state, bank: bank as PatternBank };
    saveBank(bank as PatternBank);
    this.notify();
  }

  setState(partial: Partial<AppState>): void {
    this.state = { ...this.state, ...partial };
    this.notify();
  }

  subscribe(listener: Listener): () => void {
    this.listeners.add(listener);
    return () => this.listeners.delete(listener);
  }

  private notify(): void {
    this.listeners.forEach((listener) => listener(this.state));
  }
}

export const store = new Store();
