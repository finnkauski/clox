cmake_minimum_required(VERSION 3.16)
project(clox)

if(NOT CMAKE_BUILD_TYPE)
    set(default_build_type "Debug")
    message(STATUS "Set the build type to `${default_build_type}` as none was specified.")
    set(CMAKE_BUILD_TYPE ${default_build_type} CACHE STRING "Choose the build type." FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()
message(STATUS "clox Build Type: ${CMAKE_BUILD_TYPE}")

# Set the version for clox
set(clox_Version_Major 0)
set(clox_Version_Minor 1)
set(clox_Version_Patch 0)
set(clox_Version_Status "-dev")
set(PROJECT_VERSION
    "${clox_Version_Major}.${clox_Version_Minor}.${clox_Version_Patch}${clox_Version_Status}"
)
message(STATUS "${PROJECT_NAME} version: ${PROJECT_VERSION}")

include_directories(
    external/
)

# Initialisation
set(SOURCES
    src/main.c
    src/utils.c
    src/lexer.c
    src/stb_ds.c
    external/stb_ds.h
)

add_executable(
    clox
    ${SOURCES}
)

# Add compiler warnings
target_compile_options(clox PRIVATE -Wall -Wextra -Wpedantic)
