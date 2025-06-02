add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor/JUCE EXCLUDE_FROM_ALL)
set(target reaper_${PROJECT_NAME})

_juce_initialise_target(${target} ${target})

set(products_folder "${CMAKE_CURRENT_BINARY_DIR}")
set_target_properties(${target} PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${products_folder}"
    LIBRARY_OUTPUT_DIRECTORY "${products_folder}"
    RUNTIME_OUTPUT_DIRECTORY "${products_folder}")

target_link_libraries(reaper_${PROJECT_NAME}
    PRIVATE
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        # juce::juce_recommended_warning_flags
       )

target_compile_definitions(
    reaper_${PROJECT_NAME} 
    PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_MODAL_LOOPS_PERMITTED=1
)

set_target_properties(reaper_${PROJECT_NAME} PROPERTIES
    JUCE_NEEDS_WEB_BROWSER FALSE
    JUCE_NEEDS_CURL FALSE
)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE INTERNAL "")
endif()

set(ASIO_SDK_URL "https://www.steinberg.net/asiosdk")
set(ASIO_SDK_TEMPDIR "${CMAKE_CURRENT_BINARY_DIR}/asiosdk_temp")
set(ASIO_SDK_DIR "${CMAKE_CURRENT_BINARY_DIR}/asiosdk")

if(NOT EXISTS "${ASIO_SDK_DIR}")
    message(STATUS "Downloading ASIO SDK...")
    file(DOWNLOAD "${ASIO_SDK_URL}" "${CMAKE_CURRENT_BINARY_DIR}/asiosdk.zip"
        STATUS download_status
        SHOW_PROGRESS
    )
    if(download_status)
        message(STATUS "Extracting ASIO SDK...")
        file(ARCHIVE_EXTRACT 
            INPUT "${CMAKE_CURRENT_BINARY_DIR}/asiosdk.zip"
            DESTINATION "${ASIO_SDK_TEMPDIR}"
        )
        file(GLOB ASIO_SDK_DIRS "${ASIO_SDK_TEMPDIR}/*")
        foreach(dir ${ASIO_SDK_DIRS})
            if(IS_DIRECTORY ${dir} AND dir MATCHES ".*asiosdk.*")
            message(STATUS "ASIO SDK extracted to ${dir}")
            file(RENAME ${dir} ${ASIO_SDK_DIR})
            break()
            endif()
        endforeach()
    endif()
endif()

target_include_directories(
    reaper_${PROJECT_NAME}
    PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/asiosdk/common
)

target_include_directories(reaper_${PROJECT_NAME} PRIVATE "${ASIO_SDK_DIR}")

if(BUILD_WITH_ASIO)
    message(STATUS "Building with ASIO")
    target_compile_definitions(reaper_${PROJECT_NAME} PRIVATE JUCE_ASIO=1)
endif()
