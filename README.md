# TR-808 Synth

![TR-808 Synth Screenshot](./assets/screenshot.png)

A simple but functional drum sequencer created with AI in about 30 minutes, available both as a web-based application and a full-featured VST3 plugin. Inspired by the classic TR-808 but not pretending to replicate the original. A modern VST-style interface with comprehensive functionality. Use it freely in your projects!

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
- **97 parameters** with full automation support (73 voice + 24 FX sends)
- **36 factory presets** across 6 genres (UK Garage, DnB, Techno, House, Trap, Lo-Fi)
- **Per-voice FX sends** to reverb and delay buses
- **Master dynamics chain** (compressor, limiter, soft clipper)
- **MIDI input** support (GM drum map compatible)
- **Host sync** with tempo and playback state
- **Resizable UI** (1100x720 - 1650x1080) with HiDPI support
- **Accessibility** features (keyboard navigation, screen reader support)
- **Professional dark theme** with custom knobs and faders
- **Low-latency audio** processing with optimized DSP
- **Follows VST3 UI Best Practices 2025** (APVTS, throttled repaints, no OpenGL)

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

## Tech Stack, Architecture, and Performance

### Tech Stack
**Web Application**: TypeScript frontend with Web Audio API, built with Vite 6, tested with Vitest 4 + Playwright, linted with ESLint + Prettier. **Native VST3 Plugin**: JUCE 7 framework with CMake build system, Steinberg VST3 SDK for audio, optimized C++ synthesis algorithms.

### Architecture and Performance
The web app features centralized state management, precise audio timing with <2ms jitter, modular UI components, and CSS-based design system. The VST3 plugin offers real-time safe DSP, APVTS parameter system for automation, full MIDI support, and sample-accurate host integration. Both platforms deliver efficient performance: the web app has <10 KB bundle size with <2ms timing accuracy, while the VST3 plugin provides low-latency processing with optimized CPU usage. The plugin requires CMake 3.22+ and JUCE to build, supporting cross-platform development via submodules.

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
