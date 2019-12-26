#include <assert.h>

#include "future.h"
#include "threadpool.h"
#include "runnable.h"
#include "err.h"

void function(void *args, size_t argsz __attribute__((unused))) {
    future_t *future = args;

    try(pthread_mutex_lock(&future->mutex));
    future->res = future->callable.function(future->callable.arg,
                                            future->callable.argsz,
                                            &future->res_size);

    try(pthread_cond_signal(&future->for_res));
    try(pthread_mutex_unlock(&future->mutex));
}

int async(thread_pool_t *pool, future_t *future, callable_t callable) {
    future->res = NULL;
    future->callable = callable;

    try(pthread_mutex_init(&future->mutex, 0));
    try(pthread_cond_init(&future->for_res, 0));


    try(defer(pool, (runnable_t)
            {.arg = future, .argsz = sizeof(future_t *), .function = function}));

    return 0;
}

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *)) {
    try(async(pool, future, (callable_t) {.arg = await(
            from), .argsz = from->res_size, .function = function}));

    return 0;
}

void *await(future_t *future) {
    try(pthread_mutex_lock(&future->mutex));

    while (!future->res)
        try(pthread_cond_wait(&future->for_res, &future->mutex));

    try(pthread_mutex_unlock(&future->mutex));

    assert(future->res);
    return future->res;
}
