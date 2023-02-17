cmake_minimum_required(VERSION 3.13)

project(function_cache)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(FC_MAX_CACHED_AMOUNT 100 CACHE STRING "The maximal amount of cached solutions")
set(FC_CACHE_FILEPATH ${CMAKE_CURRENT_BINARY_DIR}/cache_file.dat CACHE FILEPATH
   "The path to the persistent function cahe")

add_executable(function_cache
  bin/function_cache.cpp
)

add_compile_definitions(function_cache
  PRIVATE FC_MAX_CACHE_SIZE=${FC_MAX_CACHE_SIZE}
  PRIVATE FC_CACHE_FILEPATH="${FC_CACHE_FILEPATH}"
)

target_include_directories(function_cache
  PRIVATE ${CMAKE_HOME_DIRECTORY}/include
)
