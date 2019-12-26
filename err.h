#ifndef _ERR_
#define _ERR_

#include <stdlib.h>

#define try(expr) \
do {\
    int err;\
    if ((err = expr) != 0)\
        syserr(err, #expr" failed %s", __FILE__);\
} while(0)

//#define safe_malloc(SIZEOF) (malloc(SIZEOF) || return 1;)


#define try_no_exit(sth) do {int err; if((err = sth) != 0) return err;} while(0)

/* print system call error message and terminate */
extern void syserr(int bl, const char *fmt, ...);


/* print error message and terminate */
extern void fatal(const char *fmt, ...);

void *safe_malloc(size_t size);

#endif
