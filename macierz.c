#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"

int *matrix;
future_t *res;

void *calculate(void * arg, size_t args, size_t *res_size) {
    *res_size = sizeof(int);

    usleep(((int*)arg)[1]);

    return arg;
}

int main() {
    int k, n;
    scanf("%d%d", &k, &n);
    if (!(matrix = malloc(sizeof(int) * 2 * k * n)))
        syserr(1, "malloc failed");

    if (!(res = malloc(sizeof(future_t) * k * n)))
        syserr(1, "malloc failed");

    for (int i = 0; i < k * n; ++i) {
        scanf("%d%d", matrix + 2 * i, matrix + 2 * i + 1);
    }

    thread_pool_t pool;
    thread_pool_init(&pool, 4);

    for (int i = 0; i < k; ++i) {
        for (int j = 0; j < n; ++j) {

            callable_t callable;
            callable.arg = matrix + 2 * (i * n + j);
            callable.argsz = sizeof(int) * 2;
            callable.function = calculate;

            async(&pool, &res[(i * n + j)], callable);
        }
    }

    for (int i = 0; i < k; ++i) {
        int sum = 0;
        for (int j = 0; j < n; ++j) {
            sum += *(int*)await(&res[(i * n + j)]);
        }
        printf("%d\n", sum);
    }

    return 0;
}