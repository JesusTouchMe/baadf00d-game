// Copyright 2025 JesusTouchMe

#ifndef CORE_IPC_SERVER_HANDLER_H
#define CORE_IPC_SERVER_HANDLER_H 1

#include <infra/ipc.h>

void ipc_server_handler_init(void);

// usually for events
void broadcast_packet(struct ipc_base_packet* packet);

void setup_client_handler(ipc_client_t client);

#endif //CORE_IPC_SERVER_HANDLER_H