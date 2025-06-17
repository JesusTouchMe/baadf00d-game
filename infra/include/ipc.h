// Copyright 2025 JesusTouchMe

#ifndef INFRA_IPC_H
#define INFRA_IPC_H 1

#include <stddef.h>

typedef struct ipc_server* ipc_server_t;
typedef struct ipc_client* ipc_client_t;

// --- Server ---

ipc_server_t ipc_server_create(const char* name);
bool ipc_server_listen(ipc_server_t server);
ipc_client_t ipc_server_accept(ipc_server_t server);
void ipc_server_close(ipc_server_t server);

// --- Client ---

ipc_client_t ipc_client_connect(const char* name);
void ipc_client_close(ipc_client_t client);

// --- Raw IO ---

ssize_t ipc_read(ipc_client_t client, void* buffer, size_t count);
ssize_t ipc_write(ipc_client_t client, const void* buffer, size_t count);

#endif //INFRA_IPC_H