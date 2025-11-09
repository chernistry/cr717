#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class MidiDragSource : public juce::Component
{
public:
    MidiDragSource()
    {
        setSize(120, 30);
    }
    
    void setMidiFile(const juce::MidiFile& file)
    {
        midiFile = file;
    }
    
    void paint(juce::Graphics& g) override
    {
        g.fillAll(juce::Colour(0xff3a3a3a));
        g.setColour(juce::Colour(0xffff6b35));
        g.drawRect(getLocalBounds(), 2);
        
        g.setFont(12.0f);
        g.drawText("⬇ Drag MIDI", getLocalBounds(), juce::Justification::centred);
    }
    
    void mouseDown(const juce::MouseEvent&) override
    {
        dragStartPos = getMouseXYRelative();
    }
    
    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (!isDragging && e.getDistanceFromDragStart() > 5)
        {
            isDragging = true;
            
            // Create temporary MIDI file with timestamp
            auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory);
            auto timestamp = juce::Time::getCurrentTime().toMilliseconds();
            tempFile = tempDir.getChildFile("TR808_Pattern_" + juce::String(timestamp) + ".mid");
            
            // Write MIDI file
            juce::FileOutputStream stream(tempFile);
            if (stream.openedOk())
            {
                midiFile.writeTo(stream);
                stream.flush();
            }
            
            // Perform external drag & drop
            juce::StringArray files;
            files.add(tempFile.getFullPathName());
            
            // This is the key: use performExternalDragDropOfFiles for OS-level drag
            juce::DragAndDropContainer::performExternalDragDropOfFiles(files, false, this, nullptr);
        }
    }
    
    void mouseUp(const juce::MouseEvent&) override
    {
        isDragging = false;
    }
    
private:
    juce::MidiFile midiFile;
    juce::File tempFile;
    juce::Point<int> dragStartPos;
    bool isDragging = false;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MidiDragSource)
};
