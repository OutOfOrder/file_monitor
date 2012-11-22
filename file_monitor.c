#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

int open(const char *fn, int flags, ...) {
    static int (*real_open)(const char *fn, int flags, ...);
    static int envChecked = 0;
    static FILE* logFP = NULL;

    if (!real_open) {
        real_open = dlsym(RTLD_NEXT, "open");
    }
    
    if (envChecked == 0) {
        envChecked = 1;
        char *logfile = getenv("FILE_MONITOR_LOG");
        if (logfile) {
            int logFD = real_open(logfile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
            logFP = fdopen(logFD, "w");
        }
    }

    char m[4] = { '\0', '\0', '\0' };
    m[0] = (flags & O_CREAT) ? 'c' : '_';
    m[1] = (flags & O_WRONLY) ? 'w' : '_';
    m[2] = (flags & O_APPEND) ? 'a' : '_';

    if (logFP) {
        fprintf(logFP, "opened file '%s' - %s\n", fn, m);
    } else {
        fprintf(stderr, "opened file '%s' - %s\n", fn, m);
    }
    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode_t mode = va_arg(ap, mode_t);
        va_end(ap);
        return real_open(fn, flags);
    } else {
        return real_open(fn, flags);
    }
}

// vim: et ts=4
