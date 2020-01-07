#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"
#include "err.h"

int *sums;
pthread_mutex_t *mutex;

/// Function that sleeps arg[1] ms and increments sums[arg[2]] by arg[0].
void calculate(void *arg, size_t args __attribute__((unused))) {
    int *a = arg;
    int ms = a[1];
    int s = ms / 1000;
    ms %= 1000;

    try(nanosleep((const struct timespec[]){{s, 1000000L * ms}}, NULL));

    try(pthread_mutex_lock(&mutex[a[2]]));

    sums[a[2]] += a[0];

    try(pthread_mutex_unlock(&mutex[a[2]]));
}

int main() {
    int *matrix;
    int k, n;

    scanf("%d%d", &k, &n);

    try_ptr(matrix = malloc(sizeof(int) * 3 * k * n));
    try_ptr(sums = malloc(sizeof(int) * k));
    try_ptr(mutex = malloc(sizeof(pthread_mutex_t) * k));

    for (int i = 0; i < k; ++i) {
        sums[i] = 0;
        try(pthread_mutex_init(&mutex[i], 0));    
    }

/// Reading matrix.
    for (int i = 0; i < k * n; ++i)
        scanf("%d%d", matrix + 3 * i, matrix + 3 * i + 1);

    thread_pool_t pool;
    try(thread_pool_init(&pool, 4));

/// Ordering n * k matrix calculations on the pool.
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[3 * (i * n + j) + 2] = i;
            try(defer(&pool, (runnable_t) {
                              .arg = &matrix[3 * (i * n + j)],
                              .argsz = sizeof(int) * 3,
                              .function = calculate}
            ));
        }
    }

    thread_pool_destroy(&pool);

/// Printing sums.
    for (int i = 0; i < k; ++i) {
        printf("%d\n", sums[i]);
    }

    for (int i = 0; i < k; ++i) {
        try(pthread_mutex_destroy(&mutex[i]));    
    }
    
    free(matrix);
    free(sums);
    free(mutex);

    return 0;
}
