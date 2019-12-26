#include <stdio.h>
#include <stdlib.h>

void
syserr(int bl, const char *fmt, const char *func, const char *file, int line) {
    fprintf(stderr, "ERROR: %s (returned %d), at %s in %s, line %d", fmt, bl,
            func, file, line);
    exit(1);
}

void fatal(const char *fmt) {
    fprintf(stderr, "ERROR: %s", fmt);
    exit(1);
}

void *safe_malloc(size_t size) {
    void *res = malloc(size);
    if (!res)
        fatal("malloc failed");
    return res;
}
