// Copyright 2025 JesusTouchMe

#include "infra/thread.h"

#include <stdlib.h>

#ifdef PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

struct thread {
    HANDLE handle;
    bool detached;
};

struct mutex {
    CRITICAL_SECTION cs;
};

struct condition_variable {
    CONDITION_VARIABLE cv;
};

struct tls_key {
    DWORD key;
    tls_destructor_t destructor;
};

thread_t thread_create(thread_func_t func, void* arg) {
    HANDLE handle = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) func, arg, 0, nullptr);
    if (handle == nullptr) {
        return nullptr;
    }

    thread_t thread = malloc(sizeof(struct thread));
    if (thread == nullptr) {
        CloseHandle(handle);
        return nullptr;
    }

    thread->handle = handle;
    thread->detached = false;

    return thread;
}

void thread_join(thread_t thread) {
    if (thread != nullptr && !thread->detached && thread->handle != nullptr) {
        WaitForSingleObject(thread->handle, INFINITE);
    }
}

void thread_detach(thread_t thread) {
    if (thread != nullptr && thread->handle != nullptr) {
        CloseHandle(thread->handle);
        thread->handle = nullptr;
        thread->detached = true;
    }
}

void thread_destroy(thread_t thread) {
    if (thread == nullptr) return;
    if (thread->handle != nullptr && !thread->detached) {
        CloseHandle(thread->handle);
    }
    free(thread);
}

void thread_sleep(unsigned int ms) {
    Sleep(ms);
}

void thread_yield(void) {
    SwitchToThread();
}

mutex_t mutex_create(void) {
    mutex_t mutex = malloc(sizeof(struct mutex));
    if (mutex == nullptr) {
        return nullptr;
    }

    InitializeCriticalSection(&mutex->cs);

    return mutex;
}

void mutex_destroy(mutex_t mutex) {
    if (mutex == nullptr) return;
    DeleteCriticalSection(&mutex->cs);
    free(mutex);
}

void mutex_lock(mutex_t mutex) {
    EnterCriticalSection(&mutex->cs);
}

void mutex_unlock(mutex_t mutex) {
    LeaveCriticalSection(&mutex->cs);
}

condition_variable_t condition_variable_create(void) {
    condition_variable_t cond = malloc(sizeof(struct condition_variable));
    if (cond == nullptr) {
        return nullptr;
    }

    InitializeConditionVariable(&cond->cv);

    return cond;
}

void condition_variable_destroy(condition_variable_t cond) {
    free(cond);
}

void condition_variable_wait(condition_variable_t cond, mutex_t mutex) {
    SleepConditionVariableCS(&cond->cv, &mutex->cs, INFINITE);
}

void condition_variable_notify(condition_variable_t cond) {
    WakeConditionVariable(&cond->cv);
}

void condition_variable_notify_all(condition_variable_t cond) {
    WakeAllConditionVariable(&cond->cv);
}

tls_key_t tls_create(tls_destructor_t destructor) {
    DWORD key = TlsAlloc();
    if (key == TLS_OUT_OF_INDEXES) return nullptr;

    tls_key_t tls_key = malloc(sizeof(struct tls_key));
    if (tls_key == nullptr) {
        TlsFree(key);
        return nullptr;
    }

    tls_key->key = key;
    tls_key->destructor = destructor;

    return tls_key;
}

void tls_destroy(tls_key_t key) {
    if (key == nullptr) return;
    TlsFree(key->key);
    free(key);
}

void tls_set(tls_key_t key, void* value) {
    TlsSetValue(key->key, value);
}

void* tls_get(tls_key_t key) {
    return TlsGetValue(key->key);
}

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)

#include <pthread.h>
#include <unistd.h>
#include <sched.h>

struct thread {
    pthread_t handle;
    bool detached;
};

struct mutex {
    pthread_mutex_t mutex;
};

struct condition_variable {
    pthread_cond_t cv;
};

struct tls_key {
    pthread_key_t key;
};

thread_t thread_create(thread_func_t func, void* arg) {
    pthread_t handle;
    if (pthread_create(&handle, nullptr, (void*(*)(void*)) func, arg) != 0) {
        return nullptr;
    }

    thread_t thread = malloc(sizeof(struct thread));
    if (thread == nullptr) {
        return nullptr;
    }

    thread->handle = handle;
    thread->detached = false;

    return thread;
}

void thread_join(thread_t thread) {
    if (thread != nullptr && !thread->detached) {
        pthread_join(thread->handle, nullptr);
    }
}

void thread_detach(thread_t thread) {
    if (thread != nullptr && !thread->detached) {
        pthread_detach(thread->handle);
        thread->detached = true;
    }
}

void thread_destroy(thread_t thread) {
    free(thread);
}

void thread_sleep(unsigned int ms) {
    usleep(ms * 1000);
}

void thread_yield(void) {
    sched_yield();
}

mutex_t mutex_create(void) {
    mutex_t mutex = malloc(sizeof(struct mutex));
    if (mutex == nullptr) return nullptr;

    pthread_mutex_init(&mutex->mutex, nullptr);

    return mutex;
}

void mutex_destroy(mutex_t mutex) {
    if (mutex == nullptr) return;
    pthread_mutex_destroy(&mutex->mutex);
    free(mutex);
}

void mutex_lock(mutex_t mutex) {
    pthread_mutex_lock(&mutex->mutex);
}

void mutex_unlock(mutex_t mutex) {
    pthread_mutex_unlock(&mutex->mutex);
}

condition_variable_t condition_variable_create(void) {
    condition_variable_t cond = malloc(sizeof(struct condition_variable));
    if (cond == nullptr) return nullptr;

    pthread_cond_init(&cond->cv, nullptr);

    return cond;
}

void condition_variable_destroy(condition_variable_t cond) {
    if (cond == nullptr) return;
    pthread_cond_destroy(&cond->cv);
    free(cond);
}

void condition_variable_wait(condition_variable_t cond, mutex_t mutex) {
    pthread_cond_wait(&cond->cv, &mutex->mutex);
}

void condition_variable_notify(condition_variable_t cond) {
    pthread_cond_signal(&cond->cv);
}

void condition_variable_notify_all(condition_variable_t cond) {
    pthread_cond_broadcast(&cond->cv);
}

tls_key_t tls_create(tls_destructor_t destructor) {
    pthread_key_t key_handle;
    if (pthread_key_create(&key_handle, destructor) != 0) {
        return nullptr;
    }

    tls_key_t key = malloc(sizeof(struct tls_key));
    if (key == nullptr) return nullptr;

    key->key = key_handle;

    return key;
}

void tls_destroy(tls_key_t key) {
    if (key == nullptr) return;
    pthread_key_delete(key->key);
    free(key);
}

void tls_set(tls_key_t key, void* value) {
    pthread_setspecific(key->key, value);
}

void* tls_get(tls_key_t key) {
    return pthread_getspecific(key->key);
}

#else
#error "Unsupported platform"
#endif