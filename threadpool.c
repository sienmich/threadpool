#include <pthread.h>
#include <stddef.h>
#include <stdlib.h>

#include "threadpool.h"
#include "queue.h"
#include "err.h"


void *worker(void *data) {
    thread_pool_t *pool = data;
    while (!pool->end) {
        try(pthread_mutex_lock(&pool->mutex));

        while (queue_empty(&pool->tasks) && !pool->end)
            try(pthread_cond_wait(&pool->for_task, &pool->mutex));

        if (pool->end) {
            try(pthread_mutex_unlock(&pool->mutex));
            return 0;
        }

        runnable_t my_task = queue_get(&pool->tasks);

        try(pthread_mutex_unlock(&pool->mutex));

        my_task.function(my_task.arg, my_task.argsz);   //running the task
    }
    return 0;
}

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {

    try(pthread_mutex_init(&pool->mutex, 0));
    try(pthread_cond_init(&pool->for_task, 0));
    try(pthread_attr_init(&pool->attr));

    pool->size = num_threads;
    pool->threads = 0;
    pool->end = false;

    queue_init(&pool->tasks);


    pool->threads = safe_malloc(num_threads * sizeof(pthread_t));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_create(&pool->threads[i], &pool->attr, worker, pool));

    return 0;
}

void thread_pool_destroy(struct thread_pool *pool) {
    pool->end = true;

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_cond_signal(&pool->for_task));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_join(pool->threads[i], NULL));

//    for (size_t i = 0; i < pool->size; ++i)
//        if ((err = pthread_cancel(pool->threads[i])) != 0)
//            syserr (err, "pthread_cancel failed");

    try(pthread_cond_destroy(&pool->for_task));
    try(pthread_mutex_destroy(&pool->mutex));
    try(pthread_attr_destroy(&pool->attr));

    free(pool->threads);
    queue_destroy(&pool->tasks);
}

int defer(struct thread_pool *pool, runnable_t runnable) {
    try(pool->end);

    try(pthread_mutex_lock(&pool->mutex));
    try(queue_add(&pool->tasks, runnable));

    try(pthread_cond_signal(&pool->for_task));
    try(pthread_mutex_unlock(&pool->mutex));

    return 0;
}
