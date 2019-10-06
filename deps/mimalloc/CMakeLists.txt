cmake_minimum_required(VERSION 3.0)
project(libmimalloc C CXX)
include("cmake/mimalloc-config-version.cmake")
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

option(MI_INTERPOSE "Use interpose to override standard malloc on macOS" OFF)
option(MI_SEE_ASM "Generate assembly files" OFF)
option(MI_CHECK_FULL "Use full internal invariant checking in DEBUG mode" OFF)
option(MI_USE_CXX "Use the C++ compiler to compile the library" OFF)
option(MI_SECURE "Use security mitigations (like guard pages and randomization)" OFF)

set(mi_install_dir "lib/mimalloc-${mi_version}")

set(mi_sources
  src/stats.c
  src/os.c
  src/segment.c
  src/page.c
  src/alloc.c
  src/alloc-aligned.c
  src/alloc-posix.c
  src/heap.c
  src/options.c
  src/init.c)


# Set default build type
if (NOT CMAKE_BUILD_TYPE)
  if ("${CMAKE_BINARY_DIR}" MATCHES ".*(D|d)ebug$")
    #message(STATUS "No build type selected, default to *** Debug ***")
    set(CMAKE_BUILD_TYPE "Debug")
  else ()
    #message(STATUS "No build type selected, default to *** Release ***")
    set(CMAKE_BUILD_TYPE "Release")
  endif ()
else ()
  #message(STATUS "Build type specified as *** ${CMAKE_BUILD_TYPE} ***")
endif ()

if ("${CMAKE_BINARY_DIR}" MATCHES ".*(S|s)ecure$")
  set(MI_SECURE "ON")
endif ()

if (MI_SECURE MATCHES "ON")
  message(STATUS "Set secure build (MI_SECURE=ON)")
  list(APPEND mi_defines MI_SECURE=2)
endif ()

if (MI_SEE_ASM MATCHES "ON")
  message(STATUS "Generate assembly listings (MI_SEE_ASM=ON)")
  list(APPEND mi_cflags -save-temps)
endif ()

if (MI_CHECK_FULL MATCHES "ON")
  message(STATUS "Set debug level to full invariant checking (MI_CHECK_FULL=ON)")
  list(APPEND mi_defines MI_DEBUG=3)   # full invariant checking
endif ()

if (MI_USE_CXX MATCHES "ON")
  message(STATUS "Use the C++ compiler to compile (MI_USE_CXX=ON)")
  set_source_files_properties(${mi_sources} PROPERTIES LANGUAGE CXX)
  set_source_files_properties(src/static.c test/test-api.c PROPERTIES LANGUAGE CXX)
endif ()

# Compiler flags
if (CMAKE_C_COMPILER_ID MATCHES "AppleClang|Clang|GNU")
  list(APPEND mi_cflags -Wall -Wextra -Wno-unknown-pragmas -ftls-model=initial-exec)
  if (CMAKE_C_COMPILER_ID MATCHES "GNU")
    list(APPEND mi_cflags -Wno-invalid-memory-model)
    list(APPEND mi_cflags -fvisibility=hidden)
  endif ()
endif ()

if (NOT (CMAKE_BUILD_TYPE MATCHES "Release|RelWithDebInfo"))
  string(TOLOWER "${CMAKE_BUILD_TYPE}" build_type)
  set(mi_basename "mimalloc-${build_type}")
else ()
  if (MI_SECURE MATCHES "ON")
    set(mi_basename "mimalloc-secure")
  else ()
    set(mi_basename "mimalloc")
  endif ()
endif ()

# extra needed libraries
if (WIN32)
  list(APPEND mi_libraries psapi shell32 user32)
else ()
  list(APPEND mi_libraries pthread)
endif ()

# -----------------------------------------------------------------------------
# Main targets
# -----------------------------------------------------------------------------

# static library
add_library(mimalloc-static STATIC ${mi_sources})
if (WIN32)
  # When building both static and shared libraries on Windows, a static library should use a
  # different output name to avoid the conflict with the import library of a shared one.
  string(REPLACE "mimalloc" "mimalloc-static" mi_output_name ${mi_basename})
  set_target_properties(mimalloc-static PROPERTIES OUTPUT_NAME ${mi_output_name})
else ()
  set_target_properties(mimalloc-static PROPERTIES OUTPUT_NAME ${mi_basename})
endif ()
target_compile_definitions(mimalloc-static PRIVATE ${mi_defines} MI_STATIC_LIB)
target_compile_options(mimalloc-static PRIVATE ${mi_cflags})
target_include_directories(mimalloc-static PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  )
target_link_libraries(mimalloc-static PUBLIC ${mi_libraries})
