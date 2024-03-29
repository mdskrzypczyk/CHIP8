cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")

mark_as_advanced(
        BUILD_GMOCK BUILD_GTEST BUILD_SHARED_LIBS
        gmock_build_tests gtest_build_samples gtest_build_tests
        gtest_disable_pthreads gtest_force_shared_crt gtest_hide_internal_symbols
)

set_target_properties(gtest PROPERTIES FOLDER extern)
set_target_properties(gtest_main PROPERTIES FOLDER extern)
set_target_properties(gmock PROPERTIES FOLDER extern)
set_target_properties(gmock_main PROPERTIES FOLDER extern)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})
    target_include_directories(${TESTNAME} PUBLIC ${CMAKE_SOURCE_DIR}/src)
    # link the Google test infrastructure, mocking library, and a default main function to
    # the test executable.  Remove gtest_main if writing your own main function.
    target_link_libraries(${TESTNAME} gtest gmock gtest_main)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
    gtest_discover_tests(${TESTNAME}
            # set a working directory so your project root so that you can find test data via paths relative to the project root
            WORKING_DIRECTORY ${PROJECT_DIR}
            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_DIR}"
            )
    set_target_properties(${TESTNAME} PROPERTIES FOLDER tests)
    target_link_libraries(${TESTNAME} ${SDL2_LIBRARY})
endmacro()

package_add_test(audio_test audio_test.cpp
        ${PROJECT_SOURCE_DIR}/src/audio.cpp
        )
package_add_test(chip8_test chip8_test.cpp
        ${PROJECT_SOURCE_DIR}/src/audio.cpp
        ${PROJECT_SOURCE_DIR}/src/chip8.cpp
        ${PROJECT_SOURCE_DIR}/src/input.cpp
        ${PROJECT_SOURCE_DIR}/src/graphics.cpp
        )
file(COPY "resources/test_opcode.ch8" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
package_add_test(input_test input_test.cpp
        ${PROJECT_SOURCE_DIR}/src/input.cpp
        )
package_add_test(graphics_test graphics_test.cpp
        ${PROJECT_SOURCE_DIR}/src/graphics.cpp
        )