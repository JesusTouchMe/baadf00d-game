// Copyright 2025 JesusTouchMe

#ifndef INFRA_MODULE_H
#define INFRA_MODULE_H 1

#include "infra/compiler_and_platform.h"

typedef struct module* module_t;

INFRA_API module_t module_load(const char* name);
INFRA_API void* module_get_symbol(module_t module, const char* name);
INFRA_API void module_unload(module_t module);
INFRA_API const char* module_last_error(void);

#endif //INFRA_MODULE_H