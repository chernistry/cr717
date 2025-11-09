Looking at the file `best_practices_vst.md`, I can see it's mostly in English already, but there are several Russian sections that need translation. Since you're in ask mode, I'll provide the fully translated version for you to copy and apply. Here's the complete file with all Russian text translated to English:

```1:227:tr808-synth/.sdd/best_practices_vst.md
# Best Practices Guide — VST/AU instrument in 2025 (for project cr717)

## 1. TL;DR

* Default format: **VST3 + AU (macOS)**. VST2 — EOL. VST3 SDK — **MIT license** → easier distribution. Add **CLAP** as optional. ([Ableton Help][1])
* Framework: **JUCE 7/8** or **iPlug2**. JUCE — fast start and ready modules DSP/GUI; iPlug2 — lightweight, flexible. ([JUCE][2])
* Real-time: in audio-stream **no allocations/locks/IO**; suppress denormals (**ScopedNoDenormals**). ([docs.juce.com][3])
* Automation: **sample-accurate** in VST3; parameter-smoothing (**SmoothedValue**). ([GitHub][4])
* Distribution macOS: **code sign + hardened runtime + notarization**; Universal2 (arm64+x86_64). ([Moonbase][5])
* Installation paths: **VST3 →** `C:\Program Files\Common Files\VST3`, **AU →** `/Library/Audio/Plug-Ins/Components`. ([Steinberg Help Center][6])
* DAW compatibility: Ableton Live (macOS) = **VST3 and AU**, Windows = **VST3**. Do not count on VST2. ([JUCE][7])
* Testing: **pluginval** in CI + **auval/Logic** on macOS. ([GitHub][8])
* Performance: budget for audio-block without "jitter"; avoid denormals, aliasing → **Oversampling**. ([docs.juce.com][9])
* Security/Supply chain: SLSA-levels, actual **OWASP Top 10 (2025 RC1)** focus on misconfiguration; signature/verification of artifacts. ([SLSA][10])

---

## 2. Landscape — what new in 2025

* **VST3 SDK → MIT**: simplified legal risks and integrations. ([Ableton Help][1])
* **JUCE**: active support OS/DAW and plan **JUCE 8**; mature CMake-build. ([JUCE][11])
* **CLAP** gained critical mass: Bitwig, u-he, FL Studio 24, Reaper and others; open MIT license; multithreading, MIDI 2.0-friendly. Ableton Live still without CLAP. ([u-he][12])
* **MIDI 2.0/UMP**: CoreMIDI supports Universal MIDI Packet; lay adapter. ([Apple Developer][13])
* **macOS distribution**: mandatory hardened runtime + notarization (`notarytool`). ([JUCE][14])
* **Security of supply chain**: SLSA/provenance of builds became norm. ([SLSA][10])

---

## 3. Architectural patterns

### Pattern A — **C++ plugin (MVP): JUCE VST3 + AU**

**When**: fast entry to Ableton/Logic/Reaper; minimal risk.
**Steps**: JUCE Audio Plug-In → CMake → modules `juce_audio_processors`, `juce_dsp`; target formats VST3/AU.
**Pluses**: single code base, ready DSP-utilities, SmoothedValue/Oversampling, cross-DAW. **Minuses**: commercial license for commercial releases. ([JUCE][2])
**Options later**: CLAP through clap-juce-extensions; MIDI 2.0; AAX. ([u-he][12])

### Pattern B — **Lightweight frame (Scale-up): iPlug2 VST3/AU/CLAP**

**When**: need lightweight runtime, fine tuning GUI/concurrency, WebAssembly-branch.
**Migration from A**: transfer DSP core and parameters, rewrite GUI layer. ([GitHub][15])

---

## 4. Priority 1 — Real-time audio and correctness DSP

**Why**: stable audio-stream without drops — key goal.
**Scope**: processing, parameters, automation, denormals, anti-aliasing.
**Solutions**:

* No allocations, lock-free, without IO/logs in audio-stream.
* Parameter smoothing: `juce::SmoothedValue` for all managed parameters, especially frequencies/gains. ([JUCE][16])
* Sample-accurate automation in VST3: process event-list with offsets. ([GitHub][4])
* Denormals: `juce::ScopedNoDenormals` in `processBlock`. ([docs.juce.com][3])
* Anti-aliasing nonlinearities: `juce::dsp::Oversampling` (choice FIR/IIR by latency/phase). ([docs.juce.com][9])
  **Implementation (steps)**:

1. Add `ScopedNoDenormals guard;` in beginning `processBlock`. ([docs.juce.com][3])
2. For each `AudioParameterFloat` start `SmoothedValue`, update in `prepareToPlay` and tick in cycle samples/blocks. ([JUCE][16])
3. VST3: iterate `IParameterChanges` and apply changes with sample offset. ([GitHub][4])
4. For saturation/drive — wrap nonlinearity in Oversampling x2–x4; set `setLatencySamples` by `getLatencyInSamples()`. ([docs.juce.com][9])
   **Guardrails & SLOs**:

* XRuns = 0 at 48kHz/64-sample buffer; deviation of block time ≤25% p95.
* Audio-stream without allocations; target CPU <10% on 8-voice pattern.
  **Failure modes → recovery**:
* CPU peaks at silence → enable denormal flush; check IIR-branches. ([JUCE][17])
* Clicks at automation → increase smoothing time or switch to sample-accurate path. ([JUCE][18])

---

## 5. Priority 2 — Distribution and DAW compatibility

**Why**: simple install, minimum support tickets.
**Scope**: formats, paths, signing, notarization.
**Solutions**:

* Formats: VST3 for Windows/macOS; AU for macOS; CLAP optional. Ableton Live: VST3/AU on macOS, VST3 on Windows. ([JUCE][7])
* Installation paths: `C:\Program Files\Common Files\VST3`; `/Library/Audio/Plug-Ins/Components`. ([Steinberg Help Center][6])
* macOS: Developer ID, hardened runtime, notarization (`xcrun notarytool`). ([JUCE][14])
  **Steps**:

1. Sign:

```bash
codesign --deep --force --timestamp --options runtime \
  --sign "Developer ID Application: Your Co" "My808.vst3"
