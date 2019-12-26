#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"


void *calculate(void * arg, size_t argsz, size_t *res_size) {
    assert(argsz == 2 * sizeof(int));

    // arg[0] = n!, arg[1] = n
    int *tmp = arg;
    tmp[0] *= ++tmp[1];

    *res_size = argsz;

    return arg;
}

int main() {

    int n;
    scanf("%d", &n);

    thread_pool_t pool;
    try(thread_pool_init(&pool, 3));

    future_t *res;
    res = safe_malloc(sizeof(future_t) * n);

    int dp[2];
    dp[0] = 1;
    dp[1] = 0;

    try(async(&pool, &res[0], (callable_t)
    {.function = calculate, .arg = dp, .argsz = 2 * sizeof(int)}));


    for (int i = 0; i < n - 1; ++i)
        try(map(&pool, &res[i+1], &res[i], calculate));

    int *result = await(&res[n -1]);

    printf("%d\n", result[0]);

    thread_pool_destroy(&pool);
    free(res);

    return 0;
}