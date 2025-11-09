# TR-808 Garage VST3

Native VST3 instrument plugin for TR-808 drum synthesis.

## Features

- **6 Synthesized 808 Voices**: BD, SD, CH, OH, CP, RS
- **21 Parameters**: Per-voice Level, Tune, Decay, Tone/Snappy controls
- **8 Factory Presets**: Pattern A-H (UK Garage, Deep House, Techno, etc.)
- **RT-Safe Processing**: Zero allocations in audio thread
- **Low Latency**: 0 samples reported latency
- **GUI Editor**: Dark theme with rotary knobs and faders
- **GM Drum Map Compatible**: Standard MIDI note mapping

## System Requirements

- **macOS**: 10.13+ (High Sierra or later)
- **CPU**: Intel or Apple Silicon (Universal Binary)
- **RAM**: 4 MB minimum
- **DAW**: Any VST3-compatible host (Ableton Live, Logic Pro, Reaper, etc.)

## Installation

### macOS

1. Download `TR808Garage-1.0.0-macos.vst3.zip`
2. Unzip the archive
3. Copy `TR-808 Garage.vst3` to:
   ```
   ~/Library/Audio/Plug-Ins/VST3/
   ```
4. Rescan plugins in your DAW

## MIDI Note Mapping

| Note | Name | Voice |
|------|------|-------|
| C1 (36) | Bass Drum | BD |
| C#1 (37) | Rim Shot | RS |
| D1 (38) | Snare Drum | SD |
| D#1 (39) | Hand Clap | CP |
| F#1 (42) | Closed Hi-Hat | CH |
| A#1 (46) | Open Hi-Hat | OH |

## Factory Presets

1. **Pattern A - UK Garage**: Balanced, punchy kick
2. **Pattern B - Deep House**: Lower kick, softer snare
3. **Pattern C - Techno**: Loud kick, aggressive
4. **Pattern D - Hip-Hop**: Deep kick, snappy snare
5. **Pattern E - Minimal**: Quiet kick, prominent hats
6. **Pattern F - Breakbeat**: Tuned snare, varied levels
7. **Pattern G - Ambient**: Long decays, spacious
8. **Pattern H - Aggressive**: Everything loud

## DAW Integration

### Ableton Live
1. Create MIDI track
2. Load from Instruments → Plug-ins → VST3
3. Select preset from "Program" dropdown
4. Trigger with MIDI notes C1-A#1

### Logic Pro
1. Create Software Instrument track
2. Load from VST3 instruments
3. Use preset menu for patterns
4. Play with MIDI keyboard

### Reaper
1. Insert → Virtual Instrument → VST3: TR-808 Garage
2. Program change for presets
3. MIDI item triggers drums

## Performance

- **CPU**: <3% at 44.1kHz/64 samples
- **Latency**: 0 samples
- **Memory**: ~4 MB
- **Tested**: 44.1k-96k, 64-1024 buffer sizes

## Troubleshooting

**Plugin not appearing**: Verify path `~/Library/Audio/Plug-Ins/VST3/`, rescan in DAW

**No sound**: Check MIDI routing, verify notes C1-A#1, check levels

**Clicks/pops**: Increase buffer size to 128 or 256

## Technical Specs

- **Format**: VST3
- **I/O**: Stereo out, MIDI in
- **Synthesis**: Analog modeling
- **Parameters**: 21 automatable
- **Version**: 1.0.0
- **JUCE**: 8.0.4

## License

MIT - See project root

---

**Build**: 2025-11-09 | **Validation**: pluginval ✓
