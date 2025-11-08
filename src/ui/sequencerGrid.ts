import { createElement } from './dom';

export interface SequencerGridCallbacks {
  onStepToggle: (instrument: string, step: number) => void;
}

export class SequencerGrid {
  private container: HTMLElement;
  private instruments = ['BD', 'SD', 'CH', 'OH', 'CP'];

  constructor(containerId: string, callbacks: SequencerGridCallbacks) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.render(callbacks);
  }

  private render(callbacks: SequencerGridCallbacks): void {
    this.container.innerHTML = '';

    this.instruments.forEach((instrument) => {
      const row = createElement('div', 'sequencer-row');
      const label = createElement('div', 'instrument-label');
      label.textContent = instrument;
      row.appendChild(label);

      for (let step = 0; step < 16; step++) {
        const btn = createElement('button', 'step-button', {
          'data-instrument': instrument,
          'data-step': step.toString(),
        });
        row.appendChild(btn);
      }

      this.container.appendChild(row);
    });

    // Event delegation
    this.container.addEventListener('click', (e) => {
      const target = e.target as HTMLElement;
      if (target.classList.contains('step-button')) {
        const instrument = target.dataset.instrument!;
        const step = parseInt(target.dataset.step!, 10);
        target.classList.toggle('active');
        callbacks.onStepToggle(instrument, step);
      }
    });
  }

  highlightStep(step: number): void {
    const buttons = this.container.querySelectorAll('.step-button');
    buttons.forEach((btn) => {
      const btnStep = parseInt((btn as HTMLElement).dataset.step!, 10);
      btn.classList.toggle('current', btnStep === step);
    });
  }

  setStepActive(instrument: string, step: number, active: boolean): void {
    const btn = this.container.querySelector(
      `[data-instrument="${instrument}"][data-step="${step}"]`
    );
    if (btn) {
      btn.classList.toggle('active', active);
    }
  }
}
