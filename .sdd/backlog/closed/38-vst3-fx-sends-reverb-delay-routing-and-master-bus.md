Read /Users/sasha/IdeaProjects/personal_projects/cr717/.sdd/coding_rules.md first

# VST3 FX Sends (Reverb/Delay), Per‑Voice Routing, Master Bus (Comp/Limiter)

Objective
- Add studio‑grade FX to the plugin: per‑voice FX sends (Reverb, Delay), global FX returns, and a master bus chain (gentle comp + limiter + soft clip) with true‑peak metering. Keep latency 0 samples (lookahead optional off by default) and CPU lean.

Acceptance Criteria (DoD)
- Two FX sends per voice: SendA (Reverb), SendB (Delay). UI controls available per voice (0..1) and on FX panel.
- Reverb: high‑quality algorithmic (FreeverbX‑class) or convolution w/ small IR (optional oversampling flag); pre‑delay, size, damp, width; wet/dry; 0 added reported latency.
- Delay: tempo‑sync (1/8, 1/8T, 1/16, 1/4, dotted), feedback, filter (HP/LP), stereo width; wet/dry.
- Master Bus: gentle compressor (soft knee), limiter (no lookahead default), soft clipper; bypass switch; true‑peak meter (oversample TP detect only).
- Metering: per‑voice pre‑fader meter; master peak/RMS + TP; update 30–60 Hz; lock‑free taps.
- Routing: per‑voice Output (Main or Alt 1–4) to DAW; FX returns mixed to Main only (documented).

Steps
1) Architecture
   - Create FX buses: ReverbBus, DelayBus; per‑voice sends accumulate to buses.
   - Master chain in fixed order: Comp → Limiter → SoftClip → Output; bypass flags.
2) DSP
   - Implement algorithmic reverb (Schroeder/Moorer variant) or small IR convolution; ensure no allocations in process; parameter smoothing.
   - Implement delay with circular buffer; tempo sync via host BPM; feedback clamp; filters in feedback loop.
   - Master dynamics: light comp; limiter (brickwall); soft clip.
3) UI
   - FX tab: controls for both buses; meters for FX returns; master chain controls and meters.
4) QA
   - CPU budget at 44.1k/48k, 64/128 buffers; TP detection validated with test signals; no denormals.

Affected Files
- native/vst3/CR717/Source/dsp/{Reverb,Delay,Comp,Limiter,SoftClip}.*
- native/vst3/CR717/Source/{Mixer,Routing,PluginProcessor}.*
- native/vst3/CR717/Source/ui/{FXPanel,MasterPanel, Meters}.*

Risks & Mitigations
- Convolution CPU spikes → default to algorithmic reverb; IR as optional.
- Limiter lookahead adds latency → default off; expose small lookahead with latency report if enabled.

Dependencies
- 31 (DSP core), 32 (parameters), 33/35 (GUI), 37 (pan/outputs for routing).

