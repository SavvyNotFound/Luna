cmake_minimum_required(VERSION 3.16)
project(LunaEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ALLOWED_BUILD_TYPES Debug Release Dist)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

if(NOT CMAKE_BUILD_TYPE IN_LIST ALLOWED_BUILD_TYPES)
    message(FATAL_ERROR "Invalid build type '${CMAKE_BUILD_TYPE}'. Allowed types: ${ALLOWED_BUILD_TYPES}")
endif()

# Platforms Defines
if (WIN32)
    add_compile_definitions(LUNA_PLATFORM_WINDOWS)
elseif (LINUX)
    add_compile_definitions(LUNA_PLATFORM_LINUX)
elseif (APPLE)
    add_compile_definitions(LUNA_PLATFORM_MAC)
endif()

if(MSVC)
    # SPDLOG
    add_compile_options(/utf-8)
endif()

add_compile_options(
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O3>
)
add_compile_definitions(LUNA_FORCE_X11)

add_subdirectory(Luna)
add_subdirectory(Sandbox)
add_subdirectory(Crescent)
