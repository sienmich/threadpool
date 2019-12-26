#ifndef _ERR_
#define _ERR_

#define try(expr) \
do {\
    int err;\
    if ((err = expr) != 0)\
        syserr(err, #expr" failed", __func__, __FILE__, __LINE__);\
} while(0)

//#define try_no_exit(sth) do {int err; if((err = sth) != 0) return err;} while(0)

/* print system call error message and terminate */
void
syserr(int bl, const char *fmt, const char *func, const char *file, int line);

/* print error message and terminate */
extern void fatal(const char *fmt);

void *safe_malloc(size_t size);

#endif
