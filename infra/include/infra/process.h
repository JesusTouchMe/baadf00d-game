// Copyright 2025 JesusTouchMe

#ifndef INFRA_PROCESS_H
#define INFRA_PROCESS_H 1

#include "infra/structures.h"

#include <stddef.h>
#include <stdio.h>

typedef struct process* process_t;

process_t process_create(const char* name, const char* cwd, struct array_of_strings args, struct array_of_strings env, bool new_console);
bool process_is_running(process_t process);
int process_wait(process_t process);
void process_terminate(process_t process);
void process_destroy(process_t process);

#endif //INFRA_PROCESS_H