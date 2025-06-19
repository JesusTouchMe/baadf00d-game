// Copyright 2025 JesusTouchMe

#ifndef INFRA_PROCESS_H
#define INFRA_PROCESS_H 1

#include "infra/compiler_and_platform.h"
#include "infra/structures.h"

#include <stddef.h>
#include <stdio.h>

typedef struct process* process_t;

INFRA_API process_t process_create(const char* name, const char* cwd, struct array_of_strings args, struct array_of_string_pairs env, bool new_console);
INFRA_API bool process_is_running(process_t process);
INFRA_API int process_wait(process_t process);
INFRA_API void process_terminate(process_t process);
INFRA_API void process_destroy(process_t process);

#endif //INFRA_PROCESS_H