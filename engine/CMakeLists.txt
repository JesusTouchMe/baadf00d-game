cmake_minimum_required(VERSION 3.29)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib)

add_subdirectory(core)
add_subdirectory(launch)

target_include_directories(core PUBLIC shared_include)
target_include_directories(launch PUBLIC shared_include)
