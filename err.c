#include <stdlib.h>
#include "err.h"

/// Prints error message and aborts the program execution.
void
syserr(int bl, const char *fmt, const char *func, const char *file, int line) {
    fprintf(stderr, "ERROR: %s (returned %d), at %s in %s, line %d", fmt, bl,
            func, file, line);
    exit(1);
}
