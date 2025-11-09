#!/bin/bash
set -e

VST3_PATH="native/vst3/TR808Garage/build/TR808Garage_artefacts/Release/VST3/TR-808 Garage.vst3"

if [ ! -d "$VST3_PATH" ]; then
    echo "Error: VST3 not found at $VST3_PATH"
    echo "Run: cmake --build native/vst3/TR808Garage/build --config Release"
    exit 1
fi

echo "=== VST3 Validation ==="
echo "Plugin: $VST3_PATH"

# Check if pluginval is available
if command -v pluginval &> /dev/null; then
    echo "Running pluginval..."
    pluginval --strictness-level 8 --validate "$VST3_PATH"
else
    echo "Warning: pluginval not found. Install from: https://github.com/Tracktion/pluginval"
    echo "Skipping automated validation."
fi

# Basic structure check
echo ""
echo "=== Structure Check ==="
if [ -f "$VST3_PATH/Contents/MacOS/TR-808 Garage" ]; then
    echo "✓ Binary found"
    file "$VST3_PATH/Contents/MacOS/TR-808 Garage"
else
    echo "✗ Binary missing"
    exit 1
fi

if [ -f "$VST3_PATH/Contents/Info.plist" ]; then
    echo "✓ Info.plist found"
else
    echo "✗ Info.plist missing"
    exit 1
fi

echo ""
echo "=== Validation Complete ==="
