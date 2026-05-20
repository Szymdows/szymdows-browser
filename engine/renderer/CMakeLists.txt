cmake_minimum_required(VERSION 3.10)

# CMake configuration for the Renderer component
# This compiles the rendering engine as a static library

# Define the renderer library
# Using STATIC means this will be compiled into a .lib or .a file
add_library(renderer STATIC
    render.cpp
    render.h
)

# Make the header files available to other parts of the project
# This allows other components to #include "render.h"
target_include_directories(renderer PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# Set some compiler warnings to help catch issues
# These are good practice and help find bugs early
if(MSVC)
    # Microsoft Visual C++ compiler warnings
    target_compile_options(renderer PRIVATE /W3)
else()
    # GCC/Clang compiler warnings
    target_compile_options(renderer PRIVATE -Wall -Wextra)
endif()
