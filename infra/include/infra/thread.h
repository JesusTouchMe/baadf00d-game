// Copyright 2025 JesusTouchMe

#ifndef INFRA_THREAD_H
#define INFRA_THREAD_H 1

typedef struct thread* thread_t;
typedef struct mutex* mutex_t;
typedef struct condition_variable* condition_variable_t;
typedef struct tls_key* tls_key_t;

typedef void* (*thread_func_t)(void*);
typedef void  (*tls_destructor_t)(void*);

// --- Thread creation and management ---

thread_t thread_create(thread_func_t func, void* arg);
void thread_join(thread_t thread);
void thread_detach(thread_t thread);
void thread_destroy(thread_t thread);

// --- Utils ---

void thread_sleep(unsigned int ms);
void thread_yield(void);

// --- Mutex creation and management ---

mutex_t mutex_create(void);
void mutex_destroy(mutex_t mutex);
void mutex_lock(mutex_t mutex);
void mutex_unlock(mutex_t mutex);

// --- Condition variable creation and management

condition_variable_t condition_variable_create(void);
void condition_variable_destroy(condition_variable_t cond);
void condition_variable_wait(condition_variable_t cond, mutex_t mutex);
void condition_variable_notify(condition_variable_t cond);
void condition_variable_notify_all(condition_variable_t cond);

// --- TLS creation and management ---

tls_key_t tls_create(tls_destructor_t destructor);
void tls_destroy(tls_key_t key);
void tls_set(tls_key_t key, void* value);
void* tls_get(tls_key_t key);

#endif //INFRA_THREAD_H