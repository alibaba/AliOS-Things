/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef POSIX_DEFAULT_CONFIG_H
#define POSIX_DEFAULT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* set to 1 if you want to enable pthread, set to 0 to disable pthread */
#ifndef POSIX_CONFIG_PTHREAD_ENABLE
#define POSIX_CONFIG_PTHREAD_ENABLE        1
#endif

/* set to 1 if you want to enable mqueue, set to 0 to disable mqueue */
#ifndef POSIX_CONFIG_MQUEUE_ENABLE
#define POSIX_CONFIG_MQUEUE_ENABLE         1
#endif

/* set to 1 if you want to enable semaphore, set to 0 to disable semaphore */
#ifndef POSIX_CONFIG_SEMAPHORE_ENABLE
#define POSIX_CONFIG_SEMAPHORE_ENABLE      1
#endif

/* set to 1 if you want to enable timer, set to 0 to disable timer */
#ifndef POSIX_CONFIG_TIMER_ENABLE
#define POSIX_CONFIG_TIMER_ENABLE          1
#endif

/* set to 1 if you want to enable dirent, set to 0 to disable dirent */
#ifndef POSIX_CONFIG_DIRENT_ENABLE
#define POSIX_CONFIG_DIRENT_ENABLE         1
#endif

/* POSIX_CONFIG_DIRENT_ENABLE must set to 1 before this enable */
/* ramfs must be added to the prj before this enable */
/* set to 1 if you want to enable tmpfile, set to 0 to disable tmpfile */
#ifndef POSIX_CONFIG_DIRENT_TMPFILE_ENABLE
#define POSIX_CONFIG_DIRENT_TMPFILE_ENABLE 1
#endif

/* set to 1 if you want to enable prctl, set to 0 to disable prctl */
#ifndef POSIX_CONFIG_PRCTL_ENABLE
#define POSIX_CONFIG_PRCTL_ENABLE          1
#endif

/* the position of pthread struct in user info in TCB */
#define PTHREAD_CONFIG_USER_INFO_POS 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_DEFAULT_CONFIG_H */
