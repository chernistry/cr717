# Specification for **Cherni CR-717** UI Plugin (VST3/AU) — Full-Featured Drum Synth (2025)

> Goal: Provide a realistic, implementable specification for a premium VST3/AU UI using JUCE 7.x with clear tokens, dimensions, control behavior, production, and accessibility standards.

---

## 1) TL;DR

* Dark theme by default; AA contrast; targets ≥24×24 px. ([JUCE Documentation][1])
* Rigid modular grid 12-column + base 8 px; window 1100×620…1400×780; scale 0.75–1.25× with per-monitor DPI support. ([Melatonin][2])
* Resize and scale via VST3 `IPlugView`/`resizeView` + `IPlugViewContentScaleSupport`; JUCE receives scaleFactor in editor. ([Steinberg Media][3])
* OS-level drag-and-drop MIDI: `juce::DragAndDropContainer::performExternalDragDropOfFiles` with temporary `.mid`. ([JUCE Documentation][4])
* Presets: VST3 Program Lists + state via `getStateInformation()`/`setStateInformation()`; instant UI↔model sync. ([Steinberg Media][5])
* Performance: 60 fps goal; coalescing repaints; cache heavy renders `Component::setBufferedToImage`; no allocations in paint. ([JUCE][6])
* Master metering: True-Peak/LUFS per ITU-R BS.1770 + EBU Tech 3341/3342; update 30–60 Hz. ([JUCE][7])
* JUCE Accessibility: focus, tab-order, descriptions; Keyboard nav; contrast. ([JUCE Documentation][8])
* Host data: tempo/transport from VST3 `ProcessContext` for sync; UI doesn't store parameter duplicates. ([JUCE][9])
* Stability testing: **pluginval** + DAW/OS/DPI matrix. ([JUCE][10])

---

## 2) Context and Constraints

* **Engine**: JUCE 7.x; formats VST3 (mandatory), AU (optional).
* **DAW minimum**: Ableton Live, Reaper, Studio One.
* **Performance**: 16 ms frame budget; repaint by dirty rectangles; meters ≤60 Hz. ([JUCE][6])
* **HiDPI**: correct operation when moving between monitors with different scale; account for Windows Per-Monitor-V2 cases. ([Steinberg Media][11])

---

## 3) Design System (Tokens) and JUCE Mapping

### 3.1 Colors (HSL, dark default)

```json
{
  "color": {
    "bg/0": "hsl(0 0% 10%)",
    "bg/1": "hsl(0 0% 13%)",
    "bg/2": "hsl(0 0% 18%)",
    "panel": "hsl(0 0% 14%)",
    "text/hi": "hsl(0 0% 98%)",
    "text/lo": "hsl(0 0% 65%)",
    "accent/808": "hsl(18 100% 56%)",
    "accent/yellow": "hsl(44 100% 54%)",
    "accent/red": "hsl(2 88% 60%)",
    "accent/cyan": "hsl(188 80% 54%)",
    "ok": "hsl(135 70% 52%)",
    "warn": "hsl(32 95% 56%)",
    "err": "hsl(0 80% 58%)",
    "border": "hsl(0 0% 28%)",
    "meter/peak": "hsl(0 84% 56%)",
    "meter/rms": "hsl(160 70% 48%)"
  }
}
```

* Text contrast ≥4.5:1 (AA). ([JUCE Documentation][1])
* **JUCE**: store in `juce::Colour` from sRGB; generate tokens in C++ header at build time.

### 3.2 Typography and Sizes

* Font: system sans (macOS SF-Pro, Win Segoe UI).
* Sizes: 11/12/14/16/20 pt for UI; line height 1.2–1.35.
* Base intervals: 8 px; radii 4/8 px; shadows very subtle.

### 3.3 Grid and Minimum Dimensions

* Window: **1100×620** (min), **1200×680** (recommended), **1400×780** (max without tabs).
* Columns: 12; margins 16 px; gutter 12 px.
* Scale: 0.75–1.25×, per-monitor scale-factor change when moving window. Implementation — see §10. ([Steinberg Media][11])

