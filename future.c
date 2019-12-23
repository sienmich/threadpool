#include "future.h"
#include "threadpool.h"
#include "runnable.h"
#include "err.h"

#include <stdlib.h>
#include <assert.h>

typedef void *(*function_t)(void *);


void function(void * args, size_t argsz __attribute__((unused))) {
    int err;

    future_t *future = args;

    future->res_size = malloc(sizeof(size_t*));

    future->res = future->callable.function(future->callable.arg, future->callable.argsz, future->res_size);

    if ((err = pthread_cond_signal(&future->for_res)) != 0)
        syserr (err, "cond signal failed");
}

int async(thread_pool_t *pool, future_t *future, callable_t callable) {
    int err;
    future->res = NULL;
    future->callable = callable;

    if ((err = pthread_mutex_init(&future->mutex, 0) != 0))
        syserr (err, "mutex init failed");
    if ((err = pthread_cond_init(&future->for_res, 0)) != 0)
        syserr (err, "cond init failed");

    runnable_t runnable;

    runnable.arg = future;
    runnable.argsz = sizeof(future_t *);
    runnable.function = function;

    defer(pool, runnable);

    return 0;
}

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *)) {

    callable_t callable;
    callable.arg = await(from);
    callable.argsz = *from->res_size;
    callable.function = function;

    async(pool, future, callable);

    return 0;
}

void *await(future_t *future) {
    int err;

    if ((err = pthread_mutex_lock(&future->mutex)) != 0)
        syserr (err, "lock failed");

    while (!future->res)
        if ((err = pthread_cond_wait(&future->for_res, &future->mutex)) != 0)
            syserr (err, "cond wait failed");

    if ((err = pthread_mutex_unlock(&future->mutex)) != 0)
        syserr (err, "unlock failed");

    assert(future->res);

    return future->res;
}
