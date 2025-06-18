// Copyright 2025 JesusTouchMe

#include "ipc_server_handler.h"

#include <infra/protocols/ipc_packet.h>

#include <infra/thread.h>

#include <stdlib.h>

static void* client_thread(void* arg) {
    ipc_client_t client = (ipc_client_t) arg;

    while (true) {
        struct ipc_base_packet* packet = ipc_packet_read(client, ipc_read_all);
        if (packet == nullptr) break;

        switch ((enum ipc_packet_type) packet->header.type) {
            case IPC_PACKET_HANDSHAKE: {
                struct ipc_base_packet handshake;
                ipc_packet_handshake_init(&handshake);

                ipc_packet_write(&handshake, client, ipc_write_all);

                break;
            }
            case IPC_PACKET_CMD:
                break;

            default:
                printf("Unknown IPC packet type: %d\n", packet->header.type);
                break;
        }

        // individual handlers handle individual cleanup, we just free the raw packet
        free(packet);
    }
    loop_end:

    ipc_client_close(client);

    return nullptr;
}

void setup_client_handler(ipc_client_t client) {
    thread_t thread = thread_create(client_thread, client);
    thread_detach(thread);
    thread_destroy(thread);
}
