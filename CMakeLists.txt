cmake_minimum_required(VERSION 3.22)
project(MenuBarRecorder VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
elseif(APPLE)
    add_compile_options(-Wno-deprecated-declarations)
endif()

include(CMake/CPM.cmake)

CPMAddPackage(
        NAME JUCE
        GITHUB_REPOSITORY juce-framework/JUCE
        GIT_TAG 8.0.6  # Or your desired JUCE version
)


juce_add_binary_data(BinaryData
        SOURCES
        Resources/IBMPlexMono-Regular.ttf
        NAMESPACE BinaryData
)

set (MacPList "<plist version=\"1.0\">
<dict>
    <key>LSUIElement</key>
    <true/>
</dict>
</plist>")



juce_add_gui_app(${PROJECT_NAME}
        COMPANY_NAME "YourCompanyName"  # *** CHANGE THIS ***
        PRODUCT_NAME "MenuBarRecorder"
        PLIST_TO_MERGE "${MacPList}"
        BUNDLE_ID "com.yourcompany.MenuBarRecorder"  # *** CHANGE THIS (UNIQUE ID) ***
        MACOSX_CODE_SIGN_ENTITLEMENTS ${CMAKE_CURRENT_SOURCE_DIR}/MenuBarRecorder.entitlements
        VERSION 0.1.0
)


target_sources(${PROJECT_NAME}
        PRIVATE
        Source/Main.cpp
        Source/SystemAudioDeviceSwitcher.cpp
        Source/MainContentComponent.cpp
        Source/AudioHandler.cpp
        Source/AudioRecorder.cpp
)

target_include_directories(${PROJECT_NAME}
        PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/JuceLibraryCode
        ${CMAKE_CURRENT_BINARY_DIR}
        Source
)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        BinaryData
        juce::juce_gui_basics
        juce::juce_gui_extra
        juce::juce_audio_basics
        juce::juce_audio_devices
        juce::juce_audio_formats
        juce::juce_audio_utils
        juce::juce_data_structures
        juce::juce_events
        PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
)

if(APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE "-framework CoreAudio")
endif()

target_compile_definitions(${PROJECT_NAME}
        PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_SILENCE_XCODE_15_LINKER_WARNING=1
        JUCE_APP_NAME="MenuBar Recorder"
        JUCE_APP_VERSION="0.1.0"
)