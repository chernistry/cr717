import type { PatternBank } from '../state/types';

const STORAGE_KEY = 'tr808:v1:bank';

export function saveBank(bank: PatternBank): void {
  try {
    const json = JSON.stringify(bank);
    localStorage.setItem(STORAGE_KEY, json);
  } catch (error) {
    if (error instanceof DOMException && error.name === 'QuotaExceededError') {
      console.error('localStorage quota exceeded');
    } else {
      console.error('Failed to save bank:', error);
    }
  }
}

export function loadBank(): PatternBank | null {
  try {
    const json = localStorage.getItem(STORAGE_KEY);
    if (!json) return null;

    const bank = JSON.parse(json) as PatternBank;

    // Validate structure
    if (!bank.patterns || !bank.currentPattern) {
      console.error('Invalid bank structure');
      return null;
    }

    return bank;
  } catch (error) {
    console.error('Failed to load bank:', error);
    return null;
  }
}

export function clearBank(): void {
  localStorage.removeItem(STORAGE_KEY);
}

// Legacy support
export function loadPattern() {
  return null;
}

export function savePattern() {
  // No-op, use saveBank instead
}
