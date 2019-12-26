#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "threadpool.h"
#include "future.h"
#include "err.h"

future_t *res;

void *calculate(void * arg, size_t args, size_t *res_size) {
    assert(args == 2 * sizeof(int));

    // arg[0] = n!, arg[1] = n
    int *tmp = arg;
    tmp[0] *= ++tmp[1];

    *res_size = 2 * sizeof(int);

    return arg;
}

int main() {
    int n=3;
    scanf("%d", &n);

    thread_pool_t pool;
    thread_pool_init(&pool, 3);

    if (!(res = malloc(sizeof(future_t) * n)))
        syserr(1, "malloc failed");

    int dp[2];
    dp[0] = 1;
    dp[1] = 0;


    async(&pool, &res[0], (callable_t){.function = calculate, .arg = dp, .argsz = 2 * sizeof(int)});

    for (int i = 0; i < n - 1; ++i)
        map(&pool, &res[i+1], &res[i], calculate);

    int *result = await(&res[n -1]);

    printf("%d\n", result[0]);

    return 0;
}