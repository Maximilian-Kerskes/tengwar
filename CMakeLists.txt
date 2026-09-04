cmake_minimum_required(VERSION 3.20)

project(tengwar
    VERSION 0.1.0
    LANGUAGES C
)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_library(tengwar_core
    src/lexer.c
    src/parser.c
)

target_include_directories(tengwar_core PUBLIC
    include
)

target_compile_definitions(tengwar_core PUBLIC
    _POSIX_C_SOURCE=200809L
)

target_compile_options(tengwar_core PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

add_executable(tengwar
    src/main.c
)

target_link_libraries(tengwar PRIVATE
    tengwar_core
)

enable_testing()
add_subdirectory(tests)
