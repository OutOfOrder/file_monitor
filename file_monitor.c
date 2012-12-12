#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>

static int (*real_open)(const char *fn, int flags, ...);
static int (*real_open64)(const char *fn, int flags, ...);
static FILE* (*real_fopen)(const char*__restrict fn, const char*__restrict mode);
static FILE* (*real_fopen64)(const char*__restrict fn, const char*__restrict mode);
static int (*real_stat)(const char *__restrict path, struct stat *__restrict buf);
static int (*real_stat64)(const char *__restrict path, struct stat64 *__restrict buf);

static short startup = 0;
static FILE* logFP = NULL;

void startup_check()
{
    if (startup == 1) {
        return;
    }

    startup = 1;

    real_open = dlsym(RTLD_NEXT, "open");
    real_open64 = dlsym(RTLD_NEXT, "open64");
    real_fopen = dlsym(RTLD_NEXT, "fopen");
    real_fopen64 = dlsym(RTLD_NEXT, "fopen64");
    real_stat = dlsym(RTLD_NEXT, "stat");
    real_stat64 = dlsym(RTLD_NEXT, "stat64");

    char *logfile = getenv("FILE_MONITOR_LOG");
    if (logfile) {
        int logFD = real_open(logfile, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR);
        logFP = fdopen(logFD, "w");
    } else {
        logFP = stderr;
    }
}

void log_open(const char* func, const char* fn, const char* mode)
{
    fprintf(logFP, "opened file %s('%s','%s')\n", func, fn, mode);
}

void log_other(const char* func, const char* fn, const char* extra)
{
    fprintf(logFP, "called %s('%s') == '%s'", func, fn, extra);
}

int open(const char *fn, int flags, ...)
{
    startup_check();

    char m[6];
    m[0] = (flags & O_RDONLY) > 0 ? 'r' : '_';
    m[1] = (flags & O_WRONLY) > 0 ? 'w' : '_';
    m[2] = (flags & O_RDWR) > 0   ? '+' : '_';
    m[3] = (flags & O_CREAT) > 0  ? 'c' : '_';
    m[4] = (flags & O_APPEND) > 0 ? 'a' : '_';
    m[5] = '\0';

    log_open("open", fn, m);

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode_t mode = va_arg(ap, mode_t);
        va_end(ap);
        return real_open(fn, flags, mode);
    } else {
        return real_open(fn, flags);
    }
}

int open64(const char *fn, int flags, ...)
{
    startup_check();

    char m[6];
    m[0] = (flags & O_RDONLY) > 0 ? 'r' : '_';
    m[1] = (flags & O_WRONLY) > 0 ? 'w' : '_';
    m[2] = (flags & O_RDWR) > 0   ? '+' : '_';
    m[3] = (flags & O_CREAT) > 0  ? 'c' : '_';
    m[4] = (flags & O_APPEND) > 0 ? 'a' : '_';
    m[5] = '\0';

    log_open("open64", fn, m);

    if (flags & O_CREAT) {
        va_list ap;
        va_start(ap, flags);
        mode_t mode = va_arg(ap, mode_t);
        va_end(ap);
        return real_open64(fn, flags, mode);
    } else {
        return real_open64(fn, flags);
    }
}

FILE* fopen(const char* fn, const char* modes)
{
    startup_check();

    log_open("fopen", fn, modes);

    return real_fopen(fn, modes);
}

FILE* fopen64(const char* fn, const char* modes)
{
    startup_check();

    log_open("fopen64", fn, modes);

    return real_fopen64(fn, modes);
}

int stat(const char* path, struct stat* buf)
{
    startup_check();

    int ret = real_stat(path, buf);

    log_other("stat", path, ret == 0 ? "OK" : "!found");

    return ret;
}

int stat64(const char* path, struct stat64* buf)
{
    startup_check();

    int ret = real_stat64(path, buf);

    log_other("stat", path, ret == 0 ? "OK" : "!found");

    return ret;
}

// vim: et ts=4
