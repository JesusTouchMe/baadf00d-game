// Copyright 2025 JesusTouchMe

#ifndef INFRA_COMPILER_AND_PLATFORM_H
#define INFRA_COMPILER_AND_PLATFORM_H 1

// DLL stuff

#ifdef PLATFORM_WINDOWS
    #ifdef INFRA_BUILD_DLL
        #define INFRA_API __declspec(dllexport)
    #else
        #define INFRA_API __declspec(dllimport)
    #endif
#else
    #if __GNUC__ >= 4
        #define INFRA_API __attribute__((visibility("default")))
    #else
        #define INFRA_API
    #endif
#endif

#endif //INFRA_COMPILER_AND_PLATFORM_H
