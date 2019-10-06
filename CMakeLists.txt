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
        src/ast.c
        src/common.c
        src/file.c
        src/lex.c
        src/main.c
        src/parser.c
        src/pos.c
        src/str.c
        deps/alf/alf_unicode.c
        )

## ========================================================================== ##
## Executable
## ========================================================================== ##

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME}
        LLVM-8
        mimalloc-static
        )
target_include_directories(${PROJECT_NAME} PRIVATE
        src
        deps/alf
        deps/mimalloc/include
        ${LLVM_INCLUDE_DIRS}
        )