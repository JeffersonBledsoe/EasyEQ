# 3.15 is the minimum for JUCE plugins
cmake_minimum_required(VERSION 3.15)
project(EasyEQ VERSION 0.0.1)
add_subdirectory(JUCE)

# Will need these later for AAX + VST2
# juce_set_vst2_sdk_path(...)
# juce_set_aax_sdk_path(...)

juce_add_plugin(EasyEQ
    # ICON_BIG MyIconBig
    # ICON_SMALL MyIconSmall
    COMPANY_NAME Jefferson Bledsoe
    COMPANY_WEBSITE jeffersonbledsoe.com
    COMPANY_EMAIL me@jeffersonbledsoe.com
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Demo
    PLUGIN_CODE Demo
    FORMATS AU VST3 Standalone
    PRODUCT_NAME "EasyEQ"
    MICROPHONE_PERMISSION_ENABLED TRUE
)

set_property(TARGET EasyEQ PROPERTY CXX_STANDARD 17)

# Need this until we move to individually including needed module headers
juce_generate_juce_header(EasyEQ)

# Do I need to include EVERY cmake file here?
target_sources(EasyEQ PRIVATE
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp
    Source/HandleEditor.cpp
    Source/FrequencyResponsePlot.cpp
    Source/EasyEqLookAndFeel.cpp
    Source/ControlPanelBand.cpp
    Source/ControlPanel.cpp
    Source/BandHandle.cpp
)

target_compile_definitions(EasyEQ
    PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_USE_OGGVORBIS=0
    JUCE_USE_FLAC=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_STRICT_REFCOUNTEDPOINTER=1
)

# Don't think I need this as the code uses the BinaryData namespace
juce_add_binary_data(BinaryData SOURCES 
    Assets/ShapeIcons/BandPass.svg
    Assets/ShapeIcons/Bell.svg
    Assets/ShapeIcons/HighCut.svg
    Assets/ShapeIcons/HighShelf.svg
    Assets/ShapeIcons/LowCut.svg
    Assets/ShapeIcons/LowShelf.svg
    Assets/ShapeIcons/Notch.svg
)

target_link_libraries(EasyEQ PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_plugin_client
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    juce::juce_audio_processors
    juce::juce_audio_formats
    juce::juce_audio_devices
    juce::juce_dsp
    BinaryData
)