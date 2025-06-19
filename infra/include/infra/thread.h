// Copyright 2025 JesusTouchMe

#ifndef INFRA_THREAD_H
#define INFRA_THREAD_H 1

#include "infra/compiler_and_platform.h"

typedef struct thread* thread_t;
typedef struct mutex* mutex_t;
typedef struct condition_variable* condition_variable_t;
typedef struct tls_key* tls_key_t;

typedef void* (*thread_func_t)(void*);
typedef void  (*tls_destructor_t)(void*);

// --- Thread creation and management ---

INFRA_API thread_t thread_create(thread_func_t func, void* arg);
INFRA_API void thread_join(thread_t thread);
INFRA_API void thread_detach(thread_t thread);
INFRA_API void thread_destroy(thread_t thread);

// --- Utils ---

INFRA_API void thread_sleep(unsigned int ms);
INFRA_API void thread_yield(void);

// --- Mutex creation and management ---

INFRA_API mutex_t mutex_create(void);
INFRA_API void mutex_destroy(mutex_t mutex);
INFRA_API void mutex_lock(mutex_t mutex);
INFRA_API void mutex_unlock(mutex_t mutex);

// --- Condition variable creation and management

INFRA_API condition_variable_t condition_variable_create(void);
INFRA_API void condition_variable_destroy(condition_variable_t cond);
INFRA_API void condition_variable_wait(condition_variable_t cond, mutex_t mutex);
INFRA_API void condition_variable_notify(condition_variable_t cond);
INFRA_API void condition_variable_notify_all(condition_variable_t cond);

// --- TLS creation and management ---

INFRA_API tls_key_t tls_create(tls_destructor_t destructor);
INFRA_API void tls_destroy(tls_key_t key);
INFRA_API void tls_set(tls_key_t key, void* value);
INFRA_API void* tls_get(tls_key_t key);

#endif //INFRA_THREAD_H