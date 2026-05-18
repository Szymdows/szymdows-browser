cmake_minimum_required(VERSION 3.10)

# Main CMake configuration file for Szymdows Browser
# This coordinates building all the C++ components

project(SzymdowsBrowser VERSION 0.1.2)

# Set C++ standard to C++11
# This is needed for modern C++ features like auto, lambdas, etc.
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Print some information about the build
message(STATUS "Building Szymdows Browser ${PROJECT_VERSION}")
message(STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER}")

# Build type information
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "Build type not specified, defaulting to Release")
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

# Add the renderer library subdirectory
# This compiles the rendering engine
add_subdirectory(engine/renderer)

# Add the browser subdirectory
# This compiles the main browser executable
add_subdirectory(browser)

# TODO: Figure out how to properly integrate the Rust components
# For now the Rust code is compiled separately by the build script
# Eventually these should be linked together properly
# Maybe using cargo-c or cbindgen?

# Print a message when configuration is complete
message(STATUS "CMake configuration complete")
message(STATUS "Run 'cmake --build .' to compile the project")