// Copyright 2025 JesusTouchMe

#include "ipc_client.h"

#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {
    if (argc < 2) {
        return 1;
    }

    const char* command_prefix = argv[1];

    char input[IO_BUFFER_SIZE];
    char output[IO_BUFFER_SIZE];

    ipc_client_t* client = ipc_client_open(CONSOLE_CONNECTION_NAME);

    printf("Connected to kernel. TODO: make the ui/ux more interesting\n");

    while (true) {
        printf(command_prefix);

        if (!fgets(input, IO_BUFFER_SIZE, stdin)) {
            printf("EOF received, exiting...\n");
            break;
        }

        size_t len = strlen(input);
        if (len == 0) continue;

        ssize_t written = ipc_client_write(client, input, len);
        if (written < 0) {
            fprintf(stderr, "Error passing data to kernel\n");
            break;
        }

        ssize_t read = ipc_client_read(client, output, IO_BUFFER_SIZE - 1);
        if (read <= 0) {
            fprintf(stderr, "Error reading from kernel or connection was closed\n");
            break;
        }

        output[read] = '\0';

        if (strncmp(output, ".{{terminate}}", 14) == 0) {
            break;
        }

        printf("%s", output);
    }

    ipc_client_close(client);
    return 0;
}
