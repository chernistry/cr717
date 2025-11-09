# Ticket 45: VST3 Voice Synthesis Alignment to TR-808 Spec

**Status:** Open  
**Priority:** High  
**Estimated Effort:** 3-5 days  
**Dependencies:** None (VST3 plugin already exists)  
**Related:** `.sdd/sound_specs.md`

---

## Objective

Align all 12 drum voice synthesis algorithms in the **VST3 plugin** (`native/vst3/CR717/Source/*Voice.h`) to the canonical TR-808 specifications documented in `.sdd/sound_specs.md`, using Roland service notes and CCRMA research as ground truth.

---

## Acceptance Criteria

### Bass Drum (BassDrumVoice.h)
- ✅ Bridged-T resonator at **56 Hz** nominal (currently 150 Hz)
- ✅ Pitch overshoot envelope: **10 ms** exponential drop
- ✅ Amplitude decay: **500 ms** default (100-1000 ms range)
- ✅ Tone LPF: **~1.5 kHz** cutoff
- ✅ Transient click injection (HP-filtered pulse)

### Snare Drum (SnareDrumVoice.h)
- ✅ Dual resonators: **180 Hz** + **330 Hz** (BPF)
- ✅ White noise: HP/BP **700-3 kHz** band
- ✅ Body decay: **250 ms**, noise decay: **200 ms**
- ✅ Snappy control: noise VCA depth (0-1)
- ✅ Tone control: HF attenuation

### Toms (TomVoice.h)
- ✅ Resonant BPF: **130 Hz / 200 Hz / 325 Hz**
- ✅ Pitch bend: **10-20 ms** downward
- ✅ Expo decay: **300 ms / 280 ms / 220 ms**
- ✅ Tune control: ±12 semitones

### Rimshot (PercussionVoice.h)
- ✅ BP center: **~2.5 kHz**
- ✅ Expo decay: **30 ms**
- ✅ Pulse/impulse excitation

### Handclap (PercussionVoice.h)
- ✅ Four noise bursts: **0 / 12 / 24 / 36 ms**
- ✅ Noise BP: **~1.5 kHz**
- ✅ Tail decay: **150 ms**
- ✅ Comb/reverb network for diffusion

### Closed Hi-Hat (HiHatVoice.h)
- ✅ Six square oscillators: **205.3, 304.4, 369.6, 522.7, 540, 800 Hz**
- ✅ Dual BPF: **3.44 kHz** + **7.10 kHz** (3rd-order)
- ✅ Expo decay: **190 ms**
- ✅ Choke logic: OH kills CH

### Open Hi-Hat (HiHatVoice.h)
- ✅ Same oscillators/filters as CH
- ✅ Expo decay: **490 ms**
- ✅ Choke logic: OH retrigger kills CH

### Crash Cymbal (CymbalVoice.h)
- ✅ Same oscillators/filters as hats
- ✅ Expo decay: **1.2 s**
- ✅ Additional HP for brightness

### Ride Cymbal (CymbalVoice.h)
- ✅ Same oscillators/filters as CY
- ✅ Expo decay: **1.9 s**

### Cowbell (PercussionVoice.h)
- ✅ Two square oscillators: **540 Hz** + **800 Hz**
- ✅ BP emphasis: **~1-2 kHz**
- ✅ Expo decay: **350 ms**

### Global Behaviors
- ✅ Accent bus: per-voice gain scaling (BD 0.6, SD 0.5, Hats 0.4)
- ✅ Hat choke: OH mutes CH (2-5 ms release)

---

## Implementation Steps

### Phase 1: Audit Current VST3 Voices (Day 1)
1. Review `BassDrumVoice.h` - currently uses 150 Hz, needs 56 Hz
2. Review `SnareDrumVoice.h` - check if dual resonators exist
3. Review `HiHatVoice.h` - verify oscillator count and frequencies
4. Review `CymbalVoice.h` - check decay times
5. Review `TomVoice.h` - verify frequencies (130/200/325 Hz)
6. Review `PercussionVoice.h` - check RS, CP, CB implementations

