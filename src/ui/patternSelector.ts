export class PatternSelector {
  private container: HTMLElement;
  private currentPattern: string = 'A';
  private onSelect: (pattern: string) => void;

  constructor(containerId: string, onSelect: (pattern: string) => void) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.onSelect = onSelect;
    this.render();
    this.attachKeyboard();
  }

  setCurrentPattern(pattern: string): void {
    this.currentPattern = pattern;
    this.updateUI();
  }

  private render(): void {
    this.container.innerHTML = `
      <div class="pattern-selector">
        <div class="pattern-buttons">
          ${['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H']
            .map(
              (p) => `
            <button class="pattern-btn ${p === this.currentPattern ? 'active' : ''}" 
                    data-pattern="${p}">${p}</button>
          `
            )
            .join('')}
        </div>
        <div class="pattern-actions">
          <button class="action-btn" id="copy-pattern" title="Copy (Ctrl+C)">
            <i class="ph ph-copy"></i>
          </button>
          <button class="action-btn" id="paste-pattern" title="Paste (Ctrl+V)">
            <i class="ph ph-clipboard"></i>
          </button>
          <button class="action-btn" id="clear-pattern" title="Clear">
            <i class="ph ph-trash"></i>
          </button>
        </div>
      </div>
    `;

    this.attachEvents();
  }

  private attachEvents(): void {
    this.container.querySelectorAll('.pattern-btn').forEach((btn) => {
      btn.addEventListener('click', () => {
        const pattern = (btn as HTMLElement).dataset.pattern;
        if (pattern) this.selectPattern(pattern);
      });
    });

    document.getElementById('copy-pattern')?.addEventListener('click', () => {
      this.container.dispatchEvent(new CustomEvent('copy'));
    });

    document.getElementById('paste-pattern')?.addEventListener('click', () => {
      this.container.dispatchEvent(new CustomEvent('paste'));
    });

    document.getElementById('clear-pattern')?.addEventListener('click', () => {
      if (confirm(`Clear pattern ${this.currentPattern}?`)) {
        this.container.dispatchEvent(new CustomEvent('clear'));
      }
    });
  }

  private attachKeyboard(): void {
    document.addEventListener('keydown', (e) => {
      if (e.target instanceof HTMLInputElement) return;

      // 1-8 to select patterns
      if (e.key >= '1' && e.key <= '8') {
        const patterns = ['A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'];
        const index = parseInt(e.key, 10) - 1;
        const pattern = patterns[index];
        if (pattern) this.selectPattern(pattern);
        return;
      }

      // Ctrl+C to copy
      if (e.ctrlKey && e.key === 'c') {
        e.preventDefault();
        this.container.dispatchEvent(new CustomEvent('copy'));
        return;
      }

      // Ctrl+V to paste
      if (e.ctrlKey && e.key === 'v') {
        e.preventDefault();
        this.container.dispatchEvent(new CustomEvent('paste'));
      }
    });
  }

  private selectPattern(pattern: string): void {
    this.currentPattern = pattern;
    this.onSelect(pattern);
    this.updateUI();
  }

  private updateUI(): void {
    this.container.querySelectorAll('.pattern-btn').forEach((btn) => {
      const pattern = (btn as HTMLElement).dataset.pattern;
      if (pattern) {
        btn.classList.toggle('active', pattern === this.currentPattern);
      }
    });
  }
}
