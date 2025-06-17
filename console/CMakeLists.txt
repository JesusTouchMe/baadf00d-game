cmake_minimum_required(VERSION 3.29)

include(CheckIPOSupported)
check_ipo_supported(RESULT supported OUTPUT error)

set(SOURCES
        src/main.c)

set(HEADERS)

source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${SOURCES} ${HEADERS})

add_executable(baadf00d-console ${SOURCES} ${HEADERS})

target_include_directories(baadf00d-console
    PUBLIC
        include
)

if(${CMAKE_BUILD_TYPE} MATCHES Release|RelWithDebInfo)
    if(supported)
        message(STATUS "enabling lto")
        set_target_properties(baadf00d-console PROPERTIES INTERPROCEDURAL_OPTIMIZATION TRUE)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(baadf00d-console PRIVATE -O3 -march=native)
    elseif(MSVC)
        target_compile_options(baadf00d-console PRIVATE /O2 /arch:AVX2)
    endif()
endif()

if(WIN32)
    target_compile_definitions(baadf00d-console PUBLIC PLATFORM_WINDOWS)
elseif(APPLE)
    target_compile_definitions(baadf00d-console PUBLIC PLATFORM_MACOS)
elseif(UNIX AND NOT APPLE)
    target_compile_definitions(baadf00d-console PUBLIC PLATFORM_LINUX)
endif()

set_property(TARGET baadf00d-console PROPERTY C_STANDARD 23)

target_link_libraries(baadf00d-console baadf00d::infra)