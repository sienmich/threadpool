#ifndef FUTURE_H
#define FUTURE_H

#include "threadpool.h"

typedef struct callable {
  void *(*function)(void *, size_t, size_t *);
  void *arg;
  size_t argsz;
} callable_t;

typedef struct future {
    callable_t callable;
    void *res;
    size_t res_size;
    pthread_mutex_t mutex;
    pthread_cond_t for_res;
} future_t;

/// Orders a callable task to be calculated on pool and the result will be
/// stored in the future variable. User is responsible for memory management.
int async(thread_pool_t *pool, future_t *future, callable_t callable);

/// Orders a function to be calculated on pool from a future value.
/// The result will be stored in another future variable.
/// User is responsible for memory management.
int map(thread_pool_t *pool, future_t *future, future_t *from,
        void *(*function)(void *, size_t, size_t *));

/// Waits for a future variable to be calculated and returns it.
void *await(future_t *future);

#endif