```

2. Package in `.pkg` and send for notarization:

```bash
xcrun notarytool submit My808.pkg --keychain-profile "AC_PROFILE" --wait
```

3. Check: Logic/Live see the plugin; AU passes `auval`. ([JUCE][14])
   **SLOs**: installation <1 min; plugin appears in DAW after first scan.

---

## 6. Priority 3 — MIDI 2.0 and extensions

**Why**: long-term compatibility and expressiveness.
**Solutions**:

* Design the event layer so that it's easy to add **MIDI 2.0/UMP**. On macOS CoreMIDI already supports UMP. ([Apple Developer][13])
* Consider **CLAP** for per-note modulation and better multithreading. ([u-he][12])
  **Steps**: abstract MIDI as "note-events + per-note params", without hard-binding to MIDI 1.0.

---

## 7. Testing strategy

* **Unit**: DSP-core separate from host-API; tests for filters/envelopes.
* **Integration**: host-sim through **pluginval** headless. ([GitHub][8])
* **E2E**: loading in AudioPluginHost/Logic/Live; presets/automation. ([JUCE][19])
* **Performance**: profiling per blocks; test "quiet tail" for denormals.
* **Security**: SBOM, dependency scan; verification of artifact signatures.

---

## 8. Observability & Ops

* **Metrics**: block processing time p50/p95, number of parameter-events, UI exception reports.
* **Logs**: only in debug, outside audio-stream.
* **Alerts**: crashes during host scan, notarization failure.

---

## 9. Security Best Practices

* **Signature/notarization** for macOS; code-signing on Windows. ([JUCE][14])
* **SLSA**: at least L1–L2 (build provenance, reproducibility). ([SLSA][10])
* **OWASP Top 10 2025 RC1**: focus on Security Misconfiguration — store paths/access in release-build config, avoid network calls without necessity. ([OWASP Foundation][20])
* **Dependencies**: fixed versions JUCE/iPlug2/VST3 SDK; reproducible builds.

---

## 10. Performance & Cost

* **Budgets**: at 48 kHz / 64 samples — p95 block processing ≤0.5× buffer; CPU <10% on typical machine.
* **Optimization**: SIMD (SSE/NEON) through JUCE dsp; avoid denormals; reasonable Oversampling (2–4×). ([docs.juce.com][9])
* **Profiling**: stress-automation, "silence tail".

---

## 11. CI/CD Pipeline (minimum)

* **Build**: GitHub Actions `macos-14`, `windows-latest`; CMake.
* **Validation**: `pluginval --strictness-level 8` on built binaries. ([GitHub][8])
* **Signature/Notarization**: codesign/notarytool steps for macOS. ([JUCE][14])
* **Releases**: artifacts `.vst3` + `.component` + installers.
* Reference for CI for audio plugins. ([Moonbase][5])

---

## 12. Code Quality Standards

* **Style**: modern C++17/20, RAII, no new/delete.
* **Types**: strict `enum class`, `span`, `gsl` if desired.
* **Documentation**: Doxygen for DSP core.
* **Refactoring**: GUI and DSP separated; parameters — in one layer.

---

## 13. Reading List (dates and essence)

* Steinberg **VST3 Dev Portal** — parameters/automation, sample-accurate. (updating) — basics of working with parameters and events. ([Steinberg Media][21])
* Steinberg **VST3 SDK (MIT)** — sources and examples. (current 2025) — plugin examples. ([GitHub][4])
* **Apple**: Prepare for distribution / Hardened runtime / Notarization. (live doc) — macOS requirements. ([JUCE][14])
* **JUCE Docs**: Oversampling, ScopedNoDenormals, dsp. (current) — ready classes. ([docs.juce.com][9])
* **pluginval** — CLI-validation of plugins, CI. (current) — format/host tests. ([GitHub][8])
* **CLAP**: overview/benefits, MIT license. (u-he) — when to add CLAP. ([u-he][12])
* **CoreMIDI UMP/MIDI 2.0** — foundation for future MIDI. (Apple) ([Apple Developer][13])

---

## 14. Decision Log (ADR)

* **ADR-001**: *VST3+AU as baseline; CLAP — optional* instead of "only VST3", because Live/Logic are covered immediately, and CLAP gives advantages in Bitwig/Reaper. ([JUCE][7])
* **ADR-002**: *JUCE for MVP* instead of iPlug2, because SmoothedValue/Oversampling and fast start are needed; iPlug2 — as alternative at scale. ([docs.juce.com][9])
* **ADR-003**: *Mandatory notarization macOS* due to Gatekeeper requirements. ([JUCE][14])

---

## 15. Anti-patterns

* **Allocations/locks in audio-stream** → jitter/drops. Do prealloc and lock-free queues.
* **Lack of parameter smoothing** → clicks. Use SmoothedValue. ([JUCE][16])
* **Ignoring denormals** → CPU-peaks on silence. Enable ScopedNoDenormals. ([docs.juce.com][3])
* **Raw nonlinear DSP without oversampling** → aliasing. Apply x2–x4 Oversampling. ([docs.juce.com][9])
* **Incorrect installation paths** → DAW doesn't see plugin. Use standard directories. ([Steinberg Help Center][6])

---

## 16. Evidence & Citations

Key claims and practices confirmed: VST3 MIT, Dev Portal and SDK; Apple notarization/paths; Ableton formats; CLAP overview; MIDI 2.0/UMP; real-time rules; denormals; oversampling; pluginval. See links in text. ([Ableton Help][1])

---

## 17. Verification

* **Smoke script (macOS)**: build VST3/AU, `codesign` + `notarytool`, copy to standard paths, open Logic/Live, pass `auval`, run `pluginval --strictness-level 8`. ([JUCE][14])
* **Benchmark**: 48k/64 samples — measure p95 block time in DAW and on host-simulation; test "fade-to-silence" for denormals. ([JUCE][17])
* **Confidence**: Architecture/real-time — High; Distribution/signing — High; MIDI 2.0/CLAP — Medium (depends on hosts).

---

### Appendix — Quick start (JUCE, CMake, VST3+AU)

**Building formats**: include in CMake `JUCE_BUILD_VST3_PLUGIN TRUE`, `JUCE_BUILD_AU_PLUGIN TRUE`. JUCE docs/roadmap confirm CMake/format relevance. ([JUCE][11])

**Install-paths**:

* Windows: copy `.vst3` to `C:\Program Files\Common Files\VST3`. ([Steinberg Help Center][6])
* macOS: `.vst3` → `/Library/Audio/Plug-Ins/VST3`, `.component` → `/Library/Audio/Plug-Ins/Components`. ([Apple Support][22])

**Automation verification**: process `IParameterChanges` for sample-accurate; smoothing — `SmoothedValue`. ([GitHub][4])

**Oversampling**: `dsp::Oversampling<float>(channels, stages, FIR|IIR)`; report latency to host. ([docs.juce.com][9])
