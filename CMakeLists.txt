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
    # Code Coverage
    # Note: This needs to be done before including GoogleTest below
    include(CodeCoverage)
    append_coverage_compiler_flags()
    set(COVERAGE_LCOV_EXCLUDES "extern/*" "tests/*")
    setup_target_for_coverage_gcovr_html(NAME coverage
            EXECUTABLE ctest -C Debug --test-dir tests
            EXCLUDE ${COVERAGE_LCOV_EXCLUDES}
            DEPENDENCIES input_test)

    # Testing
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

# Documentation
find_package(Doxygen REQUIRED)
if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY docs)
    set(DOXYGEN_GENERATE_HTML YES)
    doxygen_add_docs(docs ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/src)
endif()

install(TARGETS chip8 DESTINATION bin)