# 61 — Parameter Registry, UI Mapping, and State Persistence

Read `.sdd/best_practices_vst.md` and native coding rules first.

Context
- Spec: `.sdd/backlog/tasks/004-rehaul-sounds-implement.md` (global: expose parameters)
- Goal: Centralize parameters (IDs, ranges, defaults), bind to JUCE GUI controls, persist via `AudioProcessorValueTreeState` (APVTS) and host state.

Dependencies
- Tickets 56–60 provide DSP endpoints

Scope
- Define APVTS `ParameterLayout` with metadata (min/max/default, unit, scaling)
- UI mapping: `SliderAttachment`, `ComboBoxAttachment`, `ButtonAttachment` to bind UI to APVTS
- Persistence: implement `getStateInformation`/`setStateInformation` and ValueTree state versioning/migration
- Reserve stable parameter IDs for automation/host compatibility

Acceptance Criteria
- All new dynamics/FX/voice params appear in UI with correct ranges and tooltips
- Changes reflect immediately in audio; state saved/restored by host and APVTS
- C++ unit tests validate layout completeness and state serialization schema

Implementation Steps
1) Files (native VST3): `native/vst3/CR717/Source/parameters/Parameters.h/.cpp`
2) Build `createParameterLayout()` returning APVTS layout; include all new params
3) GUI: bind components using APVTS attachments; adopt tokens from Ticket 63
4) Implement `getStateInformation`/`setStateInformation` with versioning/migration
5) Tests: `native/tests/unit/params/test_parameters.cpp`, `test_state_serialization.cpp`

Affected Files
- `native/vst3/CR717/Source/parameters/Parameters.*` (new)
- `native/vst3/CR717/Source/ui/editor/*` (updates for attachments)
- `native/tests/unit/params/*`, `native/tests/unit/state/*`

Risks & Mitigations
- UI clutter: group controls per section (Voices/Dynamics/FX); hide advanced
- Backward-compat: version config schema; provide migration defaults
