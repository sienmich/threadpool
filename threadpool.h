#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <stddef.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>


typedef struct runnable {
  void (*function)(void *, size_t);
  void *arg;
  size_t argsz;
} runnable_t;

typedef struct thread_pool {
    size_t size;
    pthread_t *threads;
    pthread_mutex_t mutex;
    pthread_cond_t for_task;

    int task_count;
    runnable_t *tasks;

} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, size_t pool_size);

void thread_pool_destroy(thread_pool_t *pool);

int defer(thread_pool_t *pool, runnable_t runnable);

#endif
