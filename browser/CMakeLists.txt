cmake_minimum_required(VERSION 3.10)

# CMake configuration for the Browser UI component
# This builds the main browser executable

# Create the main browser executable
# This compiles main.cpp and links it with the renderer library
add_executable(SzymdowsBrowser
    ui/main.cpp
)

# Link with the renderer library
# This makes the rendering functions available to the browser
target_link_libraries(SzymdowsBrowser 
    renderer
)

# Add include directories so we can find the headers
target_include_directories(SzymdowsBrowser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/renderer
)

# Enable compiler warnings to help catch potential issues
if(MSVC)
    target_compile_options(SzymdowsBrowser PRIVATE /W3)
else()
    target_compile_options(SzymdowsBrowser PRIVATE -Wall)
endif()