---

## 4) Information Architecture and Layouts

### 4.1 Top Panel **Transport & Global**

* **Elements**: Play/Stop, Host Sync indicator, BPM (host/local), Swing, Preset Browser, Bank A–H, Pattern ops (Copy/Paste/Clear), MIDI Drag.
* **Height**: 72 px.
* **Behavior**:

  * If host provides tempo/position (`ProcessContext`), toggle **Host** is active; otherwise — internal BPM. ([JUCE][9])
  * **Drag MIDI** button initiates OS-DND temporary `.mid`. ([JUCE Documentation][4])

### 4.2 Central Region (two tabs)

* **KIT**: 12 voice modules in 3×4; each — Knobs/Envs/Sends/Mixer.
* **SEQ** (optional in v1): 16-step grid with header 1–16 and groups of 4; accent, choke-groups, pattern length.

### 4.3 Right Column **Mixer/FX**

* Per-voice `Level`, `Pan`, `Out` (stereo/multi-out), `Snd A/B`, M/S/Solo; master meters TP/RMS/LUFS; minimalist Comp/Limiter.

### 4.4 Bottom Panel **Status/Help**

* Modifier hints (Shift=Fine, Alt=Coarse, Ctrl=Reset), sync state, CPU UI %, highlight problems.

---

## 5) Components: Geometry, States, Gestures

### 5.1 Button (Primary/Toggle/Segmented)

* Sizes: **Primary** 28×28/28×96; **Toggle** 24×24; **Segmented** height 28.
* States: default/hover/pressed/disabled; focus-ring 2 px.
* Hotkeys: Space/Enter activates focused control.

### 5.2 Knob

* Diameter **32 px** (small), **40 px** (medium), **56 px** (large).
* Travel angle: 300°. Increment: 0.5–1.0% per 10 px, Shift=×0.2, Alt=×3.
* Double-click — reset to default; right-click — context with value entry.
* Drawing with cache (`setBufferedToImage(true)`) on idle/resize, not in real-time. ([JUCE][6])

### 5.3 Fader (vert/hor.)

* Width 14–18 px; minimum height 120 px.
* Fine divisions every 1 dB/unit; large labels step 6 dB.

### 5.4 Pad/Step (Sequencer Pad)

* Square **28–32 px** at 1200 px window width; shrink to 24 px at 1100 px.
* States: off / on / accent / current-step; current-step highlighted without flickering; update at 60 fps.
* Multi-editing: drag-paint, Alt=erase, double-click=accent.

### 5.5 Meter (Master/Voice)

* Modes: True-Peak and RMS/LUFS; ballistics per EBU Tech 3341; scales -60…0 dBFS, TP warning -1 dBTP. ([Tech EBU][12])
* Update frequency: 30–60 Hz; without blocking allocations. ([JUCE][6])

### 5.6 Preset Browser

* Left column: Styles (UKG/House/Techno/…); center: list with previews; right: description/tags.
* Actions: Load/Save/Rename/Duplicate/Star.
* VST3 ProgramLists integrated so host can display list. ([Steinberg Media][5])

---

## 6) Voice Modules (12 units) — Parameters and UI

Each module: header (color code), **3–5 knobs**, toggles, Sends A/B, M/S/Solo.

* **BD**: Level, Tone, Decay, Tune, Click; FineTune, Pan; Send A/B.
* **SD**: Level, Tone, Snappy, Tune; FineTune, Pan.
* **LT/MT/HT**: Level, Decay, Tune; FineTune, Pan.
* **RS/CP/CB**: Level (+Tone for CP), Pan.
* **CH/OH**: Level, Decay; Choke group (OH chokes CH).
* **CY/RD**: Level, Tone, Decay; Pan.

Module grid layout: card **(W 260 × H 160)** at 1200 px width; adaptation by grid 3×4/2×6.

---

