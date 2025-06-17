// Copyright 2025 JesusTouchMe

#include "infra/ipc.h"

#include <stdlib.h>
#include <string.h>

char ipc_last_error_message[512];

void ipc_set_last_error_message(const char* error_message) {
    strncpy(ipc_last_error_message, error_message, sizeof(ipc_last_error_message));
}

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct ipc_server {
    char name[256];
};

struct ipc_client {
    HANDLE pipe;
};

static void full_name(const char* name, char* dest, size_t dest_len) {
    snprintf(dest, dest_len, "\\\\.\\pipe\\%s", name);
}

void ipc_set_os_error_message(const char* error_message, DWORD error_code) {
    CHAR message[512];
    DWORD message_length = FormatMessageA(
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error_code,
        0,
        message,
        512, nullptr
    );

    if (message_length == 0) {
        HINSTANCE inst = LoadLibraryA("Ntdsbmsg.dll");
        if (inst == nullptr) {
            strncpy(message, "no error message found and could not load Ntdsbmsg.dll", 512);
            message_length = strlen(message);
        } else {
            message_length = FormatMessageA(
                FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS,
                inst,
                error_code,
                0,
                message,
                512, nullptr
            );

            FreeLibrary(inst);
        }
    }

    if (message_length == 0) {
        strncpy(message, "no error message", 512);
    }

    snprintf(ipc_last_error_message, sizeof(ipc_last_error_message), "%s. GLE=%X (%s)", error_message, error_code, message);
}

ipc_server_t ipc_server_create(const char* name) {
    ipc_server_t server = malloc(sizeof(struct ipc_server));
    if (server == nullptr) {
        ipc_set_last_error_message("Memory allocation failed");
        return nullptr;
    }

    full_name(name, server->name, sizeof(server->name));

    return server;
}

bool ipc_server_listen(ipc_server_t server) {
    return true; // return success
}

ipc_client_t ipc_server_accept(ipc_server_t server) {
    HANDLE pipe = CreateNamedPipeA(
        server->name,
        PIPE_ACCESS_DUPLEX,
        PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES,
        4096, 4096, 0, nullptr
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        ipc_set_os_error_message("CreateNamedPipe failed", GetLastError());
        return nullptr;
    }

    if (!ConnectNamedPipe(pipe, nullptr) && GetLastError() != ERROR_PIPE_CONNECTED) {
        ipc_set_os_error_message("ConnectNamedPipe failed", GetLastError());
        CloseHandle(pipe);
        return nullptr;
    }

    ipc_client_t client = malloc(sizeof(struct ipc_client));
    client->pipe = pipe;
    return client;
}

void ipc_server_close(ipc_server_t server) {
    free(server);
}

ipc_client_t ipc_client_connect(const char* name) {
    char endpoint[512];
    full_name(name, endpoint, sizeof(endpoint));

    HANDLE pipe = CreateFileA(
        endpoint,
        GENERIC_READ | GENERIC_WRITE,
        0, nullptr, OPEN_EXISTING, 0, nullptr
    );

    if (pipe == INVALID_HANDLE_VALUE) {
        ipc_set_os_error_message("CreateFile failed", GetLastError());
        return nullptr;
    }

    ipc_client_t client = malloc(sizeof(struct ipc_client));
    client->pipe = pipe;
    return client;
}

void ipc_client_close(ipc_client_t client) {
    CloseHandle(client->pipe);
    free(client);
}

ssize_t ipc_read(ipc_client_t client, void* buffer, size_t count) {
    DWORD bytes_read;
    if (!ReadFile(client->pipe, buffer, (DWORD) count, &bytes_read, nullptr)) {
        return -1;
    }

    return (ssize_t) bytes_read;
}

ssize_t ipc_write(ipc_client_t client, const void* buffer, size_t count) {
    DWORD bytes_written;
    if (!WriteFile(client->pipe, buffer, (DWORD) count, &bytes_written, nullptr)) {
        return -1;
    }

    return (ssize_t) bytes_written;
}

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

struct ipc_server {
    int fd;
    char path[108];
};

struct ipc_client {
    int fd;
};

static void full_name(const char* name, char* dest, size_t dest_len) {
    snprintf(dest, dest_len, "/tmp/%s.sock", name);
}

void ipc_set_os_error_message(const char* error_message, int error_code) {
    snprintf(ipc_last_error_message, sizeof(ipc_last_error_message), "%s. errno=%X (%s)", error_message, error_code, strerror(error_code));
}

ipc_server_t ipc_server_create(const char* name) {
    ipc_server_t server = malloc(sizeof(struct ipc_server));
    if (server == nullptr) {
        ipc_set_last_error_message("Memory allocation failed");
        return nullptr;
    }

    full_name(name, server->path, sizeof(server->path));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        ipc_set_os_error_message("socket() failed", errno);
        free(server);
        return nullptr;
    }

    server->fd = fd;

    return server;
}

bool ipc_server_listen(ipc_server_t server) {
    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, server->path, sizeof(addr.sun_path) - 1);

    unlink(server->path);

    if (bind(server->fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        ipc_set_os_error_message("bind() failed", errno);
        return false;
    }

    if (listen(server->fd, 16) < 0) {
        ipc_set_os_error_message("listen() failed", errno);
        return false;
    }

    return true;
}

ipc_client_t ipc_server_accept(ipc_server_t server) {
    int client_fd = accept(server->fd, nullptr, nullptr);
    if (client_fd < 0) {
        ipc_set_os_error_message("accept() failed", errno);
        return nullptr;
    }

    ipc_client_t client = malloc(sizeof(struct ipc_client));
    client->fd = client_fd;
    return client;
}

void ipc_server_close(ipc_server_t server) {
    close(server->fd);
    unlink(server->path);
    free(server);
}

ipc_client_t ipc_client_connect(const char* name) {
    char path[108];
    full_name(name, path, sizeof(path));

    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        ipc_set_os_error_message("socket() failed", errno);
        return nullptr;
    }

    struct sockaddr_un addr = {0};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);

    if (connect(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        ipc_set_os_error_message("connect() failed", errno);
        close(fd);
        return nullptr;
    }

    ipc_client_t client = malloc(sizeof(struct ipc_client));
    client->fd = fd;
    return client;
}

void ipc_client_close(ipc_client_t client) {
    close(client->fd);
    free(client);
}

ssize_t ipc_read(ipc_client_t client, void* buffer, size_t count) {
    return read(client->fd, buffer, count);
}

ssize_t ipc_write(ipc_client_t client, const void* buffer, size_t count) {
    return write(client->fd, buffer, count);
}

#else
#error "Unsupported platform"
#endif

// platform independent code

ssize_t ipc_read_all(ipc_client_t server, void* buffer, size_t count) {
    size_t offset = 0;
    while (offset < count) {
        ssize_t bytes_read = ipc_read(server, (char*) buffer + offset, count - offset);
        if (bytes_read <= 0) {
            return -1;
        }

        offset += bytes_read;
    }

    return offset;
}

ssize_t ipc_write_all(ipc_client_t server, const void* buffer, size_t count) {
    size_t offset = 0;
    while (offset < count) {
        ssize_t bytes_read = ipc_write(server, (const char*) buffer + offset, count - offset);
        if (bytes_read <= 0) {
            return -1;
        }

        offset += bytes_read;
    }

    return offset;
}

const char* ipc_last_error(void) {
    return ipc_last_error_message;
}
