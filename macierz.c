#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"

void *calculate(void *arg, size_t args __attribute__((unused)), size_t *res_size) {
    *res_size = sizeof(int);

    usleep(1000 * ((int *) arg)[1]);

    return arg;
}

int main() {
    int *matrix;
    future_t *res;
    int k, n;

    scanf("%d%d", &k, &n);

    matrix = safe_malloc(sizeof(int) * 2 * k * n);
    res = safe_malloc(sizeof(future_t) * k * n);

    for (int i = 0; i < k * n; ++i) {
        scanf("%d%d", matrix + 2 * i, matrix + 2 * i + 1);
    }

    thread_pool_t pool;
    try(thread_pool_init(&pool, 4));


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

    for (int i = 0; i < k; ++i) {
        int sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += *(int *)await(&res[(i * n + j)]);
        }
        printf("%d\n", sum);
    }

    return 0;
}