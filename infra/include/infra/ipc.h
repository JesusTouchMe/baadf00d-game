// Copyright 2025 JesusTouchMe

#ifndef INFRA_IPC_H
#define INFRA_IPC_H 1

#include "infra/compiler_and_platform.h"

#include <stdio.h>

typedef struct ipc_server* ipc_server_t;
typedef struct ipc_client* ipc_client_t;

// --- Server ---

INFRA_API ipc_server_t ipc_server_create(const char* name);
INFRA_API bool ipc_server_listen(ipc_server_t server);
INFRA_API ipc_client_t ipc_server_accept(ipc_server_t server);
INFRA_API void ipc_server_close(ipc_server_t server);

// --- Client ---

INFRA_API ipc_client_t ipc_client_connect(const char* name);
INFRA_API void ipc_client_close(ipc_client_t client);

// --- Raw IO ---

INFRA_API ssize_t ipc_read(ipc_client_t client, void* buffer, size_t count);
INFRA_API ssize_t ipc_write(ipc_client_t client, const void* buffer, size_t count);

// --- IO Helpers ---

// These continuously read/write until there is no more left to read/write
INFRA_API ssize_t ipc_read_all(ipc_client_t server, void* buffer, size_t count);
INFRA_API ssize_t ipc_write_all(ipc_client_t server, const void* buffer, size_t count);

// --- Error ---

INFRA_API const char* ipc_last_error(void);

#endif //INFRA_IPC_H
