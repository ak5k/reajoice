cmake_minimum_required(VERSION 3.22)

project(reajoice VERSION 0.2.0)

set(
    FETCHCONTENT_BASE_DIR
    "${CMAKE_SOURCE_DIR}/.deps"
    CACHE PATH
    "Directory used by FetchContent for third-party sources"
    FORCE
)

include(FetchContent)

FetchContent_Declare(
    juce
    URL https://github.com/juce-framework/JUCE/archive/refs/heads/master.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(juce)

FetchContent_Declare(
    wdl_source
    URL https://github.com/justinfrankel/WDL/archive/refs/heads/main.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(wdl_source)

FetchContent_Declare(
    reaper_sdk_source
    URL https://github.com/justinfrankel/reaper-sdk/archive/refs/heads/main.tar.gz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(reaper_sdk_source)

set(REAJOICE_JUCE_SOURCE_DIR "${juce_SOURCE_DIR}" CACHE INTERNAL "")
set(REAJOICE_WDL_SOURCE_DIR "${wdl_source_SOURCE_DIR}" CACHE INTERNAL "")
set(REAJOICE_REAPER_SDK_SOURCE_DIR "${reaper_sdk_source_SOURCE_DIR}" CACHE INTERNAL "")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    if(WIN32)
        set(USER_CONFIG_DIR "$ENV{APPDATA}")
    elseif(APPLE)
        set(USER_CONFIG_DIR "~/Library/Application Support")
    else()
        set(USER_CONFIG_DIR "~/.config")
    endif()

    set(CMAKE_INSTALL_PREFIX
        "${USER_CONFIG_DIR}/REAPER"
        CACHE PATH
        "REAPER resource path where to install ReaPack"
        FORCE
    )
endif()

if(CMAKE_OSX_ARCHITECTURES)
    list(JOIN CMAKE_OSX_ARCHITECTURES "-" ARCH_NAME)
elseif(MSVC_CXX_ARCHITECTURE_ID)
    set(ARCH_NAME ${MSVC_CXX_ARCHITECTURE_ID})
else()
    set(ARCH_NAME ${CMAKE_SYSTEM_PROCESSOR})
endif()

string(TOLOWER "${ARCH_NAME}" ARCH_NAME)

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELWITHDEBINFO ON)

if(NOT WIN32)
    add_compile_options(
        -fsigned-char
        -fstack-protector-strong
        -fdiagnostics-color
    )
endif()

add_library(reaper_${PROJECT_NAME} SHARED)
set_target_properties(
    reaper_${PROJECT_NAME}
    PROPERTIES
        PREFIX
            "" # disable the "lib" prefix
        OUTPUT_NAME
            "reaper_${PROJECT_NAME}-${ARCH_NAME}"
)

file(
    GLOB_RECURSE _SOURCES
    CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.hpp"
)
target_sources(reaper_${PROJECT_NAME} PRIVATE ${_SOURCES})
target_include_directories(
    reaper_${PROJECT_NAME}
    PRIVATE
    "${REAJOICE_REAPER_SDK_SOURCE_DIR}/sdk"
)

set(REAPER_USER_PLUGINS "UserPlugins")
install(
    TARGETS
        reaper_${PROJECT_NAME}
    COMPONENT reajoice
    RUNTIME
        DESTINATION "${REAPER_USER_PLUGINS}" # Windows .dll
    LIBRARY
        DESTINATION "${REAPER_USER_PLUGINS}" # Linux .so/macOS .dylib
)

find_package(WDL REQUIRED)
target_link_libraries(reaper_${PROJECT_NAME} PRIVATE WDL::WDL)
if(NOT WIN32)
    find_package(SWELL REQUIRED)
endif()
if(SWELL_FOUND)
    target_link_libraries(reaper_${PROJECT_NAME} PRIVATE SWELL::swell)
endif()

include(Juce)
