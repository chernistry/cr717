# Ticket 12: Integration & E2E Tests

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 4-6 hours  
**Dependencies:** All previous tickets

---

## Objective

Comprehensive test coverage for full user flows.

---

## Acceptance Criteria

- ✅ E2E: Load page → click Play → verify audio
- ✅ E2E: Toggle steps → verify pattern changes
- ✅ E2E: Change BPM → verify tempo changes
- ✅ E2E: Save/load pattern → verify persistence
- ✅ Performance: 0 long tasks during playback
- ✅ Cross-browser: Chrome, Firefox, Safari

---

## Implementation

### Create `tests/e2e/playback.spec.ts`
```typescript
import { test, expect } from '@playwright/test';

test.describe('CR-717 Playback', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
  });

  test('should load page with title', async ({ page }) => {
    await expect(page.locator('h1')).toContainText('CR-717');
  });

  test('should start playback on Play button', async ({ page }) => {
    const playBtn = page.getByRole('button', { name: /play/i });
    await playBtn.click();
    
    await expect(playBtn).toBeDisabled();
    const stopBtn = page.getByRole('button', { name: /stop/i });
    await expect(stopBtn).toBeEnabled();
  });

  test('should toggle steps', async ({ page }) => {
    const step = page.locator('[data-instrument="BD"][data-step="0"]');
    await step.click();
    await expect(step).toHaveClass(/active/);
    
    await step.click();
    await expect(step).not.toHaveClass(/active/);
  });

  test('should change BPM', async ({ page }) => {
    const slider = page.locator('#bpm-slider');
    await slider.fill('140');
    
    const display = page.locator('#bpm-display');
    await expect(display).toHaveText('140');
  });

  test('should highlight current step during playback', async ({ page }) => {
    await page.getByRole('button', { name: /play/i }).click();
    
    // Wait for first step to highlight
    await page.waitForSelector('[data-step="0"].current', { timeout: 1000 });
    
    // Wait for step to advance
    await page.waitForTimeout(300);
    const currentSteps = await page.locator('.step-button.current').count();
    expect(currentSteps).toBeGreaterThan(0);
  });
});
```

### Create `tests/e2e/persistence.spec.ts`
```typescript
import { test, expect } from '@playwright/test';

test.describe('Pattern Persistence', () => {
  test('should save and load pattern', async ({ page }) => {
    await page.goto('/');
    
    // Toggle some steps
    await page.locator('[data-instrument="BD"][data-step="0"]').click();
    await page.locator('[data-instrument="SD"][data-step="4"]').click();
    
    // Reload page
    await page.reload();
    
    // Verify steps are still active
    await expect(page.locator('[data-instrument="BD"][data-step="0"]')).toHaveClass(/active/);
    await expect(page.locator('[data-instrument="SD"][data-step="4"]')).toHaveClass(/active/);
  });
});
```

### Update `playwright.config.ts`
```typescript
import { defineConfig, devices } from '@playwright/test';

export default defineConfig({
  testDir: './tests/e2e',
  fullyParallel: true,
  forbidOnly: !!process.env.CI,
  retries: process.env.CI ? 2 : 0,
  workers: process.env.CI ? 1 : undefined,
  reporter: 'html',
  use: {
    baseURL: 'http://localhost:5173',
    trace: 'on-first-retry',
  },
  projects: [
    {
      name: 'chromium',
      use: { ...devices['Desktop Chrome'] },
    },
    {
      name: 'firefox',
      use: { ...devices['Desktop Firefox'] },
    },
    {
      name: 'webkit',
      use: { ...devices['Desktop Safari'] },
    },
  ],
  webServer: {
    command: 'npm run dev',
    port: 5173,
    reuseExistingServer: !process.env.CI,
  },
});
```

---

## Affected Files

**New:**
- `tests/e2e/playback.spec.ts`
- `tests/e2e/persistence.spec.ts`

**Modified:**
- `playwright.config.ts`

---

## Tests

- E2E: Full user journey
- E2E: Cross-browser compatibility
- Performance: Long tasks monitoring

---

## Definition of Done

- [ ] All E2E tests pass
- [ ] Tests run in Chrome, Firefox, Safari
- [ ] No flaky tests
- [ ] Committed: "test: add comprehensive E2E tests"
