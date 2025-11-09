# TR-808 Garage VST3

Native VST3 instrument plugin for TR-808 drum synthesis.

## Prerequisites

- **macOS**: Xcode 14+ with Command Line Tools
- **CMake**: 3.22+
- **VST3 SDK**: Optional (JUCE includes VST3 support)

## Build

```bash
# Configure
cmake -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release

# Output: build/TR808Garage_artefacts/Release/VST3/TR-808 Garage.vst3
```

## Install

```bash
# Copy to system VST3 directory
cp -r "build/TR808Garage_artefacts/Release/VST3/TR-808 Garage.vst3" \
      ~/Library/Audio/Plug-Ins/VST3/
```

## Validate

```bash
# Using VST3PluginTestHost (from VST3 SDK)
./scripts/validate_vst3.sh

# Or manually with pluginval
pluginval --strictness-level 8 --validate "TR-808 Garage.vst3"
```

## Development

```bash
# Debug build
cmake -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug

# Clean
rm -rf build
```

## Testing in DAW

1. Copy .vst3 to `~/Library/Audio/Plug-Ins/VST3/`
2. Rescan plugins in your DAW (Ableton Live, Logic Pro, Reaper)
3. Load as instrument track
4. Verify: stereo output, MIDI input, 0 latency

## CI

GitHub Actions builds Release artifacts for macOS (Universal Binary).
See `.github/workflows/build-vst3.yml`
