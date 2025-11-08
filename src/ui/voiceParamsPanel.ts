import { voiceParams } from '../state/voiceParams';

export class VoiceParamsPanel {
  private container: HTMLElement;
  private currentVoice: string = 'BD';

  constructor(containerId: string) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.render();
  }

  selectVoice(voice: string): void {
    this.currentVoice = voice;
    this.render();
  }

  private render(): void {
    const params = voiceParams[this.currentVoice as keyof typeof voiceParams];
    if (!params) return;

    this.container.innerHTML = `
      <div class="voice-params-panel">
        <h3 class="panel-title">${this.currentVoice} Parameters</h3>
        <div class="params-grid">
          ${Object.entries(params)
            .map(
              ([key, value]) => `
            <div class="param-control">
              <label>${key}</label>
              <input type="range" 
                     min="${this.getMin(key)}" 
                     max="${this.getMax(key)}" 
                     step="0.01"
                     value="${value}" 
                     data-voice="${this.currentVoice}"
                     data-param="${key}">
              <span class="param-value">${value.toFixed(2)}</span>
            </div>
          `
            )
            .join('')}
        </div>
      </div>
    `;

    this.attachEvents();
  }

  private getMin(param: string): number {
    if (param === 'tuning') return 0.5;
    return 0;
  }

  private getMax(param: string): number {
    if (param === 'tuning') return 2.0;
    if (param === 'decay') return 1.0;
    return 1.0;
  }

  private attachEvents(): void {
    this.container.querySelectorAll('input[type="range"]').forEach((input) => {
      input.addEventListener('input', (e) => {
        const target = e.target as HTMLInputElement;
        const voice = target.dataset.voice!;
        const param = target.dataset.param!;
        const value = parseFloat(target.value);

        // Update global params
        (voiceParams as any)[voice][param] = value;

        // Update display
        const display = target.nextElementSibling as HTMLElement;
        if (display) {
          display.textContent = value.toFixed(2);
        }
      });
    });
  }
}
