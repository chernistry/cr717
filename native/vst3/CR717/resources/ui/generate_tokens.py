#!/usr/bin/env python3
import json
import sys

def hex_to_rgb(hex_color):
    hex_color = hex_color.lstrip('#')
    return tuple(int(hex_color[i:i+2], 16) for i in (0, 2, 4))

def generate_header(tokens):
    lines = [
        "// Auto-generated from tokens.json - DO NOT EDIT",
        "#pragma once",
        "#include <juce_graphics/juce_graphics.h>",
        "",
        "namespace ui {",
        "namespace tokens {",
        ""
    ]
    
    # Colors
    lines.append("// Colors")
    lines.append("namespace colors {")
    for category, colors in tokens['colors'].items():
        for name, value in colors.items():
            r, g, b = hex_to_rgb(value)
            const_name = f"{category}_{name}".upper()
            lines.append(f"const juce::Colour {const_name} = juce::Colour({r}, {g}, {b});")
    lines.append("} // namespace colors")
    lines.append("")
    
    # Typography
    lines.append("// Typography")
    lines.append("namespace typography {")
    lines.append(f"constexpr const char* FONT_FAMILY = \"{tokens['typography']['fontFamily']}\";")
    for name, size in tokens['typography']['sizes'].items():
        lines.append(f"constexpr float SIZE_{name.upper()} = {size}.0f;")
    lines.append(f"constexpr float LINE_HEIGHT = {tokens['typography']['lineHeight']}f;")
    lines.append("} // namespace typography")
    lines.append("")
    
    # Spacing
    lines.append("// Spacing")
    lines.append("namespace spacing {")
    for name, value in tokens['spacing'].items():
        lines.append(f"constexpr int {name.upper()} = {value};")
    lines.append("} // namespace spacing")
    lines.append("")
    
    # Radii
    lines.append("// Radii")
    lines.append("namespace radii {")
    for name, value in tokens['radii'].items():
        lines.append(f"constexpr float {name.upper()} = {value}.0f;")
    lines.append("} // namespace radii")
    lines.append("")
    
    lines.append("} // namespace tokens")
    lines.append("} // namespace ui")
    
    return '\n'.join(lines)

if __name__ == '__main__':
    with open(sys.argv[1], 'r') as f:
        tokens = json.load(f)
    
    header = generate_header(tokens)
    
    with open(sys.argv[2], 'w') as f:
        f.write(header)
