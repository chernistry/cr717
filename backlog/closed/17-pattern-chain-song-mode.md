# Ticket 17: Pattern Chain & Song Mode

**Status:** 🟡 Open  
**Priority:** P2 (Should have)  
**Estimated Time:** 4-6 hours  
**Dependencies:** Ticket 15

---

## Objective

Multiple pattern slots (A-H) with chain mode to create full songs. Essential for real TR-808 workflow.

---

## Background

Original TR-808 has:
- 12 pattern banks (I/II) × 12 patterns = 24 total
- Manual mode: play one pattern
- Compose mode: chain patterns into songs
- Pattern length: 12 or 16 steps (switchable)

We'll implement:
- 8 pattern slots (A-H)
- Pattern chain mode
- Copy/paste patterns
- Pattern length: 16 steps (fixed for MVP)

---

## Acceptance Criteria

- ✅ 8 pattern slots (A-H) stored in localStorage
- ✅ Pattern selector UI (buttons A-H)
- ✅ Current pattern highlighted
- ✅ Copy/Paste pattern (Ctrl+C / Ctrl+V)
- ✅ Clear pattern button
- ✅ Chain mode: queue patterns to play in sequence
- ✅ Chain editor: drag-drop pattern order
- ✅ Chain loops automatically
- ✅ Visual indicator of current pattern in chain
- ✅ Keyboard shortcuts: 1-8 to select pattern

---

## Implementation

### 1. Update State

**src/state/types.ts:**
```typescript
export interface PatternBank {
  patterns: Record<string, Pattern>; // A-H
  currentPattern: string; // 'A'
  chain: string[]; // ['A', 'B', 'A', 'C']
  chainMode: boolean;
  chainPosition: number;
}

export interface AppState {
  bank: PatternBank;
  playback: PlaybackState;
}
```

### 2. Pattern Selector UI

**src/ui/patternSelector.ts:**
```typescript
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
          <button class="action-btn" id="copy-pattern" title="Copy (Ctrl+C)">📋</button>
          <button class="action-btn" id="paste-pattern" title="Paste (Ctrl+V)">📄</button>
          <button class="action-btn" id="clear-pattern" title="Clear">🗑️</button>
        </div>
      </div>
    `;

    this.attachEvents();
  }

  private attachEvents(): void {
    this.container.querySelectorAll('.pattern-btn').forEach((btn) => {
      btn.addEventListener('click', () => {
        const pattern = (btn as HTMLElement).dataset.pattern!;
        this.selectPattern(pattern);
      });
    });

    document.getElementById('copy-pattern')?.addEventListener('click', () => {
      this.copyPattern();
    });

    document.getElementById('paste-pattern')?.addEventListener('click', () => {
      this.pastePattern();
    });

    document.getElementById('clear-pattern')?.addEventListener('click', () => {
      this.clearPattern();
    });
  }

  selectPattern(pattern: string): void {
    this.currentPattern = pattern;
    this.onSelect(pattern);
    this.updateUI();
  }

  private updateUI(): void {
    this.container.querySelectorAll('.pattern-btn').forEach((btn) => {
      const pattern = (btn as HTMLElement).dataset.pattern!;
      btn.classList.toggle('active', pattern === this.currentPattern);
    });
  }

  private copyPattern(): void {
    // Emit event
    this.container.dispatchEvent(new CustomEvent('copy'));
  }

  private pastePattern(): void {
    this.container.dispatchEvent(new CustomEvent('paste'));
  }

  private clearPattern(): void {
    if (confirm(`Clear pattern ${this.currentPattern}?`)) {
      this.container.dispatchEvent(new CustomEvent('clear'));
    }
  }
}
```

### 3. Chain Editor

**src/ui/chainEditor.ts:**
```typescript
export class ChainEditor {
  private container: HTMLElement;
  private chain: string[] = [];
  private onChange: (chain: string[]) => void;

  constructor(containerId: string, onChange: (chain: string[]) => void) {
    const el = document.getElementById(containerId);
    if (!el) throw new Error(`Container ${containerId} not found`);
    this.container = el;
    this.onChange = onChange;
    this.render();
  }

  setChain(chain: string[]): void {
    this.chain = chain;
    this.render();
  }

  private render(): void {
    this.container.innerHTML = `
      <div class="chain-editor">
        <div class="chain-title">Pattern Chain</div>
        <div class="chain-slots">
          ${this.chain
            .map(
              (p, i) => `
            <div class="chain-slot" data-index="${i}">
              <span>${p}</span>
              <button class="remove-btn" data-index="${i}">×</button>
            </div>
          `
            )
            .join('')}
          <button class="add-chain-btn">+ Add</button>
        </div>
      </div>
    `;

    this.attachEvents();
  }

