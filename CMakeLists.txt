cmake_minimum_required(VERSION 3.12)
project(lnc)

## ========================================================================== ##
## Settings
## ========================================================================== ##

set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Werror -Wall -Wextra")

## ========================================================================== ##
## Dependencies
## ========================================================================== ##

find_package(LLVM REQUIRED CONFIG)

add_subdirectory(deps/mimalloc)

## ========================================================================== ##
## Sources
## ========================================================================== ##

set(SOURCES
        src/args.c
        src/ast.c
        src/common.c
        src/con.c
        src/err.c
        src/file.c
        src/lex.c
        src/llvm_util.c
        src/lsp.c
        src/main.c
        src/parser.c
        src/span.c
        src/src.c
        src/str.c
        src/type.c
        deps/alf/alf_unicode.c
        deps/chif/chif_net.c
        deps/cjson/cJSON.c
        )

## ========================================================================== ##
## Executable
## ========================================================================== ##

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        m
        LLVM-8
        mimalloc-static
        )
target_include_directories(${PROJECT_NAME} PRIVATE
        src
        deps/alf
        deps/chif
        deps/cjson
        deps/mimalloc/include
        ${LLVM_INCLUDE_DIRS}
        )