#include "threadpool.h"

#include <pthread.h>
#include <semaphore.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "err.h"

void run (runnable_t *task) {
    task->function(task->arg, task->argsz);
}

void *worker(void *data) {
    thread_pool_t *pool = data;
    int err;
    while (true) {
        if ((err = pthread_mutex_lock(&pool->mutex)) != 0)
            syserr (err, "lock failed");

        while (pool->task_count == 0)
            if ((err = pthread_cond_wait(&pool->for_task, &pool->mutex)) != 0)
                syserr (err, "cond wait failed");


        pool->task_count--;
        runnable_t my_task = pool->tasks[pool->task_count]; // TODO: deep copy

        if ((err = pthread_mutex_unlock(&pool->mutex)) != 0)
            syserr (err, "unlock failed");

        run(&my_task);
    }
}

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {
    int err;

    if ((err = pthread_mutex_init(&pool->mutex, 0) != 0))
        syserr (err, "mutex init failed");
    if ((err = pthread_cond_init(&pool->for_task, 0)) != 0)
        syserr (err, "cond init 1 failed");

    pool->task_count = 0;


    pthread_attr_t attr;
    pool->size = num_threads;
    if (!pool->threads = malloc(num_threads * sizeof(pthread_t)))
        return 1;
    for (size_t i = 0; i < pool->size; ++i) {
        if ((err = pthread_create(&pool->threads[i], &attr, worker, pool)) != 0)
            return 2;
    }

    return 0;
}

void thread_pool_destroy(struct thread_pool *pool) {
    int err;

    for (size_t i = 0; i < pool->size; ++i) {
        if ((err = pthread_cancel(pool->threads[i])) != 0)
            syserr (err, "pthread_cancel failed");
    }

    if ((err = pthread_cond_destroy (&pool->for_task)) != 0)
        syserr (err, "cond destroy 1 failed");
    if ((err = pthread_mutex_destroy (&pool->mutex)) != 0)
        syserr (err, "mutex destroy failed");

    free(pool->threads);
    free(pool->tasks);

    //TODO: destroy deep copies?
}

int defer(struct thread_pool *pool, runnable_t runnable) {
    int err;

    if ((err = pthread_mutex_lock(&pool->mutex)) != 0)
        syserr (err, "lock failed");
    pool->tasks[pool->task_count++] = runnable; //TODO: malloc?

    return 0;
}
