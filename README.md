# TR-808 Synth

A simple drum sequencer created with AI in about 30 minutes, inspired by the classic TR-808 but not pretending to replicate the original. Use it freely in your projects!

This project demonstrates the SDD (Spec-Driven Development) approach - a methodology for rapid development using AI agents with structured prompts and templates. For more details on SDD, check out [SDDRush](https://github.com/chernistry/SDDRush).

All prompts used in this project are available in both locations - the SDDRush repo and the sdd directory within this project.

## Features

- 5 synthesized drum voices (Bass Drum, Snare, Closed Hi-Hat, Open Hi-Hat, Clap)
- 16-step sequencer grid
- Adjustable tempo (60-240 BPM)
- Pattern saving/loading via browser storage
- Visual step highlighting during playback

## How to Use

```bash
# Install dependencies
npm install

# Start development server
npm run dev

# Build for production
npm run build
```

## Tech Stack

- TypeScript
- Web Audio API
- Vite
- Tests: Vitest + Playwright

## License

MIT
