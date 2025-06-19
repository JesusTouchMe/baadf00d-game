// Copyright 2025 JesusTouchMe

#include "ipc_server_handler.h"

#include <infra/thread.h>

#include <stdlib.h>

struct client {
    ipc_client_t handle;
    struct client* next;
    struct client* prev;
};

struct client* clients_head = nullptr;
mutex_t clients_mutex;

static void* client_thread(void* arg) {
    struct client* client = (struct client*) arg;

    while (true) {
        struct ipc_base_packet* packet = ipc_packet_read(client->handle, ipc_read_all);
        if (packet == nullptr) break;

        switch ((enum ipc_packet_type) packet->header.type) {
            case IPC_PACKET_HANDSHAKE: {
                struct ipc_base_packet handshake;
                ipc_packet_handshake_init(&handshake);

                ipc_packet_write(&handshake, client->handle, ipc_write_all);

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

    mutex_lock(clients_mutex);

    ipc_client_close(client->handle);

    if (clients_head == client) {
        clients_head = client->next;
    }

    if (client->next != nullptr) {
        client->next->prev = client->prev;
    }

    if (client->prev != nullptr) {
        client->prev->next = client->next;
    }

    mutex_unlock(clients_mutex);

    free(client);

    return nullptr;
}

void ipc_server_handler_init(void) {
    clients_mutex = mutex_create();
    if (clients_mutex == nullptr) {
        printf("Failed to create mutex\n");
        exit(1);
    }
}

void broadcast_packet(struct ipc_base_packet* packet) {
    mutex_lock(clients_mutex);

    for (struct client* c = clients_head; c != nullptr; c = c->next) {
        ipc_packet_write(packet, c->handle, ipc_write_all);
    }

    mutex_unlock(clients_mutex);
}

void setup_client_handler(ipc_client_t client_handle) {
    struct client* client = malloc(sizeof(struct client));
    if (client == nullptr) {
        printf("Failed to allocate memory for client\n");
        exit(1);
    }

    mutex_lock(clients_mutex);

    client->handle = client_handle;
    client->next = clients_head;
    client->prev = nullptr;

    if (clients_head != nullptr) {
        clients_head->prev = client;
    }

    clients_head = client;

    mutex_unlock(clients_mutex);

    thread_t thread = thread_create(client_thread, client);
    thread_detach(thread);
    thread_destroy(thread);
}
