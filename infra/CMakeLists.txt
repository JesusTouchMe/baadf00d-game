cmake_minimum_required(VERSION 3.29)

set(SOURCES
        src/ipc.c
    src/protocols/ipc_packet.c
        src/thread.c
        src/process.c
)

set(HEADERS
    include/infra/ipc.h
    include/infra/protocols/ipc_packet.h
        include/infra/thread.h
        include/infra/process.h
        include/infra/structures.h)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_library(baadf00d-infra ${SOURCES} ${HEADERS})
add_library(baadf00d::infra ALIAS baadf00d-infra)

target_include_directories(baadf00d-infra
    PUBLIC
        include
)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(baadf00d-infra PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(baadf00d-infra PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(baadf00d-infra PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(baadf00d-infra PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(baadf00d-infra PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(baadf00d-infra PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET baadf00d-infra PROPERTY C_STANDARD 23)