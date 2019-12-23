#include "threadpool.h"
#include "queue.h"

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
    while (!pool->end) {
        if ((err = pthread_mutex_lock(&pool->mutex)) != 0)
            syserr (err, "lock failed");

        while (queue_empty(&pool->tasks) && !pool->end)
            if ((err = pthread_cond_wait(&pool->for_task, &pool->mutex)) != 0)
                syserr (err, "cond wait failed");

        if (pool->end) {
            if ((err = pthread_mutex_unlock(&pool->mutex)) != 0)
                syserr (err, "unlock failed");
            return 0;
        }


        runnable_t my_task = queue_get(&pool->tasks); // TODO: deep copy

        if ((err = pthread_mutex_unlock(&pool->mutex)) != 0)
            syserr (err, "unlock failed");

        run(&my_task);
    }
    return 0;
}

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {
    int err;

    if ((err = pthread_mutex_init(&pool->mutex, 0) != 0))
        syserr (err, "mutex init failed");
    if ((err = pthread_cond_init(&pool->for_task, 0)) != 0)
        syserr (err, "cond init 1 failed");


    if ((err = pthread_attr_init(&pool->attr)) != 0 )
        syserr(err, "attr_init");


    pool->size = num_threads;
    pool->threads = 0;
    pool->end = false;

    queue_init(&pool->tasks);

    if (!(pool->threads = malloc(num_threads * sizeof(pthread_t))))
        return 1;
    for (size_t i = 0; i < pool->size; ++i) {
        if ((err = pthread_create(&pool->threads[i], &pool->attr, worker, pool)) != 0)
            return 2;
    }

    return 0;
}

void thread_pool_destroy(struct thread_pool *pool) {
    int err;
    pool->end = true;

    for (size_t i = 0; i < pool->size; ++i)
        if ((err = pthread_cond_signal(&pool->for_task)) != 0)
            syserr (err, "cond signal failed");

    for (size_t i = 0; i < pool->size; ++i)
        if ((err = pthread_join(pool->threads[i], NULL)) != 0)
            syserr (err, "pthread_join failed");

//    for (size_t i = 0; i < pool->size; ++i)
//        if ((err = pthread_cancel(pool->threads[i])) != 0)
//            syserr (err, "pthread_cancel failed");

    if ((err = pthread_cond_destroy (&pool->for_task)) != 0)
        syserr (err, "cond destroy 1 failed");
    if ((err = pthread_mutex_destroy (&pool->mutex)) != 0)
        syserr (err, "mutex destroy failed");

    if ((err = pthread_attr_destroy (&pool->attr)) != 0)
        syserr(err, "attr_destroy");

    free(pool->threads);
    queue_destroy(&pool->tasks);
}

int defer(struct thread_pool *pool, runnable_t runnable) {
    int err;

    if ((err = pthread_mutex_lock(&pool->mutex)) != 0)
        syserr (err, "lock failed");

    queue_add(&pool->tasks, runnable);

    if ((err = pthread_cond_signal(&pool->for_task)) != 0)
        syserr (err, "cond signal failed");

    if ((err = pthread_mutex_unlock(&pool->mutex)) != 0)
        syserr (err, "unlock failed");

    return 0;
}
