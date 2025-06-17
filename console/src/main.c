// Copyright 2025 JesusTouchMe

#include <infra/ipc.h>
#include <infra/protocols/ipc_packet.h>

#include <unistd.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s command prefix\n", argv[0]);
        return 1;
    }


    sleep(1);
    const char* command_prefix = argv[1];

    ipc_client_t client = ipc_client_connect("baadf00d");
    if (client == nullptr) {
        printf("Failed to connect to IPC: %s\n", ipc_last_error());
        return 1;
    }

    struct ipc_base_packet packet;

    ipc_packet_handshake_init(&packet);

    ssize_t written = ipc_packet_write(&packet, client, ipc_write_all);
    if (written < 0) {
        printf("Failed to write to IPC: %s\n", ipc_last_error());
        return 1;
    }

    struct ipc_base_packet* received_packet = ipc_packet_read(client, ipc_read_all); // TODO: make specific readers for each packet type
    if (received_packet == nullptr) {
        printf("Failed to read from IPC: %s\n", ipc_last_error());
        return 1;
    }

    return 0;
}
