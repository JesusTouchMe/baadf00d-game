cmake_minimum_required(VERSION 3.29)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES
        src/game_loop.c
)

set(HEADERS
    include/engine/game_loop.h
        include/engine/common.h
        ../shared_include/game_interface.h
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(engine SHARED ${SOURCES} ${HEADERS})

target_include_directories(engine
        PUBLIC
            include
)

set_target_properties(engine PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib
)

target_compile_definitions(engine PRIVATE engine_BUILD_DLL)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(engine PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(engine PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(engine PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(engine PUBLIC PLATFORM_WINDOWS)
    set_target_properties(engine PROPERTIES PREFIX "")
elseif(APPLE)
    target_compile_definitions(engine PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(engine PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET engine PROPERTY C_STANDARD 23)

target_link_libraries(engine infra)
