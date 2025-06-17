// Copyright 2025 JesusTouchMe

#include "infra/protocols/ipc_packet.h"

#include <stdlib.h>

void ipc_packet_handshake_init(struct ipc_base_packet* packet) {
    packet->header.type = IPC_PACKET_HANDSHAKE;
    packet->header.id = 0;
    packet->header.body_length = 0;
}

struct ipc_base_packet* ipc_packet_read(void* socket_handle, ipc_packer_read_t read_cb) {
    struct ipc_packet_header header;

    ssize_t res = read_cb(socket_handle, &header, sizeof(header));
    if (res != sizeof(header)) {
        return nullptr;
    }

    struct ipc_base_packet* packet = malloc(sizeof(struct ipc_base_packet) + header.body_length);
    if (packet == nullptr) {
        return nullptr;
    }

    packet->header = header;

    res = read_cb(socket_handle, packet->body, header.body_length);
    if (res != header.body_length) {
        free(packet);
        return nullptr;
    }

    return packet;
}

ssize_t ipc_packet_write(struct ipc_base_packet* packet, void* socket_handle, ipc_packer_write_t write_cb) {
    ssize_t res = write_cb(socket_handle, &packet->header, sizeof(packet->header));
    if (res != sizeof(packet->header)) {
        return -1;
    }

    ssize_t body_res = write_cb(socket_handle, packet->body, packet->header.body_length);
    if (body_res != packet->header.body_length) {
        return -1;
    }

    return res + body_res;
}
