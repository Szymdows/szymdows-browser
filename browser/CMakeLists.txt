cmake_minimum_required(VERSION 3.10)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

add_executable(SzymdowsBrowser
    ui/main.cpp
)

target_link_libraries(SzymdowsBrowser 
    renderer
    ${GTK3_LIBRARIES}
)

target_include_directories(SzymdowsBrowser PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../engine/renderer
    ${GTK3_INCLUDE_DIRS}
)

target_compile_options(SzymdowsBrowser PRIVATE ${GTK3_CFLAGS_OTHER})

if(MSVC)
    target_compile_options(SzymdowsBrowser PRIVATE /W3)
else()
    target_compile_options(SzymdowsBrowser PRIVATE -Wall)
endif()