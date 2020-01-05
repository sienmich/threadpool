#include <stdlib.h>
#include "future.h"
#include "threadpool.h"
#include "runnable.h"
#include "err.h"

void eval_future(void *args, size_t argsz __attribute__((unused))) {
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
            {.arg = future, .argsz = sizeof(future_t *), .function = eval_future}));

    return 0;
}

struct map_args {
    future_t *from;
    void *(*function)(void *, size_t, size_t *);
};

void* eval_map(void *args, size_t argsz, size_t* ressize) {
    struct map_args a = *(struct map_args *)args;

    void *res = a.function(await(a.from), argsz, ressize);

    free(args);

    return res;
}

int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *)) {

    struct map_args *args;
    args = safe_malloc(sizeof(struct map_args));
    args->from = from;
    args->function = function;


    try(async(pool, future, (callable_t) {.arg = args,
              .argsz = sizeof(&args), .function = eval_map}));

    return 0;
}

void *await(future_t *future) {
    try(pthread_mutex_lock(&future->mutex));

    while (!future->res)
        try(pthread_cond_wait(&future->for_res, &future->mutex));

    try(pthread_mutex_unlock(&future->mutex));

    return future->res;
}
