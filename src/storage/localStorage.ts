import type { Pattern } from '../state/types';

const STORAGE_KEY = 'tr808:v1:pattern';

export function savePattern(pattern: Pattern): void {
  try {
    const json = JSON.stringify(pattern);
    localStorage.setItem(STORAGE_KEY, json);
  } catch (error) {
    if (error instanceof DOMException && error.name === 'QuotaExceededError') {
      console.error('localStorage quota exceeded');
    } else {
      console.error('Failed to save pattern:', error);
    }
  }
}

export function loadPattern(): Pattern | null {
  try {
    const json = localStorage.getItem(STORAGE_KEY);
    if (!json) return null;

    const pattern = JSON.parse(json) as Pattern;

    // Validate structure
    if (
      !pattern.name ||
      !pattern.bpm ||
      !pattern.steps ||
      !Array.isArray(pattern.steps.BD) ||
      pattern.steps.BD.length !== 16
    ) {
      console.error('Invalid pattern structure');
      return null;
    }

    return pattern;
  } catch (error) {
    console.error('Failed to load pattern:', error);
    return null;
  }
}

export function clearPattern(): void {
  localStorage.removeItem(STORAGE_KEY);
}
