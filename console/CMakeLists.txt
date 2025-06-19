cmake_minimum_required(VERSION 3.29)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES
        src/main.c)

set(HEADERS
        include/ipc_packet_handler.h)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(console ${SOURCES} ${HEADERS})

target_include_directories(console
    PUBLIC
        include
)

set_target_properties(console PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(console PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(console PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(console PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(console PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(console PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(console PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET console PROPERTY C_STANDARD 23)

target_link_libraries(console infra)