#ifndef ERR_H
#define ERR_H

#include <stdio.h>

/// Macro that tries to perform an action - expr.
/// If this expr doesn't evaluate to 0, which means an error,
/// the error is printed and program execution is aborted.
#define try(expr) \
do {\
    int err;\
    if ((err = (int)(expr)) != 0)\
        syserr(err, #expr" failed", __func__, __FILE__, __LINE__);\
} while(0)

/// Macro that tries to perform an action - expr.
/// If this expr doesn't evaluate to 0, which means an error,
/// the error is returned.
#define easy_try(expr) \
do {\
    int err;\
    if ((err = (int)(expr)) != 0)\
        return err;\
} while(0)

/// Macro that tries to perform an action - expr.
/// If this expr evaluates to NULL, which means an error,
/// the error is printed and program execution is aborted.
#define try_ptr(expr) \
do {\
    void* ptr;\
    if ((ptr = expr) == NULL)\
        syserr(0, #expr" failed", __func__, __FILE__, __LINE__);\
} while(0)


/// Prints error message and aborts the program execution.
void
syserr(int bl, const char *fmt, const char *func, const char *file, int line);

#endif // ERR_H
