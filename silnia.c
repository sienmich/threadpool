/// Program calculates the factorial of n on a 3 thread pool.

#include <stdio.h>
#include <stdlib.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"

/// Function that given arg[0] = n! and arg[1] = n
/// returns arg such that arg[0] = (n+1)!, arg[1] = n+1
void *calculate(void *arg, size_t argsz, size_t *res_size) {
    ((int *) arg)[0] *= ++((int *) arg)[1];

    *res_size = argsz;
    return arg;
}

int main() {
    int n;
    scanf("%d", &n);

    if (n == 0) {
        printf("1\n");
        return 0;
    }

    thread_pool_t pool;
    try(thread_pool_init(&pool, 3));

    future_t *futures;
    try_ptr(futures = malloc(sizeof(future_t) * n));

/// sil[0] = k!, sil[1] = k. Initially k = 0.
    int fac[2] = {1, 0};

/// Creating first future variable.
    try(async(&pool, &futures[0], (callable_t)
            {.function = calculate, .arg = fac, .argsz = 2 * sizeof(int)}));

/// Ordering calculation of next factorials.
    for (int i = 0; i < n - 1; ++i)
        try(map(&pool, &futures[i + 1], &futures[i], calculate));

    int *result = await(&futures[n - 1]);

    printf("%d\n", result[0]);

    thread_pool_destroy(&pool);
    free(futures);

    return 0;
}