cmake_minimum_required(VERSION 3.22)
project(CR717 VERSION 1.0.0)

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

# Generate UI tokens header from JSON
add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui_tokens.hpp
    COMMAND ${CMAKE_COMMAND} -E env python3 
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/generate_tokens.py
        ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/tokens.json
        ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui_tokens.hpp
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/tokens.json
            ${CMAKE_CURRENT_SOURCE_DIR}/resources/ui/generate_tokens.py
    COMMENT "Generating ui_tokens.hpp from tokens.json"
)

add_custom_target(GenerateTokens DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/Source/ui_tokens.hpp)

# Plugin definition
juce_add_plugin(CR717
    COMPANY_NAME "TR808"
    PLUGIN_MANUFACTURER_CODE Tr08
    PLUGIN_CODE Tr8g
    FORMATS VST3
    PRODUCT_NAME "Cherni CR-717"
    IS_SYNTH TRUE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    VST3_CATEGORIES Instrument Synth Drum
)

add_dependencies(CR717 GenerateTokens)

target_sources(CR717 PRIVATE
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
    Source/Reverb.h
    Source/Delay.h
    Source/MasterDynamics.h
    Source/Preset.h
    Source/PatternRandomizer.h
    Source/Sequencer.h
    Source/MidiDragSource.h
    Source/MidiDragSource.cpp
    Source/TopBar.h
    Source/TopBar.cpp
)

target_compile_definitions(CR717 PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
)

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(CR717 PRIVATE -Wno-deprecated-declarations)
endif()

target_link_libraries(CR717 PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
)
