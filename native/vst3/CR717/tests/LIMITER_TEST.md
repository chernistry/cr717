# True-Peak Limiter - Manual Test Plan

## Unit Tests

See `tests/unit/dsp/test_limiter.cpp`:
1. **Ceiling Enforcement**: 0dBFS input → verify peak ≤ -0.3dBFS
2. **Transient Preservation**: Impulse → verify limited but not destroyed
3. **Oversampling Toggle**: Both modes work without crashes
4. **Soft Knee**: Hard vs soft knee processing

## Manual DAW Tests

### Test 1: No Overshoots (True-Peak)
1. Load plugin, create pattern with BD + SD
2. Set Limiter: Ceiling=-0.3dB, Lookahead=5ms, Oversampling=ON
3. Drive input hot (master fader up)
4. **Expected**: No peaks above -0.3dBFS on true-peak meter
5. **Verify**: Disable oversampling → may see overshoots

### Test 2: Transient Handling
1. Pattern with sharp transients (CP, SD)
2. Lookahead=0ms → slight overshoot/distortion
3. Lookahead=5ms → clean transient limiting
4. Lookahead=10ms → very clean but 10ms latency
5. **Expected**: More lookahead = cleaner limiting

### Test 3: Release Behavior
1. Sustained sound (OH, CY) hitting limiter
2. Release=10ms (fast) → pumping/breathing
3. Release=100ms (medium) → smooth recovery
4. Release=1000ms (slow) → long recovery tail
5. **Expected**: Musical release times, no clicks

### Test 4: Soft Knee
1. Signal near ceiling
2. Knee=0dB (hard) → abrupt limiting
3. Knee=3dB (soft) → gentle limiting
4. **Expected**: Softer knee = more transparent

### Test 5: CPU Impact (Oversampling)
1. Oversampling=ON → check CPU meter
2. Oversampling=OFF → check CPU meter
3. **Expected**: ~2-4x CPU increase with oversampling

### Test 6: Ceiling Range
1. Ceiling=-0.3dB → safe for streaming/mastering
2. Ceiling=0.0dB → absolute maximum
3. **Expected**: No intersample peaks with oversampling ON

## Acceptance Criteria

✅ No overshoots beyond Ceiling with true-peak on (±0.1dB tolerance)  
✅ Kick/snare transients preserved with reasonable release  
✅ No obvious distortion at -0.3dBFS ceiling  
✅ Lookahead improves transient handling  
✅ Oversampling toggle works (4x CPU increase)  
✅ Soft knee provides transparent limiting  
✅ Unit tests verify ceiling enforcement  

## Performance Benchmarks

- **Oversampling OFF**: <1% CPU (48kHz/64 samples)
- **Oversampling ON (4x)**: 2-4% CPU (48kHz/64 samples)
- **Latency**: 0-10ms (user-configurable)
- **True-peak accuracy**: ±0.1dB with 4x oversampling

## Known Limitations

- Auto-release not implemented (fixed release time)
- No external sidechain input
- Oversampling uses linear-phase FIR (adds latency)
- No gain reduction metering in UI yet
