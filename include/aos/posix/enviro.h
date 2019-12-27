/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef ENVIRO_H
#define ENVIRO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>

/* definition for sysconf */
#define _POSIX_JOB_CONTROL         1
#define _POSIX_SAVED_IDS           1
#define _POSIX_VERSION             2016L
#define _POSIX_ASYNCHRONOUS_IO     1
#define _POSIX_FSYNC               1
#define _POSIX_MAPPED_FILES        0
#define _POSIX_MEMLOCK             0
#define _POSIX_MEMLOCK_RANGE       0
#define _POSIX_MEMORY_PROTECTION   0
#define _POSIX_MESSAGE_PASSING     1
#define _POSIX_MONOTONIC_CLOCK     1
#define _POSIX_CLOCK_SELECTION     1
#define _POSIX_PRIORITIZED_IO      1
#define _POSIX_PRIORITY_SCHEDULING 1
#define _POSIX_REALTIME_SIGNALS    0
#define _POSIX_SEMAPHORES          1
#define _POSIX_SYNCHRONIZED_IO     0
#define _POSIX_TIMERS              1
#define _POSIX_BARRIERS            0
#define _POSIX_READER_WRITER_LOCKS 0
#define _POSIX_SPIN_LOCKS          0

#define _POSIX_THREADS                    1
#define _POSIX_THREAD_ATTR_STACKADDR      1
#define _POSIX_THREAD_ATTR_STACKSIZE      1
#define _POSIX_THREAD_PRIORITY_SCHEDULING 1
#define _POSIX_THREAD_PRIO_INHERIT        1
#define _POSIX_THREAD_PRIO_PROTECT        1
#define _POSIX_THREAD_PROCESS_SHARED      1
#define _POSIX_THREAD_SAFE_FUNCTIONS      0

#define _POSIX_SPAWN          0
#define _POSIX_TIMEOUTS       0
#define _POSIX_CPUTIME        1
#define _POSIX_THREAD_CPUTIME 1
#define _POSIX_ADVISORY_INFO  0

/* definition for confstr */
#define _CS_GNU_LIBC_VERSION       1
#define _CS_GNU_LIBPTHREAD_VERSION 2

#define _POSIX_GNU_LIBC_VERSION       "newlib 7.3"
#define _POSIX_GNU_LIBPTHREAD_VERSION "IEEE Std 1003.1, 2016 Edition"

/* definition for uname */
#define _UTSNAME_SYSNAME_LENGTH  32
#define _UTSNAME_NODENAME_LENGTH 32
#define _UTSNAME_RELEASE_LENGTH  32
#define _UTSNAME_VERSION_LENGTH  32
#define _UTSNAME_MACHINE_LENGTH  32

struct utsname {
    char sysname[_UTSNAME_SYSNAME_LENGTH];   /* name of this implementation of the operating system */
    char nodename[_UTSNAME_NODENAME_LENGTH]; /* name of this node within the communications network to
                                                which this node is attached, if any */
    char release[_UTSNAME_RELEASE_LENGTH];   /* current release level of this implementation */
    char version[_UTSNAME_VERSION_LENGTH];   /* current version level of this release */
    char machine[_UTSNAME_MACHINE_LENGTH];   /* name of the hardware type on which the system is running */
};

/* definition for environment variable */
typedef struct _pthread_environ {
    char *envname;
    char *envval;

    struct _pthread_environ *next;
} _pthread_environ_t;

int   setenv(const char *envname, const char *envval, int overwrite);
char *getenv(const char *name);
int   unsetenv(const char *name);
int   putenv(char *string);

int    uname(struct utsname *name);
long   sysconf(int name);
size_t confstr(int name, char *buf, size_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* ENVIRO_H */

