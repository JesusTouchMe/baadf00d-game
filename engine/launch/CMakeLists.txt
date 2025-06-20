cmake_minimum_required(VERSION 3.29)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES
        src/main.c
)

set(HEADERS
)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(launch ${SOURCES} ${HEADERS})

target_include_directories(launch
        PUBLIC
            include
)

set_target_properties(launch PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib
)

target_compile_definitions(launch PRIVATE launch_BUILD_DLL)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(launch PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(launch PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(launch PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(launch PUBLIC PLATFORM_WINDOWS)
    set_target_properties(launch PROPERTIES PREFIX "")
elseif(APPLE)
    target_compile_definitions(launch PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(launch PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET launch PROPERTY C_STANDARD 23)

target_link_libraries(launch infra)
