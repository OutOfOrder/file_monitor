#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>

int open(const char *fn, int flags) {
    static int (*real_open)(const char *fn, int flags);

    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }

    fprintf(stderr, "opened file '%s'\n", fn);
    return real_open(fn, flags);
}

// vim: et ts=4
