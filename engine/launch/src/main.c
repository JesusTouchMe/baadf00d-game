// Copyright 2025 JesusTouchMe

#include <infra/module.h>

int main(int argc, char** argv) {
    module_t engine_module = module_load("engine");
    if (engine_module == nullptr) {
        return 1;
    }


}