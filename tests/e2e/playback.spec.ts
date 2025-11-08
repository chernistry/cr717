import { test, expect } from '@playwright/test';

test.describe('TR-808 Playback', () => {
  test.beforeEach(async ({ page }) => {
    await page.goto('/');
  });

  test('should load page with title', async ({ page }) => {
    await expect(page.locator('.logo')).toContainText('TR-808');
  });

  test('should start playback on Play button', async ({ page }) => {
    const playBtn = page.getByRole('button', { name: /play/i });
    await playBtn.click();

    await expect(playBtn).toBeDisabled();
    const stopBtn = page.getByRole('button', { name: /stop/i });
    await expect(stopBtn).toBeEnabled();
  });

  test('should stop playback', async ({ page }) => {
    await page.getByRole('button', { name: /play/i }).click();
    await page.waitForTimeout(100);

    const stopBtn = page.getByRole('button', { name: /stop/i });
    await stopBtn.click();

    await expect(stopBtn).toBeDisabled();
    const playBtn = page.getByRole('button', { name: /play/i });
    await expect(playBtn).toBeEnabled();
  });

  test('should toggle steps', async ({ page }) => {
    const step = page.locator('[data-instrument="BD"][data-step="0"]');
    await step.click();
    await expect(step).toHaveClass(/active/);

    await step.click();
    await expect(step).not.toHaveClass(/active/);
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

  test('should persist pattern on reload', async ({ page }) => {
    // Toggle a step
    const step = page.locator('[data-instrument="BD"][data-step="0"]');
    await step.click();
    await expect(step).toHaveClass(/active/);

    // Reload page
    await page.reload();

    // Check step is still active
    const reloadedStep = page.locator('[data-instrument="BD"][data-step="0"]');
    await expect(reloadedStep).toHaveClass(/active/);
  });

  test('should render all 5 instruments', async ({ page }) => {
    const instruments = ['BD', 'SD', 'CH', 'OH', 'CP'];

    for (const inst of instruments) {
      const label = page.locator('.instrument-label', { hasText: inst });
      await expect(label).toBeVisible();
    }
  });

  test('should render 16 steps per instrument', async ({ page }) => {
    const bdSteps = page.locator('[data-instrument="BD"]');
    await expect(bdSteps).toHaveCount(16);
  });

  test('should show help overlay', async ({ page }) => {
    const helpBtn = page.getByRole('button', { name: /keyboard shortcuts/i });
    await helpBtn.click();

    const overlay = page.locator('.help-overlay');
    await expect(overlay).toHaveClass(/visible/);
  });

  test('should toggle theme', async ({ page }) => {
    const themeBtn = page.locator('#theme-toggle');
    await themeBtn.click();

    const html = page.locator('html');
    await expect(html).toHaveAttribute('data-theme', 'light');
  });
});
