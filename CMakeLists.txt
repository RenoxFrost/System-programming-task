# =============================================================================
#  Student points analyzer - CMake build (Windows, Linux, macOS)
#  Usage:  cmake -S . -B build
#          cmake --build build --config Release
#          ./build/students        (Windows: build\Release\students.exe)
# =============================================================================

cmake_minimum_required(VERSION 3.15)

project(students
        VERSION 1.0.0
        DESCRIPTION "Student points analyzer - Systems Programming tasks 1-3"
        LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(SOURCES
    src/main.cpp
    src/person.cpp
    src/generator.cpp
    src/benchmark.cpp)

set(HEADERS
    include/person.h
    include/exceptions.h
    include/timer.h
    include/generator.h
    include/fileio.h
    include/processing.h
    include/benchmark.h)

add_executable(students ${SOURCES} ${HEADERS})
target_include_directories(students PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
    target_compile_options(students PRIVATE /W4 /permissive-)
else()
    target_compile_options(students PRIVATE -Wall -Wextra -pedantic)
endif()

set_target_properties(students PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
