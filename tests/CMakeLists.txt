cmake_minimum_required(VERSION 3.16)
project(TESTS VERSION 0.1.0 LANGUAGES CXX C)

set(source 
    "tests.c"
    "tokenizer_tests.h"
    "tokenizer_tests.c")


add_executable(pl701_tests ${source})

target_compile_options(pl701_tests PRIVATE -ggdb )
target_include_directories(pl701_tests PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(pl701_tests PRIVATE pl701_static)
