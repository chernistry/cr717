#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "ui_tokens.hpp"

struct UiGrid
{
    static constexpr int COLUMNS = 12;
    
    int width;
    int margin;
    int gutter;
    int columnWidth;
    
    UiGrid(int totalWidth, int marginSize = ui::tokens::spacing::LG, int gutterSize = ui::tokens::spacing::MD)
        : width(totalWidth), margin(marginSize), gutter(gutterSize)
    {
        int availableWidth = width - (2 * margin) - (gutter * (COLUMNS - 1));
        columnWidth = availableWidth / COLUMNS;
    }
    
    int getX(int col) const
    {
        return margin + col * (columnWidth + gutter);
    }
    
    int getWidth(int cols) const
    {
        return cols * columnWidth + (cols - 1) * gutter;
    }
    
    juce::Rectangle<int> getBounds(int col, int y, int cols, int height) const
    {
        return juce::Rectangle<int>(getX(col), y, getWidth(cols), height);
    }
};
