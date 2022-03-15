cmake_minimum_required(VERSION 3.16)
project(PL701 VERSION 0.1.0 LANGUAGES CXX C)

set(source 
    "src/pl701.c"
    "src/pl701.h"
    "src/tokenizer.h"
    "src/tokenizer.c"
    "src/error.h"
    "src/error.c"
    )


add_executable(pl701 ${source})
target_compile_options(pl701 PRIVATE -ggdb )

add_library(pl701_static ${source})
target_compile_options(pl701_static PRIVATE -ggdb )

add_subdirectory(tests)


