// Copyright 2025 JesusTouchMe

#ifndef INFRA_PROTOCOL_IPC_PACKET_H
#define INFRA_PROTOCOL_IPC_PACKET_H 1

#include <stdint.h>
#include <stdio.h>

// --- Header. Present in every packet ---

enum ipc_packet_type {
    IPC_PACKET_HANDSHAKE = 0,
    IPC_PACKET_CMD = 1,
    IPC_PACKET_CMD_RES = 2,
    IPC_PACKET_EVENT = 3,
};

struct ipc_packet_header {
    uint32_t type;
    uint32_t id; // event id or command id for send/res
    uint32_t body_length;
};

struct ipc_base_packet {
    struct ipc_packet_header header;
    char body[];
};

// --- Specific packet structs ---

//...

// --- Specific packet functions ---

void ipc_packet_handshake_init(struct ipc_base_packet* packet);

// --- Abstract read/write ---

typedef ssize_t (*ipc_packer_read_t)(void* socket_handle, void* buffer, size_t count);
typedef ssize_t (*ipc_packer_write_t)(void* socket_handle, const void* buffer, size_t count);

// Callback functions are called once to get the data so read- or write_all or some variation is recommended
struct ipc_base_packet* ipc_packet_read(void* socket_handle, ipc_packer_read_t read_cb);
ssize_t ipc_packet_write(struct ipc_base_packet* packet, void* socket_handle, ipc_packer_write_t write_cb);

#endif // INFRA_PROTOCOL_IPC_PACKET_H
