# Ticket 13: Performance Optimization

**Status:** 🟡 Open  
**Priority:** P1 (Must have)  
**Estimated Time:** 3-4 hours  
**Dependencies:** Ticket 12

---

## Objective

Meet all performance SLOs and optimize bundle size.

---

## Acceptance Criteria

- ✅ Bundle size <50KB gzipped
- ✅ Jitter <2ms @ 120 BPM
- ✅ 0 long tasks during playback
- ✅ Memory <100MB during 5min playback
- ✅ Load time <2s on 3G
- ✅ Lighthouse score >90

---

## Implementation

### 1. Bundle Size Optimization

Update `vite.config.ts`:
```typescript
import { defineConfig } from 'vite';

export default defineConfig({
  build: {
    target: 'es2020',
    minify: 'terser',
    terserOptions: {
      compress: {
        drop_console: true,
        drop_debugger: true,
      },
    },
    rollupOptions: {
      output: {
        manualChunks: undefined,
      },
    },
  },
  test: {
    globals: true,
    environment: 'jsdom',
  },
});
```

### 2. Performance Monitoring

Create `src/utils/performance.ts`:
```typescript
export function measureJitter(
  ctx: AudioContext,
  scheduledTime: number
): number {
  const { contextTime } = ctx.getOutputTimestamp();
  return Math.abs(contextTime - scheduledTime) * 1000; // ms
}

export function observeLongTasks(callback: (duration: number) => void): void {
  if ('PerformanceObserver' in window) {
    const observer = new PerformanceObserver((list) => {
      for (const entry of list.getEntries()) {
        callback(entry.duration);
      }
    });
    observer.observe({ entryTypes: ['longtask'] });
  }
}
```

### 3. Memory Leak Prevention

- Ensure all `setInterval` are cleared
- Disconnect audio nodes after use
- Remove event listeners on cleanup
- Cancel `requestAnimationFrame` on stop

### 4. Optimization Checklist

- [ ] Profile with Chrome DevTools Performance tab
- [ ] Check bundle size: `npm run build && ls -lh dist/assets`
- [ ] Run Lighthouse audit
- [ ] Measure jitter during playback
- [ ] Monitor memory usage over 5 minutes
- [ ] Test on 3G throttling

---

## Affected Files

**New:**
- `src/utils/performance.ts`

**Modified:**
- `vite.config.ts`
- Various files (performance fixes)

---

## Tests

- Performance: Bundle size <50KB gzipped
- Performance: Lighthouse score >90
- Performance: Memory profiling

---

## Definition of Done

- [ ] All SLOs met
- [ ] Bundle optimized
- [ ] No memory leaks
- [ ] Lighthouse >90
- [ ] Committed: "perf: optimize bundle and runtime performance"