  private attachEvents(): void {
    this.container.querySelectorAll('.remove-btn').forEach((btn) => {
      btn.addEventListener('click', () => {
        const index = parseInt((btn as HTMLElement).dataset.index!, 10);
        this.chain.splice(index, 1);
        this.onChange(this.chain);
        this.render();
      });
    });

    this.container.querySelector('.add-chain-btn')?.addEventListener('click', () => {
      const pattern = prompt('Add pattern (A-H):')?.toUpperCase();
      if (pattern && /^[A-H]$/.test(pattern)) {
        this.chain.push(pattern);
        this.onChange(this.chain);
        this.render();
      }
    });
  }
}
```

### 4. Update Scheduler for Chain Mode

**src/audio/scheduler.ts:**
```typescript
export class AudioScheduler implements Scheduler {
  // ... existing code ...
  
  private chainMode = false;
  private chain: string[] = [];
  private chainPosition = 0;
  private stepsInPattern = 16;
  private currentStepInChain = 0;

  startChain(
    ctx: AudioContext,
    bpm: number,
    chain: string[],
    onStep: (pattern: string, step: number, time: number) => void
  ): void {
    this.chainMode = true;
    this.chain = chain;
    this.chainPosition = 0;
    this.currentStepInChain = 0;

    const stepDuration = 60 / bpm / 4;
    this.nextNoteTime = ctx.currentTime;
    this.running = true;

    this.intervalId = window.setInterval(() => {
      const end = ctx.currentTime + this.scheduleAheadTime;
      while (this.nextNoteTime < end && this.running) {
        const currentPattern = this.chain[this.chainPosition];
        const stepInPattern = this.currentStepInChain % this.stepsInPattern;

        onStep(currentPattern, stepInPattern, this.nextNoteTime);

        this.nextNoteTime += stepDuration;
        this.currentStepInChain++;

        // Move to next pattern after 16 steps
        if (this.currentStepInChain >= this.stepsInPattern) {
          this.currentStepInChain = 0;
          this.chainPosition = (this.chainPosition + 1) % this.chain.length;
        }
      }
    }, this.tickInterval);
  }
}
```

### 5. CSS

```css
.pattern-selector {
  display: flex;
  flex-direction: column;
  gap: 8px;
}

.pattern-buttons {
  display: flex;
  gap: 4px;
}

.pattern-btn {
  flex: 1;
  padding: 8px;
  background: var(--color-bg-tertiary);
  border: 2px solid var(--color-border);
  color: var(--color-text-primary);
  cursor: pointer;
  border-radius: var(--radius-sm);
  font-weight: bold;
  transition: all var(--transition-fast);
}

.pattern-btn:hover {
  background: var(--color-bg-elevated);
}

.pattern-btn.active {
  background: var(--color-accent);
  border-color: var(--color-accent);
  color: white;
}

.pattern-actions {
  display: flex;
  gap: 4px;
}

.action-btn {
  flex: 1;
  padding: 6px;
  background: var(--color-bg-tertiary);
  border: 1px solid var(--color-border);
  cursor: pointer;
  border-radius: var(--radius-sm);
  font-size: 16px;
}

.chain-editor {
  padding: 12px;
  background: var(--color-bg-tertiary);
  border-radius: var(--radius-md);
}

.chain-title {
  font-size: 12px;
  font-weight: bold;
  margin-bottom: 8px;
  color: var(--color-text-secondary);
}

.chain-slots {
  display: flex;
  gap: 4px;
  flex-wrap: wrap;
}

.chain-slot {
  display: flex;
  align-items: center;
  gap: 4px;
  padding: 4px 8px;
  background: var(--color-accent);
  border-radius: var(--radius-sm);
  color: white;
  font-weight: bold;
}

.remove-btn {
  background: none;
  border: none;
  color: white;
  cursor: pointer;
  font-size: 18px;
  line-height: 1;
}

.add-chain-btn {
  padding: 4px 12px;
  background: var(--color-bg-elevated);
  border: 1px dashed var(--color-border);
  cursor: pointer;
  border-radius: var(--radius-sm);
  color: var(--color-text-secondary);
}
```

---

## Tests

- Unit: Pattern switching works
- Unit: Chain advances correctly
- E2E: Select pattern A, add notes, switch to B, different notes
- E2E: Chain mode plays patterns in sequence
- E2E: Copy/paste pattern works

---

## Affected Files

**New:**
- `src/ui/patternSelector.ts`
- `src/ui/chainEditor.ts`

**Modified:**
- `src/state/types.ts` - PatternBank
- `src/audio/scheduler.ts` - Chain mode
- `src/storage/localStorage.ts` - Save all patterns
- `src/main.ts` - Wire pattern switching
- `index.html` - Add pattern selector
- `styles.css` - Pattern selector styles

---

## Definition of Done

- [ ] 8 pattern slots working
- [ ] Pattern switching instant
- [ ] Copy/paste works
- [ ] Chain mode plays sequence
- [ ] All patterns saved
- [ ] Tests pass
- [ ] Committed: "feat: add pattern bank (A-H) and chain mode"
