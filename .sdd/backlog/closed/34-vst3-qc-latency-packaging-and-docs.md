Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 QC — Latency/Performance, Packaging, and Integration Docs

Objective
- Validate latency and performance across common buffer sizes; package the VST3 build; produce a short integration guide (Reaper/Ableton/Logic via AU later). Ensure quality gates and release artifacts.

Acceptance Criteria (DoD)
- Latency: plugin reports 0 samples latency; no additional buffers introduced.
- Performance: at 44.1k/48k, 64/128 buffer, CPU ≤ 3% on laptop; no XRuns/clicks in smoke tests.
- Artifacts: signed (if applicable) `.vst3` in `dist/plugins/` with versioned filename.
- Docs: `native/vst3/CR717/README.md` with install/use steps and troubleshooting.

Steps
1) Latency & perf tests
   - Measure CPU at various buffer sizes (host meters); set a threshold and record results.
   - Validate `VST3Validator` again on Release build.
2) Packaging
   - Copy .vst3 to `dist/plugins/CR717-<version>-macos.vst3.zip`.
   - Optional: codesign/notarize if distribution planned.
3) Docs
   - DAW integration notes; MIDI mapping; preset loading; known issues.
4) Release
   - Tag and attach artifact (if using GitHub Releases); checksum.

Affected Files
- `native/vst3/CR717/README.md`
- `dist/plugins/*`
- `.github/workflows/build-vst3.yml` (release step, optional)

Risks & Mitigations
- DAW-specific quirks → list workarounds; test in at least two DAWs.
- Codesign/notarization complexity → document and gate behind env flags.

Dependencies
- Tickets 30–33.

