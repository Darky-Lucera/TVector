cmake_minimum_required(VERSION 3.14)

project(TVector VERSION 0.1.0 LANGUAGES CXX)

# Set features
#--------------------------------------
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#--------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

# Testing
#--------------------------------------
option(ENABLE_DOCTESTS "Enable tests using doctest library" ON)
if (ENABLE_DOCTESTS)
    add_subdirectory(tests)
endif()

# Set compiler flags
#--------------------------------------
if(NOT MSVC)
    add_compile_options("$<$<CONFIG:Debug>:-g>")
    add_compile_options("$<IF:$<CONFIG:Debug>,-O0,-O2>")
    add_compile_options(-Wall -Wextra)
    add_compile_options(-Wno-switch -Wno-unused-function -Wno-unused-parameter -Wno-implicit-fallthrough)

    if(NOT APPLE)
        add_compile_options(-Wno-cast-function-type)
    endif()
else()
    # Security check
    add_compile_options(/GS)
    # Function level linking
    add_compile_options(/Gy)
    # Exceptions
    add_compile_options(/EHsc)
    if(MSVC_VERSION GREATER_EQUAL 1900)
        # SDL checks 2015+
        add_compile_options(/sdl)
    endif()
    if(MSVC_VERSION LESS_EQUAL 1920)
        # Enable Minimal Rebuild (required for Edit and Continue) (deprecated)
        add_compile_options(/Gm)
    endif()
    add_compile_options(/fp:fast)
    # Program database for edit and continue
    add_compile_options("$<IF:$<CONFIG:Debug>,/ZI,/Zi>")
    # Optimizations
    add_compile_options("$<IF:$<CONFIG:Debug>,/Od,/O2>")
    # Inline function expansion
    add_compile_options("$<IF:$<CONFIG:Debug>,/Ob0,/Ob2>")
    # Basic runtime checks
    add_compile_options("$<$<CONFIG:Debug>:/RTC1>")
    # Force Visual Studio to actualize __cplusplus version macro
    add_compile_options(/Zc:__cplusplus)
endif()

#--------------------------------------
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-D_DEBUG)
    add_definitions(-DDEBUG)
endif()

# Set output directory
#--------------------------------------
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

#--------------------------------------
add_library(TVector INTERFACE
    src/TVector.h
)
#target_sources(TVector PRIVATE
#    src/core_TVector.h
#    src/redux_TVector.h
#)
target_include_directories(TVector INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/src)
#target_compile_features(TVector INTERFACE cxx_std_17)

install(FILES src/TVector.h DESTINATION include)
#install(TARGETS TVector DESTINATION lib)
