// Copyright 2025 JesusTouchMe

#include <infra/ipc.h>
#include <infra/protocols/ipc_packet.h>

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

    ipc_client_t client = ipc_server_accept(server);
    if (client == nullptr) {
        printf("Failed to accept client: %s\n", ipc_last_error());
        return 1;
    }

    struct ipc_base_packet* handshake = ipc_packet_read(client, ipc_read_all);
    if (handshake == nullptr) {
        printf("Failed to read handshake: %s\n", ipc_last_error());
        return 1;
    }

    printf("Handshake received, sending back\n");

    ssize_t written = ipc_packet_write(handshake, client, ipc_write_all);
    if (written < 0) {
        printf("Failed to write to client: %s\n", ipc_last_error());
        return written;
    }

    return 0;
}
