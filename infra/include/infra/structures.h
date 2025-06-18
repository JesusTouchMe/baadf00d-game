// Copyright 2025 JesusTouchMe

#ifndef INFRA_STRUCTURES_H
#define INFRA_STRUCTURES_H 1

#include <stddef.h>

struct array_of_strings {
    const char* const* strings;
    size_t length;
};

struct array_of_string_pairs {
    const char* const* keys;
    const char* const* values;
    size_t length;
};

#endif //INFRA_STRUCTURES_H