#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"

/// Function that sleeps arg[1] ms and returns &arg[0].
void *calculate(void *arg, size_t args __attribute__((unused)), size_t *res_size) {
    *res_size = sizeof(int);

    int ms = ((int *) arg)[1];
    int s = ms / 1000;
    ms %= 1000;

    try(nanosleep((const struct timespec[]){{s, 1000000L * ms}}, NULL));

    return &arg[0];
}

int main() {
    int *matrix;
    future_t *res;
    int k, n;

    scanf("%d%d", &k, &n);

    try_ptr(matrix = malloc(sizeof(int) * 2 * k * n));
    try_ptr(res = malloc(sizeof(future_t) * k * n));

/// Reading matrix.
    for (int i = 0; i < k * n; ++i)
        scanf("%d%d", matrix + 2 * i, matrix + 2 * i + 1);

    thread_pool_t pool;
    try(thread_pool_init(&pool, 4));

/// Ordering n * k matrix calculations on the pool.
    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {
            try(async(&pool, &res[(i * n + j)],
                      (callable_t) {
                              .arg = &matrix[2 * (i * n + j)],
                              .argsz = sizeof(int) * 2,
                              .function = calculate}
            ));
        }
    }

/// Summing rows.
    for (int i = 0; i < k; ++i) {
        int sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += *(int *)await(&res[(i * n + j)]);
        }
        printf("%d\n", sum);
    }

    thread_pool_destroy(&pool);
    free(matrix);
    free(res);

    return 0;
}