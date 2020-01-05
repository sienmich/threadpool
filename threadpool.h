#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <pthread.h>
#include "queue.h"

/// Struct with pool of threads.
typedef struct thread_pool {
    size_t size; ///< Size of the pool.
    pthread_t *threads; ///< Array of threads.
    pthread_attr_t attr; ///< Attributes of each thread.
    pthread_mutex_t mutex;
    pthread_cond_t for_task; ///< Condition to wait for new tasks.
    queue_t tasks; ///< Queue of tasks to run.
    bool end; ///< If thread was already destroyed.
} thread_pool_t;

/// Initializes a new pool.
int thread_pool_init(thread_pool_t *pool, size_t pool_size);

/// Destroys a pool.
void thread_pool_destroy(thread_pool_t *pool);

/// Orders a task to run on a pool.
int defer(thread_pool_t *pool, runnable_t runnable);

#endif
