cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Webserv VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add executable
file(GLOB_RECURSE SOURCES
        src/run/main.cpp
        src/controller/*.cpp
        src/model/*.cpp
        src/util/*.cpp
)

add_executable(webserv ${SOURCES})

# Include directories
target_include_directories(webserv PRIVATE includes)

# Enable testing
enable_testing()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines the gtest and gtest_main
# targets.
add_subdirectory(${CMAKE_BINARY_DIR}/googletest-src
        ${CMAKE_BINARY_DIR}/googletest-build)

# Test sources files
file(GLOB_RECURSE TEST_SOURCES
        test/model/*/*.cpp
        test/util/*/*.cpp
)

add_executable(webserv_test test/test.cpp ${TEST_SOURCES} ${SOURCES})

# Link Google Test libraries
target_link_libraries(webserv_test gtest gtest_main pthread)

# Include directories
target_include_directories(webserv_test PRIVATE includes)

# Add tests to CTest
add_test(NAME webserv_test COMMAND webserv_test)

# Add compilation flags
target_compile_options(webserv PRIVATE -Wall -Wextra -Werror)
#target_compile_options(webserv_test PRIVATE -Wall -Wextra -Werror)
