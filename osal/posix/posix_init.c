/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "posix/posix_init.h"

extern int pthread_lock_init(void);
extern int timer_lock_init(void);
extern int tmpnam_lock_init(void);

int ramfs_mount(void);

int posix_init(void)
{
    int ret = -1;

#if (POSIX_PTHREAD_ENABLE > 0)
    ret = pthread_lock_init();
    if (ret != 0) {
        return -1;
    }
#endif

#if (POSIX_TIMER_ENABLE > 0)
    ret = timer_lock_init();
    if (ret != 0) {
        return -1;
    }
#endif

#if (POSIX_DIRENT_ENABLE > 0)
#if (POSIX_DIRENT_TMPFILE_ENABLE > 0)
    ret = tmpnam_lock_init();
    if (ret != 0) {
        return -1;
    }

    ret = ramfs_mount();
    if (ret != 0) {
        return -1;
    }

#endif
#endif

    return ret;
}

int ramfs_mount(void)
{
    return ramfs_register("/ramfs");
}
