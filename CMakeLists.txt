cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(CHIP8 VERSION 1.5
        DESCRIPTION "CHIP-8 Interpretter"
        LANGUAGES CXX)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include)

add_executable(chip8
        src/main.cpp
        src/chip8.cpp
        src/audio.cpp
        src/input.cpp
        src/graphics.cpp
)

target_link_libraries(chip8 ${SDL2_LIBRARY})

option(PACKAGE_TESTS "Build the tests" ON)

if(PACKAGE_TESTS)
    enable_testing()
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(COVERAGE_LCOV_EXCLUDES "extern/*")
    setup_target_for_coverage_gcovr_html(NAME coverage
            EXECUTABLE ctest --schedule-random -j 4 --test-dir tests
            EXCLUDE ${COVERAGE_LCOV_EXCLUDES}
            DEPENDENCIES input_test)
    include(GoogleTest)
    add_subdirectory(tests)
endif()
