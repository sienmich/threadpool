#include <stdlib.h>

#include "threadpool.h"
#include "err.h"
#include "vector.h"

#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <memory.h>

// TODO: komentarze
// TODO: obsługa errorów
// TODO: P1
// TODO: P7
// TODO: P11
// TODO: P12
// TODO: P22
// TODO: P23
// TODO: P24
// TODO: P26

static int received_SIGINT = 0;

static void *worker(void *data) {
    thread_pool_t *pool = data;
    while (1) {
        try(pthread_mutex_lock(&pool->mutex));

        while (queue_empty(&pool->tasks)) {
            if (received_SIGINT || pool->end) {
                try(pthread_mutex_unlock(&pool->mutex));
                return 0;
            } else
                try(pthread_cond_wait(&pool->for_task, &pool->mutex));
        }

        runnable_t my_task = queue_get(&pool->tasks);

        try(pthread_mutex_unlock(&pool->mutex));

        my_task.function(my_task.arg, my_task.argsz);   //running the task
    }
    return 0;
}

Vector pools;

void process_SIGINT(int signo __attribute__((unused))) {
    received_SIGINT = 1;

    while (pools.size) {
        thread_pool_t *pool = pools.data[0];
        thread_pool_destroy(pool);
    }
}


static bool inited = false;

static void setup_signal() {
    sigset_t block_mask;
    sigemptyset(&block_mask);
    sigaddset(&block_mask, SIGINT);

    struct sigaction action = {
            .sa_handler = process_SIGINT,
            .sa_mask = block_mask,
            .sa_flags = 0};

    try(sigaction(SIGINT, &action, 0));
    inited = true;
}

int thread_pool_init(thread_pool_t *pool, size_t num_threads) {
    if (!inited)
        setup_signal();

    try(pthread_mutex_init(&pool->mutex, 0));
    try(pthread_cond_init(&pool->for_task, 0));
    try(pthread_attr_init(&pool->attr));

    pool->size = num_threads;
    pool->threads = 0;
    pool->end = 0;

    queue_init(&pool->tasks);


    pool->threads = safe_malloc(num_threads * sizeof(pthread_t));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_create(&pool->threads[i], &pool->attr, worker, pool));

    try(pushBack(&pools, pool));

    return 0;
}

void thread_pool_destroy(struct thread_pool *pool) {
    pool->end = 1;
    deleteElementFromVector(&pools, pool);

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_cond_signal(&pool->for_task));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_join(pool->threads[i], NULL));

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
