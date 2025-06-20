// Copyright 2025 JesusTouchMe

#include "infra/module.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char module_last_error_message[512];

void module_set_last_error_message(const char* error_message) {
    strncpy(module_last_error_message, error_message, sizeof(module_last_error_message));
}

static char* construct_platform_path(const char* path) {
    const char* prefix = "";
    const char* suffix = "";

    char* normalized_path = strdup(path);
    if (normalized_path == nullptr) return nullptr;

    for (char* p = normalized_path; *p != '\0'; p++) {
        if (*p == '\\') *p = '/';
    }

#ifdef PLATFORM_WINDOWS
    suffix = ".dll";
#elifdef PLATFORM_LINUX
    prefix = "lib";
    suffix = ".so";
#elifdef PLATFORM_MACOS
    prefix = "lib";
    suffix = ".dylib";
#endif

    const char* last_slash = strrchr(normalized_path, '/');
    const char* base_name = last_slash ? last_slash + 1 : normalized_path;

    size_t path_len = strlen(normalized_path);
    size_t base_len = strlen(base_name);
    size_t prefix_len = strlen(prefix);
    size_t suffix_len = strlen(suffix);

    size_t buffer_size = path_len + base_len + prefix_len + suffix_len + 1;
    char* full_path = malloc(buffer_size);
    if (full_path == nullptr) {
        free(normalized_path);
        return nullptr;
    }

    char* p = full_path;

    if (last_slash) {
        size_t dir_len = (size_t) (last_slash - normalized_path + 1);
        memcpy(p, normalized_path, dir_len);
        p += dir_len;
    }

    memcpy(p, prefix, prefix_len);
    p += prefix_len;

    memcpy(p, base_name, base_len);
    p += base_len;

    memcpy(p, suffix, suffix_len);
    p += suffix_len;

    *p = '\0';

#ifdef PLATFORM_WINDOWS
    // normalize for windows
    for (char* p = full_path; *p != '\0'; p++) {
        if (*p == '/') *p = '\\';
    }
#endif

    free(normalized_path);
    return full_path;
}

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void module_set_os_error_message(const char* error_message, DWORD error_code) {
    CHAR message[512];
    DWORD message_length = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        0,
        message,
        512, nullptr
    );

    if (message_length == 0) {
        HINSTANCE inst = LoadLibraryA("Ntdsbmsg.dll");
        if (inst == nullptr) {
            strncpy(message, "no error message found and could not load Ntdsbmsg.dll", 512);
            message_length = strlen(message);
        } else {
            message_length = FormatMessageA(
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                inst,
                error_code,
                0,
                message,
                512, nullptr
            );

            FreeLibrary(inst);
        }
    }

    if (message_length == 0) {
        strncpy(message, "no error message", 512);
    }

    snprintf(module_last_error_message, sizeof(module_last_error_message), "%s. GLE=%X (%s)", error_message, error_code, message);
}

module_t module_load(const char* name) {
    char* module_path = construct_platform_path(name);
    if (module_path == nullptr) {
        module_set_last_error_message("couldn't construct platform-specific module name");
        return nullptr;
    }

    HMODULE handle = LoadLibraryA(module_path);
    if (handle == nullptr) {
        module_set_os_error_message("failed to load module", GetLastError());
        free(module_path);
        return nullptr;
    }

    free(module_path);
    return (module_t) handle;
}

void* module_get_symbol(module_t module, const char* name) {
    HMODULE handle = (HMODULE) module;
    FARPROC proc = GetProcAddress(handle, name);

    if (proc == nullptr) {
        module_set_os_error_message("couldn't find symbol", GetLastError());
        return nullptr;
    }

    return (void*) proc;
}

void module_unload(module_t module) {
    if (module != nullptr) FreeLibrary((HMODULE) module);
}

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

#include <dlfcn.h>

void module_set_os_error_message(const char* error_message, int error_code) {
    snprintf(module_last_error_message, sizeof(module_last_error_message), "%s. errno=%X (%s)", error_message, error_code, strerror(error_code));

}

module_t module_load(const char* name) {
    char* module_path = construct_platform_path(name);
    if (module_path == nullptr) {
        module_set_last_error_message("couldn't construct platform-specific module name");
        return nullptr;
    }

    void* handle = dlopen(module_path, RTLD_NOW);
    if (handle == nullptr) {
        module_set_last_error_message(dlerror());
        return nullptr;
    }

    return (module_t) handle;
}

void* module_get_symbol(module_t module, const char* name) {
    void* sym = dlsym(module, name);
    if (sym == nullptr) {
        module_set_last_error_message(dlerror());
        return nullptr;
    }

    return sym;
}

void module_unload(module_t module) {
    if (module != nullptr) dlclose(module);
}

#else
#error "Unsupported platform"
#endif

const char* module_last_error(void) {
    return module_last_error_message;
}
