set(target reaper_${PROJECT_NAME})

_juce_initialise_target(${target} ${target})

set(products_folder "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(
    ${target}
    PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY
            "${products_folder}"
        LIBRARY_OUTPUT_DIRECTORY
            "${products_folder}"
        RUNTIME_OUTPUT_DIRECTORY
            "${products_folder}"
)

target_link_libraries(
    reaper_${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        $<IF:$<BOOL:${CI}>,juce::juce_recommended_lto_flags,>
        # juce::juce_recommended_warning_flags
)

target_compile_definitions(
    reaper_${PROJECT_NAME}
    PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_MODAL_LOOPS_PERMITTED=1
)

set_target_properties(
    reaper_${PROJECT_NAME}
    PROPERTIES
        JUCE_NEEDS_WEB_BROWSER
            FALSE
        JUCE_NEEDS_CURL
            FALSE
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY
        "MultiThreaded$<$<CONFIG:Debug>:Debug>"
        CACHE INTERNAL
        ""
    )
endif()
