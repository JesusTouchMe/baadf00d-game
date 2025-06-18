// Copyright 2025 JesusTouchMe

#include "ipc_server_handler.h"

#include <infra/ipc.h>

int main(int argc, char** argv) {
    ipc_server_t server = ipc_server_create("baadf00d");
    if (server == nullptr) {
        printf("Failed to create server: %s\n", ipc_last_error());
        return 1;
    }

    bool success = ipc_server_listen(server);
    if (!success) {
        printf("Failed to listen on server: %s\n", ipc_last_error());
        return 1;
    }

    while (true) {
        ipc_client_t client = ipc_server_accept(server);
        if (client == nullptr) break;

        setup_client_handler(client);
    }

    ipc_server_close(server);

    return 0;
}
