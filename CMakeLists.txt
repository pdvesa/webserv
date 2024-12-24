# Minimum CMake version
cmake_minimum_required(VERSION 3.16)

# Project name and language
project(WebServ LANGUAGES CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/includes)

# Source files
file(GLOB_RECURSE SRC_FILES ${CMAKE_SOURCE_DIR}/src/*.cpp)

# Output directory for build files
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add the executable target
add_executable(${PROJECT_NAME} ${SRC_FILES})

file(GLOB UTIL_FILES
        ${CMAKE_SOURCE_DIR}/src/util/*.cpp
)
set(CUSTOM_FILES
        ${CMAKE_SOURCE_DIR}/src/model/HttpRequest.cpp
        ${UTIL_FILES}
        ${CMAKE_SOURCE_DIR}/src/run/requestTest.cpp
)

# Add a custom target (executable or library) for these files
add_executable(CustomTarget ${CUSTOM_FILES})

# Optional: Assign output directory for the custom target (optional)
set_target_properties(CustomTarget PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/custom
)