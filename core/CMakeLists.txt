cmake_minimum_required(VERSION 3.29)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES
        src/main.c
        src/ipc_server_handler.c)

set(HEADERS
        include/ipc_server_handler.h)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(core ${SOURCES} ${HEADERS})

target_include_directories(core
    PUBLIC
        include
)

set_target_properties(core PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin
)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(core PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(core PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(core PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(core PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(core PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(core PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET core PROPERTY C_STANDARD 23)

target_link_libraries(core infra)