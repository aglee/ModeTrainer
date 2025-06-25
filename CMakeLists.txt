cmake_minimum_required(VERSION 3.15)

project(ModeTrainer VERSION 1.0.0)

# Find JUCE
find_package(PkgConfig REQUIRED)

# Add JUCE as a subdirectory (assuming JUCE is installed via package manager or git submodule)
# Alternative: use CPM or FetchContent to download JUCE automatically
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)

FetchContent_MakeAvailable(JUCE)

# Create the application target
juce_add_gui_app(ModeTrainer
    PRODUCT_NAME "Mode Trainer"
    VERSION "1.0.0"
    COMPANY_NAME "Music Education"
    BUNDLE_ID "com.musiceducation.modetrainer"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/ModeTrainer.icns"
)

# Add source files
target_sources(ModeTrainer PRIVATE
    Source/Main.cpp
    Source/MainComponent.cpp
    Source/AudioEngine.cpp
)

# Link JUCE modules
target_link_libraries(ModeTrainer PRIVATE
    juce::juce_gui_extra
    juce::juce_audio_utils
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_processors
    juce::juce_audio_basics
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
)

# Compiler definitions
target_compile_definitions(ModeTrainer PRIVATE
    JUCE_WEB_BROWSER=1
    JUCE_USE_CURL=0
    JUCE_DISABLE_COMPONENT_SNAPSHOT=1
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ModeTrainer,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ModeTrainer,JUCE_VERSION>"
)

# Set C++ standard
target_compile_features(ModeTrainer PRIVATE cxx_std_17)

# Post-build step to ensure correct icon is copied
add_custom_command(TARGET ModeTrainer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/ModeTrainer.icns"
        "$<TARGET_BUNDLE_DIR:ModeTrainer>/Contents/Resources/Icon.icns"
    COMMENT "Copying icon file to app bundle"
)
