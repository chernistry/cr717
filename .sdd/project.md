# TR-808 Drum Machine - Project Description

## Project Goal

Create a web application that emulates the classic Roland TR-808 drum machine using the Web Audio API.

## Requirements

### Core Sounds (minimum)
- Bass Drum (BD) - kick drum
- Snare Drum (SD) - snare drum
- Closed Hi-Hat (CH) - closed hi-hat
- Open Hi-Hat (OH) - open hi-hat
- Clap (CP) - clap

### Functionality
- **Sequencer**: 16 steps for each instrument
- **Playback**: Play/Stop/Reset
- **Tempo**: Adjustable BPM (60-240)
- **Patterns**: Ability to save/load patterns
- **Visualization**: Step highlighting during playback

### Technical Requirements
- **Frontend**: Vanilla JavaScript or React (at choice)
- **Audio**: Web Audio API for sound synthesis
- **UI**: Minimal interface reminiscent of the original TR-808
- **Responsive**: Works on desktop and tablet

## Limitations

- No backend (everything runs in the browser)
- No external audio libraries (Web Audio API only)
- Simple UI without complex animations
- Local pattern storage (localStorage)

## Acceptance Criteria

1. ✅ All 5 basic sounds are synthesized and sound similar to the original
2. ✅ Sequencer works for 16 steps
3. ✅ Can start/stop playback
4. ✅ Can change tempo (BPM)
5. ✅ Can save and load a pattern
6. ✅ Visual indication of the current step
7. ✅ Works in modern browsers (Chrome, Firefox, Safari)

## References

- Original Roland TR-808
- Web Audio API documentation
- Examples of drum sound synthesis

## Priorities

**P1 (Must have):**
- Bass Drum, Snare, Hi-Hat synthesis
- 16-step sequencer
- Play/Stop

**P2 (Should have):**
- Clap, Open Hi-Hat
- BPM control
- Visualization

**P3 (Nice to have):**
- Pattern save/load
- Additional sounds (Cowbell, Toms)
- Swing/shuffle

## Scope Estimation

- ~500-800 lines of code
- 1-2 weeks of development
- One developer
