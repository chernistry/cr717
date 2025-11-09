#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace DesignTokens
{
    // Colors - Dark Theme
    namespace Colors
    {
        inline const juce::Colour bgPrimary{0xff1a1a1a};
        inline const juce::Colour bgSecondary{0xff2a2a2a};
        inline const juce::Colour bgTertiary{0xff333333};
        inline const juce::Colour bgElevated{0xff3a3a3a};
        
        inline const juce::Colour textPrimary{0xffffffff};
        inline const juce::Colour textSecondary{0xffaaaaaa};
        inline const juce::Colour textMuted{0xff666666};
        
        inline const juce::Colour accent{0xffff6b35};
        inline const juce::Colour accentHover{0xffff8555};
        inline const juce::Colour accentActive{0xffff4515};
        
        inline const juce::Colour success{0xff00ff00};
        inline const juce::Colour warning{0xffffaa00};
        inline const juce::Colour danger{0xffff3333};
        
        inline const juce::Colour border{0xff444444};
        inline const juce::Colour borderFocus = accent;
        
        // Instrument colors
        inline const juce::Colour instBD = accent;
        inline const juce::Colour instSD{0xffffaa00};
        inline const juce::Colour instCP{0xffff3385};
        inline const juce::Colour instHH{0xff66d9ef};
        inline const juce::Colour instTom{0xffc594c5};
        inline const juce::Colour instRS{0xffa6e22e};
        inline const juce::Colour instCY{0xfff92672};
    }
    
    // Spacing
    namespace Spacing
    {
        inline constexpr int xs = 4;
        inline constexpr int sm = 8;
        inline constexpr int md = 16;
        inline constexpr int lg = 24;
        inline constexpr int xl = 32;
    }
    
    // Border Radius
    namespace Radius
    {
        inline constexpr float sm = 2.0f;
        inline constexpr float md = 4.0f;
        inline constexpr float lg = 8.0f;
    }
    
    // Typography
    namespace Typography
    {
        inline constexpr float xs = 11.0f;
        inline constexpr float sm = 12.0f;
        inline constexpr float md = 14.0f;
        inline constexpr float lg = 16.0f;
        inline constexpr float xl = 20.0f;
    }
    
    // Transitions (UI only, never audio thread)
    namespace Transitions
    {
        inline constexpr int fast = 100;
        inline constexpr int normal = 200;
    }
}
