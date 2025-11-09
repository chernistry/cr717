# Ticket 14: Cross-Browser Testing & Polish

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 13

---

## Objective

Ensure compatibility across browsers and polish UX.

---

## Acceptance Criteria

- ✅ Works in Chrome 90+, Firefox 88+, Safari 14+
- ✅ Responsive on desktop and tablet
- ✅ Accessible (keyboard navigation, ARIA labels)
- ✅ Error messages for unsupported browsers
- ✅ Loading states and feedback
- ✅ README with setup instructions

---

## Implementation

### 1. Keyboard Shortcuts

Update `src/main.ts`:
```typescript
document.addEventListener('keydown', (e) => {
  if (e.code === 'Space') {
    e.preventDefault();
    // Toggle play/stop
  }
});
```

### 2. ARIA Labels

Update `index.html`:
```html
<button id="play-btn" aria-label="Start playback">▶ Play</button>
<button id="stop-btn" aria-label="Stop playback">■ Stop</button>
<input type="range" id="bpm-slider" aria-label="Tempo in beats per minute" />
```

### 3. Browser Compatibility Check

Create `src/utils/browserCheck.ts`:
```typescript
export function checkBrowserSupport(): { supported: boolean; message?: string } {
  if (!window.AudioContext && !(window as any).webkitAudioContext) {
    return {
      supported: false,
      message: 'Web Audio API not supported. Please use a modern browser.',
    };
  }

  if (!window.localStorage) {
    return {
      supported: false,
      message: 'localStorage not available. Pattern saving will be disabled.',
    };
  }

  return { supported: true };
}
```

### 4. Error Handling UI

Add to `index.html`:
```html
<div id="error-banner" style="display: none;" role="alert">
  <p id="error-message"></p>
</div>
```

### 5. README.md

```markdown
# CR-717 Drum Machine

Web-based emulation of the classic Roland CR-717 drum machine.

## Features

- 5 drum voices: Bass Drum, Snare, Closed Hi-Hat, Open Hi-Hat, Clap
- 16-step sequencer
- Adjustable tempo (60-240 BPM)
- Pattern save/load via localStorage
- Real-time step highlighting

## Setup

```bash
npm install
npm run dev
```

## Build

```bash
npm run build
npm run preview
```

## Testing

```bash
npm test          # Unit tests
npm run e2e       # E2E tests
```

## Browser Support

- Chrome 90+
- Firefox 88+
- Safari 14+

## License

MIT
```

---

## Affected Files

**New:**
- `src/utils/browserCheck.ts`
- `README.md`

**Modified:**
- `index.html` (ARIA labels)
- `src/main.ts` (keyboard shortcuts)
- `styles.css` (polish)

---

## Tests

- E2E: Run in all browsers
- Manual: Keyboard navigation
- Manual: Screen reader testing

---

## Definition of Done

- [ ] Works in Chrome, Firefox, Safari
- [ ] Keyboard shortcuts work
- [ ] ARIA labels added
- [ ] Error handling works
- [ ] README complete
- [ ] Committed: "feat: add cross-browser support and accessibility"
