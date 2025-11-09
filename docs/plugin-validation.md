# Plugin Validation

This project includes a native VST3 plugin implementation that can be validated using pluginval.

## Installing pluginval

pluginval has been installed via Homebrew:

```bash
brew install pluginval
```

A symlink was created to make the CLI tool accessible:

```bash
sudo ln -sf /Applications/pluginval.app/Contents/MacOS/pluginval /usr/local/bin/pluginval
```

## Validating the VST3 Plugin

To validate the plugin after building:

1. Build the plugin:
   ```bash
   cd native/vst3/TR808Garage
   cmake -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build --config Release
   ```

2. Run validation:
   ```bash
   ./scripts/validate_vst3.sh
   ```

Or run pluginval directly:
```bash
pluginval --strictness-level 8 --validate "native/vst3/TR808Garage/build/TR808Garage_artefacts/Release/VST3/TR-808 Garage.vst3"
```

## Purpose

pluginval is used to validate that the VST3 plugin:
- Meets Steinberg's VST3 SDK specifications
- Has proper structure and metadata
- Functions correctly without crashes
- Reports correct latency and channel configurations
- Complies with plugin standards for distribution