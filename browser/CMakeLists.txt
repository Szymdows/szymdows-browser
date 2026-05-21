cmake_minimum_required(VERSION 3.10)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
find_package(CURL REQUIRED)

# The FFI wrapper is in engine/parser and is now called html_parser_ffi
set(RUST_LIB_DIR ${CMAKE_SOURCE_DIR}/engine/parser/target/release)

if(WIN32)
    set(RUST_LIB ${RUST_LIB_DIR}/html_parser_ffi.dll.lib)
else()
    set(RUST_LIB ${RUST_LIB_DIR}/libhtml_parser_ffi.so)
endif()

add_executable(SzymdowsBrowser
    ui/main.cpp
)

target_link_libraries(SzymdowsBrowser 
    renderer
    ${GTK3_LIBRARIES}
    ${RUST_LIB}
    ${CURL_LIBRARIES}
)

target_include_directories(SzymdowsBrowser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/renderer
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/parser
    ${GTK3_INCLUDE_DIRS}
    ${CURL_INCLUDE_DIRS}
)

target_compile_options(SzymdowsBrowser PRIVATE ${GTK3_CFLAGS_OTHER})

if(MSVC)
    target_compile_options(SzymdowsBrowser PRIVATE /W3)
else()
    target_compile_options(SzymdowsBrowser PRIVATE -Wall)
endif()