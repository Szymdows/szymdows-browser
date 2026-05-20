cmake_minimum_required(VERSION 3.10)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Find the Rust library
# The Rust library will be in engine/parser/target/release
set(RUST_LIB_DIR ${CMAKE_SOURCE_DIR}/engine/parser/target/release)

# On Linux it's libhtml_parser.so, on Windows it's html_parser.dll
if(WIN32)
    set(RUST_LIB ${RUST_LIB_DIR}/html_parser.dll.lib)
else()
    set(RUST_LIB ${RUST_LIB_DIR}/libhtml_parser.so)
endif()

add_executable(SzymdowsBrowser
    ui/main.cpp
)

target_link_libraries(SzymdowsBrowser 
    renderer
    ${GTK3_LIBRARIES}
    ${RUST_LIB}  # Link with the Rust library!
)

target_include_directories(SzymdowsBrowser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/parser  # For parser_ffi.h
    ${GTK3_INCLUDE_DIRS}
)

target_compile_options(SzymdowsBrowser PRIVATE ${GTK3_CFLAGS_OTHER})

if(MSVC)
    target_compile_options(SzymdowsBrowser PRIVATE /W3)
else()
    target_compile_options(SzymdowsBrowser PRIVATE -Wall)
endif()