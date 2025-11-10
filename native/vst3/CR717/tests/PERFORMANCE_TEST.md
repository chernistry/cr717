# Performance & Calibration Tests

## Calibration Targets

- **Nominal Level**: -18 dBFS
- **Headroom**: 18 dB for dynamics processing
- **True-Peak Ceiling**: -0.3 dBFS (streaming safe)
- **Compressor GR**: 2-4 dB on typical drum pattern
- **Limiter**: No overshoots beyond ceiling (±0.1 dB)

## Performance Budgets

- **CPU**: <10% on M1 Mac @ 48kHz/64 samples
- **Memory**: <100MB after 5 min playback
- **Latency**: 0-10ms (user-configurable via lookahead)
- **Jitter**: <2ms timing accuracy
- **Long Tasks**: 0 during playback

## Manual DAW Tests

### 1. Gain Structure
1. Load plugin in DAW
2. Create pattern with all voices
3. Set master level to -18 dBFS nominal
4. Enable compressor (default settings)
5. **Expected**: 2-4 dB GR, no clipping

### 2. True-Peak Limiting
1. Drive input hot (master fader up)
2. Enable limiter (ceiling -0.3dB, oversampling ON)
3. Check true-peak meter in DAW
4. **Expected**: No peaks above -0.3 dBFS

### 3. Clipper Loudness
1. Enable clipper (drive 12dB, mix 50%)
2. Measure RMS before/after
3. **Expected**: Loudness within ±0.5 dB

### 4. FX Stability
1. Set delay feedback to 90%
2. Play pattern for 30 seconds
3. **Expected**: No runaway feedback, stable tail

### 5. CPU Usage
1. Play pattern continuously
2. Monitor DAW CPU meter
3. Toggle oversampling ON/OFF
4. **Expected**: 
   - Oversampling OFF: <5% CPU
   - Oversampling ON: <15% CPU

## Automated Tests

Run integration test:
```bash
# Build test
clang++ -std=c++17 -I./build/_deps/juce-src/modules -I./Source \
  tests/integration/test_dynamics_chain.cpp -o test_dynamics \
  -framework Cocoa -framework CoreAudio -framework AudioToolbox

# Run
./test_dynamics
```

Run pluginval:
```bash
pluginval --strictness-level 8 --validate "build/CR717_artefacts/Release/VST3/Cherni CR-717.vst3"
```

## Acceptance Criteria

✅ True-peak limiter prevents overshoots (±0.1 dB)  
✅ Compressor GR 2-4 dB on test pattern  
✅ Clipper maintains loudness (±0.5 dB)  
✅ FX tails stable (no runaway)  
✅ CPU <10% typical, <15% with oversampling  
✅ Memory <100MB after 5 min  
✅ pluginval passes strictness level 8  
✅ No audio dropouts/clicks  

## Known Limitations

- Oversampling adds 2-4x CPU overhead
- Lookahead adds 0-10ms latency
- No auto-release on limiter yet
- No gain reduction metering in UI