## 7) Step Sequencer (tab **SEQ**)

* **Rows**: 12; **columns**: 16 with groups of 4.
* **Features**: pattern length, swing (glob.), copy row/clear row, random/humanize (style-aware).
* **Navigation**: arrows move focus; Enter/Space — toggle; Shift+arrow — range.
* **Synchronization**: visual position tied to host audio-time (ProcessContext); no timers, render in UI timer 60 Hz. ([JUCE][9])

---

## 8) Global Elements

* **Transport**: Play/Stop; **EXT** indicator when host sync active.
* **BPM**: number + encoder; if host active — field read-only. ([JUCE][9])
* **Swing**: 0–75% with 1% step; tool-tip with value.
* **Pattern Bank A–H**: segmented buttons; Copy/Paste/Clear; holding Alt — duplication.
* **MIDI Drag**: button with pictogram; when held — creates temp `.mid`, starts `performExternalDragDropOfFiles`. Fallback — Save MIDI/Reveal. ([JUCE Documentation][4])

---

## 9) Accessibility and UX Norms

* Minimum targets **≥24×24 px**, visible focus; contrast **AA**. ([JUCE Documentation][1])
* JUCE Accessibility: role/name/description, orientation, value descriptors; keyboard support via Focus Traversal/KeyListener. ([JUCE Documentation][8])
* Text/icons — @2x/@3x assets for retina/HiDPI. ([Apple Developer][13])

---

## 10) Scaling, DPI and Resize

* **VST3**: implement `IPlugView::resizeView` and support `IPlugViewContentScaleSupport` (`setContentScaleFactor`, `getContentScaleFactor`). ([Steinberg Media][3])
* **JUCE**: respect `AudioProcessorEditor::setScaleFactor`; use `getApproximateScaleFactorForComponent()` at `resized()`. Account for call order in some hosts (editor creation before `setState`). ([JUCE][14])
* **Windows**: Per-Monitor-V2 DPI; check after dragging between displays. ([Steinberg Media][11])
* **macOS**: retina assets @2x/@3x. ([Apple Developer][15])

---

## 11) UI Performance

* **Repaints**: only dirty regions; consolidate; cache heavy elements `setBufferedToImage(true)`. ([JUCE][6])
* **Timers**: meters 30–60 Hz; grid — 60 Hz; avoid dynamic allocation in `paint()`. ([JUCE][6])
* **DPI Fault Tolerance**: test cases from known hosts/environments. ([JUCE][16])

---

## 12) Presets, State and Host

* **State**: everything affecting sound/pattern stored in processor state; UI-only settings — in controller/own. In JUCE — via `getStateInformation()`/`setStateInformation()`. ([JUCE Documentation][17])
* **Program Lists**: expose lists for host (preset reflection in Browser zone). ([Steinberg Media][5])
* **Call Order**: account for hosts where `getState → createEditor → setState`. ([JUCE][18])

---

## 13) Palette and Visual Language

* Inspired by TR-808: warm accents (orange/yellow/red) + cool auxiliary (cyan); minimal "skeuomorphism".
* Low visual noise; flat surfaces; thin borders; step highlight — soft glow 1.5–2 px.

---

## 14) Size Specification (Key Elements)

| Element                 | Sizes                            |
| ----------------------- | -------------------------------- |
| Top panel               | H 72 px                          |
| Voice card              | W 260 × H 160 px (at 1200 width) |
| Knob small/med/large    | Ø 32 / 40 / 56 px                |
| Fader                   | W 14–18 px, H ≥ 120 px           |
| Step pad (SEQ)          | 24–32 px (adaptive)              |
| Master meter            | W 10–14 px, H 120–160 px         |

---

## 15) Micro-interactions and Help

* Tooltip with range/unit hint; `Cmd/Ctrl`-click — enter precise value.
* Reset to default — double-click or `Alt`-click.
* Mouse wheel — adjust parameter; `Shift` — fine step.
* Status bar reports sync/errors drag-and-drop/meter overload.

