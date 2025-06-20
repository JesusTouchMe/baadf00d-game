// Copyright 2025 JesusTouchMe

#ifndef LAUNCH_GAME_INTERFACE_H
#define LAUNCH_GAME_INTERFACE_H 1

#include <infra/module.h>

typedef void (*game_init_t)(void);
typedef void (*game_shutdown_t)(void);
typedef void (*game_update_t)(double dt); // each tick
typedef void (*game_render_t)(double dt); // each frame


struct game_interface {
    module_t module;

    // never null. will fallback to an empty function in loader if they're not found
    game_init_t init;
    game_shutdown_t shutdown;
    game_update_t update;
    game_render_t render;
};

#endif //LAUNCH_GAME_INTERFACE_H