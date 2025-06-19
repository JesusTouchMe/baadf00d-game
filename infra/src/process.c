// Copyright 2025 JesusTouchMe

#include "infra/process.h"

#include <stdlib.h>

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct process {
    PROCESS_INFORMATION pi;
};

static char* build_command_line(const char* path, struct array_of_strings args) {
    size_t length = strlen(path) + 1;
    for (size_t i = 0; i < args.length; i++) {
        length += strlen(args.strings[i]) + 3;
    }

    char* command = malloc(length + 1);
    if (command == nullptr) return nullptr;

    memset(command, 0, length);

    strcpy(command, path);
    for (size_t i = 0; i < args.length; i++) {
        strcat(command, " \"");
        strcat(command, args.strings[i]);
        strcat(command, "\"");
    }

    return command;
}

static void* build_environment(struct array_of_string_pairs env) {
    if (env.keys == nullptr || env.values == nullptr) return nullptr;

    size_t length = 0;
    for (size_t i = 0; i < env.length; i++) {
        length += strlen(env.keys[i]) + strlen(env.values[i]) + 2;
    }

    char* env_block = malloc(length + 1);
    if (env_block == nullptr) return nullptr;

    memset(env_block, 0, length);

    for (size_t i = 0; i < env.length; i++) {
        strcat(env_block, env.keys[i]);
        strcat(env_block, "=");
        strcat(env_block, env.values[i]);
        strcat(env_block, "\0");
    }

    return env_block;
}

process_t process_create(const char* name, const char* cwd, struct array_of_strings args, struct array_of_string_pairs env, bool new_console) {
    STARTUPINFOA si = { sizeof(si) };
    DWORD flags = new_console ? CREATE_NEW_CONSOLE : 0;

    char* command = build_command_line(name, args);
    if (command == nullptr) return nullptr;

    void* env_block = build_environment(env);
    if (env_block == nullptr) {
        free(command);
        return nullptr;
    }

    process_t process = malloc(sizeof(struct process));
    if (process == nullptr) {
        free(command);
        free(env_block);
        return nullptr;
    }

    BOOL success = CreateProcessA(
        nullptr,
        command,
        nullptr,
        nullptr,
        false,
        flags,
        env_block,
        cwd,
        &si,
        &process->pi
    );

    free(command);
    free(env_block);

    if (!success) {
        free(process);
        return nullptr;
    }

    return process;
}

bool process_is_running(process_t process) {
    DWORD code = 0;
    GetExitCodeProcess(process->pi.hProcess, &code);
    return code == STILL_ACTIVE;
}

int process_wait(process_t process) {
    WaitForSingleObject(process->pi.hProcess, INFINITE);
    DWORD exit_code = 0;
    GetExitCodeProcess(process->pi.hProcess, &exit_code);
    return (int) exit_code;
}

void process_terminate(process_t process) {
    TerminateProcess(process->pi.hProcess, 1);
}

void process_destroy(process_t process) {
    if (process == nullptr) return;
    CloseHandle(process->pi.hProcess);
    CloseHandle(process->pi.hThread);
    free(process);
}

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>

struct process {
    pid_t pid;
};

process_t process_create(const char* name, const char* cwd, struct array_of_strings args, struct array_of_strings env, bool new_console) {
    process_t process = malloc(sizeof(struct process));
    if (process == nullptr) return nullptr;

    pid_t pid = fork();
    if (pid < 0) return nullptr;

    if (pid == 0) {
        if (cwd != nullptr) chdir(cwd);
        if (new_console) setsid();

        const char** new_args = malloc(sizeof(char*) * (args.length + 2));
        new_args[0] = name;
        for (size_t i = 0; i < args.length; i++) {
            new_args[i + 1] = args.strings[i];
        }
        new_args[args.length + 1] = nullptr;

        execvp(name, new_args);

        _exit(127);
    }

    process->pid = pid;
    return process;
}

bool process_is_running(process_t process) {
    return kill(process->pid, 0) == 0;
}

int process_wait(process_t process) {
    int status = 0;
    waitpid(process->pid, &status, 0);
    if (WIFEXITED(status)) return WEXITSTATUS(status);
    return -1;
}

void process_terminate(process_t process) {
    kill(process->pid, SIGTERM);
}

void process_destroy(process_t process) {
    free(process);
}

#else
#error "Unsupported platform"
#endif