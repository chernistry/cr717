# Ticket 07: Sequencer Grid UI

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 01

---

## Objective

Create 16x5 step grid with click handlers and visual feedback.

---

## Acceptance Criteria

- ✅ 16 columns (steps) × 5 rows (instruments)
- ✅ Click toggles step on/off
- ✅ Visual feedback (active/inactive states)
- ✅ Responsive layout (desktop + tablet)
- ✅ Event delegation for performance

---

## Implementation

### Create `src/ui/dom.ts`
```typescript
export function createElement<K extends keyof HTMLElementTagNameMap>(
  tag: K,
  className?: string,
  attributes?: Record<string, string>
): HTMLElementTagNameMap[K] {
  const el = document.createElement(tag);
  if (className) el.className = className;
  if (attributes) {
    Object.entries(attributes).forEach(([key, value]) => {
      el.setAttribute(key, value);
    });
  }
  return el;
}
```

### Create `src/ui/sequencerGrid.ts`
```typescript
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
```

### Update `styles.css`
```css
.sequencer-row {
  display: flex;
  gap: 4px;
  margin-bottom: 8px;
  align-items: center;
}

.instrument-label {
  width: 40px;
  font-weight: bold;
  font-size: 14px;
}

.step-button {
  width: 50px;
  height: 50px;
  border: 2px solid #444;
  background: #2a2a2a;
  cursor: pointer;
  transition: all 0.1s;
}

.step-button:hover {
  background: #3a3a3a;
}

.step-button.active {
  background: #ff6b35;
  border-color: #ff6b35;
}

.step-button.current {
  box-shadow: 0 0 10px #00ff00;
}
```

---

## Affected Files

**New:**
- `src/ui/dom.ts`
- `src/ui/sequencerGrid.ts`

**Modified:**
- `styles.css`

---

## Tests

- E2E: Click step → verify class toggle
- E2E: Verify responsive layout

---

## Definition of Done

- [ ] Grid renders 16x5
- [ ] Click toggles work
- [ ] Visual feedback works
- [ ] Responsive on tablet
- [ ] Committed: "feat(ui): add sequencer grid"
