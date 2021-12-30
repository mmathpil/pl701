cmake_minimum_required(VERSION 3.16)
project(PL701 VERSION 0.1.0 LANGUAGES CXX C)

set(source 
    "src/pl701.c"
    "src/pl701.h"
    "src/tokenizer.h"
    "src/tokenizer.c"
    "src/error.h"
    )

add_executable(pl701 ${source})

