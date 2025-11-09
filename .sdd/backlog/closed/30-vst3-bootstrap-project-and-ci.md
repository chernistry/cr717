Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 Plugin Bootstrap (JUCE + VST3 SDK) with Local Build and CI

Objective
- Create a minimal, production-ready VST3 instrument plugin project “TR‑808 Garage” with the best path to sound quality, low latency, and manageable complexity. Use JUCE 7 + Steinberg VST3 SDK; CMake build; macOS first.

Why this approach
- JUCE is the most battle-tested for VST3/AU with strong RT‑safe defaults and good UI. It provides fast time‑to‑sound with high quality and low latency. We avoid web/Node bridges for the plugin; native C++ yields better timing determinism.

Scope (In/Out)
- In: JUCE app skeleton, VST3 target, CMake, local build on macOS, VST3 validator/host test, basic AudioProcessor with silence output, CI pipeline artifact.
- Out: DSP voices, GUI, presets, AU build (handled by later tickets).

Acceptance Criteria (DoD)
- native/vst3/CR717/ builds a .vst3 bundle on macOS (Debug/Release).
- Loads and passes smoke test in VST3PluginTestHost (no crashes), reports 0 latency, stereo output.
- VST3Validator passes with no critical errors.
- CI job produces downloadable .vst3 artifact for macOS.

Steps
1) Repo layout
   - Create `native/vst3/CR717/` with CMakeLists.txt and JUCE submodule or fetch-content.
   - Add README (build prerequisites: Xcode, CMake, JUCE, VST3 SDK path).
2) SDKs
   - Install JUCE 7 (submodule or FetchContent).
   - Install Steinberg VST3 SDK; set `VST3_SDK_DIR` and configure CMake cache doc.
3) Project skeleton
   - JUCE CMake: `juce_add_plugin(CR717 ...)` (VST3 only); instrument, stereo I/O, no latency, MIDI in.
   - Implement `AudioProcessor::processBlock` passthrough silence (no DSP yet).
4) Local runbook
   - Build commands, path to resulting `.vst3`, instructions for `VST3PluginTestHost`.
   - Add `scripts/validate_vst3.sh` to run validator.
5) CI (GitHub Actions or local CI)
   - macOS 13/14 runners; cache JUCE and VST3 SDK; build Release; upload artifacts.

Affected Files
- `native/vst3/CR717/**`
- `.github/workflows/build-vst3.yml` (if using GitHub)
- `scripts/validate_vst3.sh`

Tests
- VST3Validator run (no critical errors).
- Manual: load in Reaper/Ableton (if available) and confirm discovery, no audio glitches.

Risks & Mitigations
- SDK license/availability → document steps and env var requirements; avoid committing SDK.
- CI timeouts → cache dependencies; shallow clones.

Dependencies
- Xcode + CMake toolchain; JUCE 7; Steinberg VST3 SDK.

---

# Notes
- Best practices: RT‑safe coding, no allocation in `processBlock`; set explicit channel layouts; disable denormals.
- Reference: /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/best_practices_vst.md

