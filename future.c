#include "future.h"
#include "threadpool.h"
#include "runnable.h"

#include <stdlib.h>

typedef void *(*function_t)(void *);


void function(void * args, size_t argsz) {
    future_t *future = args;

    future->res_size = malloc(sizeof(size_t*));

    future->res = future->callable.function(future->callable.arg, future->callable.argsz, future->res_size);


}

int async(thread_pool_t *pool, future_t *future, callable_t callable) {
    future->res = NULL;
    future->callable = callable;

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
    while (!future->res) {

    }
    return future->res;

    return 0;
}
