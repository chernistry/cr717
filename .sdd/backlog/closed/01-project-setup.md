# Ticket 01: Project Setup & Foundation

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 2-3 hours  
**Dependencies:** None

---

## Objective

Initialize CR-717 project with Vite, TypeScript, and testing tools (Vitest + Playwright).

---

## Acceptance Criteria (DoD)

- ✅ Vite project created with TypeScript template
- ✅ Vitest configured for unit tests
- ✅ Playwright configured for E2E tests
- ✅ ESLint + Prettier configured with strict rules
- ✅ `npm run dev` starts dev server on http://localhost:5173
- ✅ `npm run build` produces optimized bundle in `dist/`
- ✅ `npm run test` runs unit tests (even if empty)
- ✅ `npm run e2e` runs E2E tests (even if empty)
- ✅ Project structure created: `src/`, `tests/`, `public/`
- ✅ Basic `index.html` with placeholder for sequencer grid

---

## Steps

### 1. Create Vite Project
```bash
cd /Users/sasha/IdeaProjects/personal_projects/cr717
npm create vite@latest . -- --template vanilla-ts
npm install
```

### 2. Install Testing Dependencies
```bash
npm install -D vitest @vitest/ui
npm install -D @playwright/test
npx playwright install
```

### 3. Install Dev Tools
```bash
npm install -D eslint @typescript-eslint/parser @typescript-eslint/eslint-plugin
npm install -D prettier eslint-config-prettier
```

### 4. Create Config Files

**vite.config.ts:**
```typescript
import { defineConfig } from 'vite';

export default defineConfig({
  test: {
    globals: true,
    environment: 'jsdom',
  },
});
```

**playwright.config.ts:**
```typescript
import { defineConfig } from '@playwright/test';

export default defineConfig({
  testDir: './tests/e2e',
  use: {
    baseURL: 'http://localhost:5173',
  },
  webServer: {
    command: 'npm run dev',
    port: 5173,
    reuseExistingServer: true,
  },
});
```

**.eslintrc.json:**
```json
{
  "parser": "@typescript-eslint/parser",
  "extends": [
    "eslint:recommended",
    "plugin:@typescript-eslint/recommended",
    "prettier"
  ],
  "rules": {
    "@typescript-eslint/no-unused-vars": "error",
    "@typescript-eslint/explicit-function-return-type": "warn"
  }
}
```

**.prettierrc:**
```json
{
  "semi": true,
  "singleQuote": true,
  "printWidth": 88,
  "tabWidth": 2
}
```

### 5. Update package.json Scripts
```json
{
  "scripts": {
    "dev": "vite",
    "build": "tsc && vite build",
    "preview": "vite preview",
    "test": "vitest run",
    "test:watch": "vitest",
    "test:ui": "vitest --ui",
    "e2e": "playwright test",
    "e2e:ui": "playwright test --ui",
    "lint": "eslint . --ext .ts,.tsx",
    "format": "prettier --write \"src/**/*.{ts,tsx,css}\""
  }
}
```

### 6. Create Project Structure
```bash
mkdir -p src/{audio,ui,state,storage}
mkdir -p src/audio/voices
mkdir -p tests/{unit,e2e}
mkdir -p public
```

### 7. Create Basic index.html
```html
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>CR-717 Drum Machine</title>
  <link rel="stylesheet" href="/styles.css">
</head>
<body>
  <div id="app">
    <h1>CR-717 Drum Machine</h1>
    <div id="controls">
      <!-- Controls will be added in Ticket 08 -->
    </div>
    <div id="sequencer">
      <!-- Sequencer grid will be added in Ticket 07 -->
    </div>
  </div>
  <script type="module" src="/src/main.ts"></script>
</body>
</html>
```

### 8. Create Basic styles.css
```css
* {
  margin: 0;
  padding: 0;
  box-sizing: border-box;
}

body {
  font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif;
  background: #1a1a1a;
  color: #fff;
  padding: 20px;
}

#app {
  max-width: 1200px;
  margin: 0 auto;
}

h1 {
  text-align: center;
  margin-bottom: 30px;
  font-size: 2.5rem;
  color: #ff6b35;
}
```

### 9. Create Basic src/main.ts
```typescript
console.log('CR-717 initialized');

// Entry point - will be expanded in later tickets
```

### 10. Create Placeholder Tests
```typescript
// tests/unit/example.test.ts
import { describe, it, expect } from 'vitest';

describe('Example', () => {
  it('should pass', () => {
    expect(true).toBe(true);
  });
});
```

```typescript
// tests/e2e/example.spec.ts
import { test, expect } from '@playwright/test';

test('page loads', async ({ page }) => {
  await page.goto('/');
  await expect(page.locator('h1')).toContainText('CR-717');
});
```

---

## Affected Files

**New Files:**
- `vite.config.ts` - Vite configuration
- `playwright.config.ts` - Playwright configuration
- `.eslintrc.json` - ESLint rules
- `.prettierrc` - Prettier config
- `tsconfig.json` - TypeScript strict mode
- `index.html` - Main HTML
- `styles.css` - Base styles
- `src/main.ts` - Entry point
- `tests/unit/example.test.ts` - Placeholder unit test
- `tests/e2e/example.spec.ts` - Placeholder E2E test

**Modified Files:**
- `package.json` - Dependencies and scripts

---

## Tests

### Verification Steps
1. Run `npm run dev` → Should start dev server without errors
2. Open http://localhost:5173 → Should see "CR-717 Drum Machine" heading
3. Run `npm run build` → Should create `dist/` folder
4. Run `npm run test` → Should run and pass placeholder test
5. Run `npm run e2e` → Should run and pass page load test
6. Run `npm run lint` → Should pass with no errors

---

## Risks & Mitigations

**Risk:** Dependency conflicts  
**Mitigation:** Use exact versions, commit package-lock.json

**Risk:** Playwright installation fails  
**Mitigation:** Run `npx playwright install --with-deps`

**Risk:** Port 5173 already in use  
**Mitigation:** Change port in vite.config.ts

---

## Notes

- Keep dependencies minimal - only add what's needed
- Use TypeScript strict mode from the start
- All paths should be relative to project root
- Follow naming conventions: kebab-case for files, PascalCase for types

---

## Definition of Done Checklist

- [ ] All commands run without errors
- [ ] Dev server starts and page loads
- [ ] Build produces optimized bundle
- [ ] Tests run (even if placeholder)
- [ ] Linting passes
- [ ] Project structure created
- [ ] README.md updated with setup instructions
- [ ] Committed to git with message: "feat: initial project setup with Vite, TypeScript, Vitest, Playwright"
