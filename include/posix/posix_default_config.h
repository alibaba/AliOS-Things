/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef POSIX_DEFAULT_CONFIG_H
#define POSIX_DEFAULT_CONFIG_H

#ifdef __cplusplus
extern "C"
{
#endif

/* module enable */
#ifndef POSIX_PTHREAD_ENABLE
#define POSIX_PTHREAD_ENABLE        1
#endif

#ifndef POSIX_MQUEUE_ENABLE
#define POSIX_MQUEUE_ENABLE         1
#endif

#ifndef POSIX_SEMAPHORE_ENABLE
#define POSIX_SEMAPHORE_ENABLE      1
#endif

#ifndef POSIX_TIMER_ENABLE
#define POSIX_TIMER_ENABLE          1
#endif

#ifndef POSIX_DIRENT_ENABLE
#define POSIX_DIRENT_ENABLE         1
#endif

/* POSIX_DIRENT_ENABLE must set to 1 before this enable */
/* ramfs must be added to the prj before this enable */
#ifndef POSIX_DIRENT_TMPFILE_ENABLE
#define POSIX_DIRENT_TMPFILE_ENABLE 1
#endif

#ifndef POSIX_PRCTL_ENABLE
#define POSIX_PRCTL_ENABLE          1
#endif

/* the position of pthread struct in user info in TCB */
#define PTHREAD_USER_INFO_POS 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_DEFAULT_CONFIG_H */