### Phase 2: Update Core Voices (Days 2-3)
7. **BassDrumVoice.h**: Change resonator from 150 Hz to 56 Hz, add pitch overshoot, add click injection
8. **SnareDrumVoice.h**: Implement dual-resonator (180/330 Hz), tune noise filters to 700-3kHz
9. **TomVoice.h**: Set exact frequencies (130/200/325 Hz), add pitch bend
10. **PercussionVoice.h**: 
    - RS: Tune BP to 2.5 kHz, shorten decay to 30 ms
    - CP: Implement 4-pulse generator (0/12/24/36 ms), add comb network
    - CB: Implement dual-square (540/800 Hz), tune BP to 1-2 kHz

### Phase 3: Metallic Voices (Day 4)
11. **HiHatVoice.h**: Implement 6-oscillator set (205.3, 304.4, 369.6, 522.7, 540, 800 Hz)
12. Add dual 3rd-order BPF (3.44 kHz / 7.10 kHz)
13. Set decay times: CH 190 ms, OH 490 ms
14. **CymbalVoice.h**: Same 6-osc + dual BPF, set decay times: CY 1.2 s, RD 1.9 s

### Phase 4: Global Features (Day 5)
15. Implement accent bus with per-voice sensitivity in PluginProcessor
16. Implement hat choke logic (OH kills CH)
17. Verify all parameter ranges match spec

### Phase 5: Testing & Calibration
18. Build VST3 plugin
19. Load in DAW and A/B test against reference 808 samples
20. Adjust Q factors and filter slopes for authenticity
21. Update factory presets to use new voice parameters

---

## Affected Files

### To Update (VST3 C++ Headers)
- `native/vst3/CR717/Source/BassDrumVoice.h` - Change 150 Hz → 56 Hz, add click
- `native/vst3/CR717/Source/SnareDrumVoice.h` - Dual resonators, noise filters
- `native/vst3/CR717/Source/TomVoice.h` - Tune to 130/200/325 Hz, add pitch bend
- `native/vst3/CR717/Source/PercussionVoice.h` - RS (2.5 kHz), CP (4-pulse), CB (540/800 Hz)
- `native/vst3/CR717/Source/HiHatVoice.h` - 6-osc + dual BPF, choke logic
- `native/vst3/CR717/Source/CymbalVoice.h` - Same as hats, longer decays
- `native/vst3/CR717/Source/PluginProcessor.h` - Accent bus logic
- `native/vst3/CR717/Source/PluginProcessor.cpp` - Choke implementation

---

## Tests

### Build Tests
```bash
cd native/vst3/CR717
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

### DAW Tests
- Load plugin in Ableton/Logic/Reaper
- Trigger each voice via MIDI
- Verify frequencies match spec (use spectrum analyzer)
- Verify decay times match spec (use oscilloscope)
- Test hat choke (OH should kill CH)
- Test accent (should increase gain per voice sensitivity)

---

## Risks & Mitigations

| Risk | Mitigation |
|------|------------|
| Oscillator aliasing at high frequencies | Use JUCE's band-limited oscillators (dsp::Oscillator) |
| Filter instability at extreme Q | Use JUCE's TPT filters (dsp::StateVariableTPTFilter) |
| Envelope clicks/pops | Add 1-2 ms fade-in/out to all ADSR envelopes |
| Performance regression | Profile before/after, optimize filter implementations |
| Sounds "too clean" vs analog | Add subtle pitch/amplitude jitter (±0.5 Hz, ±0.5 dB) |

---

## Success Metrics

- **Frequency accuracy**: All voices within ±1 Hz of spec
- **Envelope accuracy**: Decay times within ±5% of spec
- **Spectral match**: >85% FFT correlation with reference 808 samples
- **Performance**: No CPU regression vs current implementation
- **Subjective**: Passes blind A/B test with 808 users (>70% prefer new)

---

## References

- `.sdd/sound_specs.md` - Full TR-808 synthesis spec
- Roland TR-808 Service Notes - Calibration table
- CCRMA papers (Werner/Abel/Smith) - Cymbal, BD, CB models
- Electric Druid - 808 metallic voice overview

---

## Notes

- Prioritize **BD, SD, CH, OH** (most iconic sounds) for Phase 2
- Use JUCE's `dsp::Oscillator` for band-limited square waves
- Use JUCE's `dsp::StateVariableTPTFilter` for stable filters
- Document all parameter ranges in UI tooltips
- Update factory presets to match service-note calibration values