---

## 16) Audio Metrology (UI norms)

* Display modes: Short-term LUFS, Integrated LUFS, True-Peak; LRA (per 3342) in presets browser/analytics. ([Tech EBU][19])
* Markers: 0/-1/-6/-12/-18/-24/-48 dBFS; calibration -18 LUFS ref. ([rosseladvertising.be][20])

---

## 17) Test Matrix and Validation

* **Tool**: `pluginval` on macOS/Windows. ([JUCE][10])
* **DAW/OS/DPI**:

  * Windows 10/11, 100%/150%/200%, single- and multi-monitor cases; moving window between displays. ([Steinberg Media][11])
  * macOS 13–15, Retina/non-Retina; assets @2x/@3x. ([Apple Developer][15])
* **Preset stability**: check save/restore project state; quick preset changes and immediate UI update. ([JUCE Documentation][17])
* **Drag-and-Drop**: test OS-DND .mid file in Live/Reaper/Studio One. ([JUCE Documentation][4])

---

## 18) Implementation (JUCE/VST3) — Brief Plan

1. **Tokens** generate in C++ (namespace `ui::tokens`), map to `juce::Colour`, sizes to constexpr.
2. **Grid** — base container with 12-column layout; modularity via nested `FlexBox`/`Grid`.
3. **Scale**: accept `setScaleFactor`; at `resized()` recalculate geometry, rasterize SVG considering `getApproximateScaleFactorForComponent()`. ([JUCE][14])
4. **DPI**: listen to monitor changes and adjust scale (Per-Monitor-V2). ([Steinberg Media][11])
5. **PresetManager**: connect to APVTS/model; at preset load — push pattern to SequencerModel before `createEditor` and after `setState`. ([JUCE][18])
6. **MIDI Drag**: generate `.mid` → `performExternalDragDropOfFiles` + Save/Reveal buttons. ([JUCE Documentation][4])
7. **Meters**: timer 30–60 Hz; ballistics and scales per EBU/ITU. ([Tech EBU][12])
8. **Optimization**: caches for knobs/background; dirty regions; no allocations in `paint()`. ([JUCE][6])

---

## 19) Screen Set (Layouts for Figma/implementation)

* **Main / KIT** 1200×680: top panel + 12 modules + right mixer/FX + bottom status.
* **SEQ** 1200×680: top panel + step grid across entire central area + right mixer/FX.
* **Preset Browser**: overlay 960×560, navigation keys ↑/↓, Enter — Load, `Cmd+S` — Save.
* **Compact** 1100×620: modular 2×6 voice layout, narrow mixer.

---

## 20) Anti-patterns

* Timer-based step rendering without host-time/ProcessContext accounting. ([JUCE][9])
* Internal Drag&Drop instead of OS-DND for MIDI output from plugin window. ([JUCE Documentation][4])
* Unclear DPI strategy; no response to monitor change. ([Steinberg Media][11])
* Frequent full repaints; allocations in `paint()`; lack of caches. ([JUCE][6])

---

## 21) Delivery Artifacts

* JSON tokens + C++ header generation.
* SVG/PNG asset set @1x/@2x/@3x. ([Apple Developer][13])
* Test checklist (pluginval, DPI matrix, drag MIDI, presets). ([JUCE][10])

---

## 22) Compatibility Notes

* Scale/order nuances in specific hosts (FL, Max, Renoise) to account for — known cases. ([JUCE][18])

---

### References (Key)

* VST3 `IPlugView`/resize, Content Scale Support. ([Steinberg Media][3])
* JUCE Accessibility, DnD, drawing optimization, component focus. ([JUCE Documentation][8])
* Per-Monitor DPI v2 (Windows). ([Steinberg Media][11])
* Apple HIG: accessibility/retina/contrast. ([Apple Developer][21])
* EBU/ITU loudness/true-peak (meters). ([Tech EBU][19])
* VST3 Presets/Program Lists, JUCE state.
* pluginval.