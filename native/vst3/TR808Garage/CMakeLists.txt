cmake_minimum_required(VERSION 3.22)
project(TR808Garage VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Fetch JUCE
include(FetchContent)
FetchContent_Declare(
  JUCE
  GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
  GIT_TAG 8.0.4
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(JUCE)

# Plugin definition
juce_add_plugin(TR808Garage
    COMPANY_NAME "TR808"
    PLUGIN_MANUFACTURER_CODE Tr08
    PLUGIN_CODE Tr8g
    FORMATS VST3
    PRODUCT_NAME "TR-808 Garage"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth Drum
)

target_sources(TR808Garage PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/Parameters.h
    Source/DesignTokens.h
    Source/CustomKnob.h
    Source/CustomFader.h
    Source/LevelMeter.h
    Source/LEDPad.h
    Source/Voice.h
    Source/BassDrumVoice.h
    Source/SnareDrumVoice.h
    Source/HiHatVoice.h
    Source/PercussionVoice.h
    Source/TomVoice.h
    Source/CymbalVoice.h
)

target_compile_definitions(TR808Garage PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

target_link_libraries(TR808Garage PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
