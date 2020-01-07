#include <stdlib.h>
#include <signal.h>

#include "threadpool.h"
#include "err.h"
#include "vector.h"

/// Variable checking if SIGINT was received.
static int received_SIGINT = 0;

/// Vector of all pools.
static vector_t pools;

/// Bool checking if signal handling was already inited.
static bool inited = false;

/// Function that destroys all pools.
static void process_SIGINT(int signo __attribute__((unused))) {
    received_SIGINT = 1;
    while (pools.size) {
        thread_pool_t *pool = pools.data[0];
        thread_pool_destroy(pool);
    }
}

/// Sets up the signal handling.
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

/// Function that each thread from the pool is running.
/// It waits for tasks and completes them.
static void *worker(void *data) {
    thread_pool_t *pool = data;
    while (1) {
        try(pthread_mutex_lock(&pool->mutex));

        while (queue_empty(&pool->tasks)) {
            if (received_SIGINT || pool->end) {
                try(pthread_mutex_unlock(&pool->mutex));
                return 0;
            } else {
                try(pthread_cond_wait(&pool->for_task, &pool->mutex));
            }
        }

        runnable_t my_task = queue_get(&pool->tasks);

        try(pthread_mutex_unlock(&pool->mutex));

        my_task.function(my_task.arg, my_task.argsz);   //running the task
    }
}

/// Initializes a new pool.
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


    try_ptr(pool->threads = malloc(num_threads * sizeof(pthread_t)));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_create(&pool->threads[i], &pool->attr, worker, pool));

    try(push_back(&pools, pool));

    return 0;
}

/// Destroys a pool.
void thread_pool_destroy(struct thread_pool *pool) {
    try(pthread_mutex_lock(&pool->mutex));
    pool->end = 1;
    try(pthread_mutex_unlock(&pool->mutex));

    delete_element_from_vector(&pools, pool);

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_cond_signal(&pool->for_task));

    for (size_t i = 0; i < pool->size; ++i)
        try(pthread_join(pool->threads[i], NULL));

    try(pthread_cond_destroy(&pool->for_task));
    try(pthread_mutex_destroy(&pool->mutex));
    try(pthread_attr_destroy(&pool->attr));

    free(pool->threads);
}

/// Orders a task to run on a pool.
int defer(struct thread_pool *pool, runnable_t runnable) {
    try(pool->end);

    try(pthread_mutex_lock(&pool->mutex));
    try(queue_add(&pool->tasks, runnable));

    try(pthread_cond_signal(&pool->for_task));
    try(pthread_mutex_unlock(&pool->mutex));

    return 0;
}
