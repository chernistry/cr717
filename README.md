# TR-808 Synth

![TR-808 Synth Screenshot](./assets/screenshot.png)

A professional-grade drum sequencer created with AI in about 30 minutes, available both as a web-based application and a full-featured VST3 plugin. Inspired by the classic TR-808 but not pretending to replicate the original. A modern VST-style interface with comprehensive functionality. Use it freely in your projects!

This project demonstrates the SDD (Spec-Driven Development) approach - a methodology for rapid development using AI agents with structured prompts and templates. For more details on SDD, check out [SDDRush](https://github.com/chernistry/SDDRush).

All prompts used in this project are available in both locations - the SDDRush repo and the sdd directory within this project.

## Features

### Web Application
- **12 synthesized drum voices** (BD, SD, LT, MT, HT, RS, CP, CH, OH, CY, RD, CB) with Web Audio API synthesis
- **16-step sequencer grid** for each voice with visual step highlighting
- **Professional VST-style UI** with dark theme and gradient panels
- **Individual voice mixer** with level controls for each instrument
- **Parameter controls** for per-voice customization (level, tone, decay, tuning, etc.)
- **Default atmospheric UK garage pattern** pre-loaded
- **Adjustable tempo** (60-240 BPM) with swing control
- **Pattern saving/loading** via browser storage
- **Real-time step highlighting** during playback
- **Theme toggle** (light/dark mode)
- **Keyboard shortcuts** support for efficient operation
- **Responsive design** optimized for desktop and tablet

### VST3 Plugin
- **Full-featured VST3 instrument** compatible with DAWs like Ableton Live, Logic Pro, Reaper
- **12 synthesized drum voices** matching the web version
- **Parameter automation support** for all voice parameters
- **MIDI input** support for triggering sounds
- **Host sync** with tempo and playback state
- **Factory presets** (8 pattern banks A-H with different genres)
- **Individual voice controls** with custom knobs and faders
- **Level meters** for visual feedback
- **Professional dark theme** matching the web UI
- **Low-latency audio** processing with optimized DSP

## How to Use

### Web Application
```bash
# Install dependencies
npm install

# Start development server
npm run dev

# Build for production
npm run build
```

### VST3 Plugin Installation
```bash
# Build the plugin (requires CMake and JUCE)
cd native/vst3/TR808Garage
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Copy the resulting .vst3 bundle to your DAW's plugin directory:
# macOS: ~/Library/Audio/Plug-Ins/VST3/
# Windows: Program Files\Common Files\VST3\
# Linux: ~/.vst3/
```

### Web Application Keyboard Shortcuts:
- Space: Play/Stop
- +/-: Increase/Decrease BPM by 5
- T: Toggle theme (light/dark)
- D: Load default pattern
- ?: Show help
- Escape: Close help
- 1-5: Select voice
- Enter: Toggle step
- Arrow keys: Navigate steps

### VST3 Plugin Controls:
- **Pattern Bank**: Switch between 8 preset patterns (A-H)
- **MIDI Input**: Trigger sounds via connected MIDI keyboard/controller
- **DAW Transport**: Sync with host DAW playback and tempo
- **Parameter Automation**: All parameters support DAW automation
- **Individual Voice Controls**: Custom knobs for level, tune, decay, tone per voice

## Tech Stack

### Web Application
- **Frontend**: TypeScript
- **Audio**: Web Audio API with look-ahead scheduler
- **Build**: Vite 6
- **Testing**: Vitest 4 + Playwright
- **Linting**: ESLint + Prettier

### Native VST3 Plugin
- **Framework**: JUCE 7
- **Build**: CMake
- **Audio**: Steinberg VST3 SDK
- **UI**: JUCE native components with custom drawing
- **DSP**: Optimized C++ synthesis algorithms

## Architecture

### Web Application
- **State Management**: Centralized store with pub/sub pattern
- **Audio Engine**: Look-ahead scheduler with precise timing (<2ms jitter)
- **UI Components**: Modular, reusable components (sequencer grid, mixer, controls)
- **Design System**: CSS variables for consistent theming and styling

### VST3 Plugin
- **Audio Processing**: Real-time safe DSP with block processing
- **Parameter System**: APVTS for automation-compatible parameters
- **MIDI Handling**: Full MIDI input/output support
- **Host Integration**: Tempo sync, playback state, sample-accurate timing

## Performance

### Web Application
- Bundle size: <10 KB (3.13 KB gzipped)
- Timing accuracy: <2ms jitter at 120 BPM
- Memory usage: <100 MB
- No long tasks during playback

### VST3 Plugin
- Low-latency audio processing
- Optimized for real-time performance
- Sample-accurate timing
- Efficient CPU usage

## Building and Development

### Prerequisites (VST3 Plugin)
- **CMake** (3.22 or later)
- **JUCE** (included as submodule)
- **Steinberg VST3 SDK** (for validation)
- **Build tools**: Xcode/macOS, Visual Studio/Windows, or GCC/Linux

### Development Workflow
1. Clone the repository with submodules: `git clone --recursive`
2. For web app: use standard npm workflow
3. For VST3: use CMake to build native plugin from `native/vst3/TR808Garage/`

## License

MIT
