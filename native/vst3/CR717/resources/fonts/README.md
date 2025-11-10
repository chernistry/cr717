# Fonts for CR-717

## Current Setup

The plugin uses JUCE's default system fonts with custom sizing via `DesignTokens::Typography`.

## To Add Custom Fonts (Optional)

1. Download font files (TTF/OTF) - e.g., Inter, Roboto from Google Fonts
2. Place in this directory
3. Add to CMakeLists.txt:
   ```cmake
   juce_add_binary_data(CR717_BinaryData
       SOURCES
       resources/fonts/YourFont-Regular.ttf
       resources/fonts/YourFont-Bold.ttf
   )
   ```
4. Load in LookAndFeelCR717 constructor:
   ```cpp
   auto typeface = juce::Typeface::createSystemTypefaceFor(
       BinaryData::YourFontRegular_ttf,
       BinaryData::YourFontRegular_ttfSize
   );
   setDefaultSansSerifTypeface(typeface);
   ```

## Current Typography Scale

- XS: 11px (labels, hints)
- SM: 12px (body text)
- MD: 14px (default)
- LG: 16px (headings)
- XL: 20px (titles)

All defined in `DesignTokens::Typography`.
