/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#ifndef POSIX_CONFIG_H
#define POSIX_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef POSIX_CONFIG_PTHREAD_ENABLE
#define POSIX_CONFIG_PTHREAD_ENABLE 1
#endif

#ifndef POSIX_CONFIG_MQUEUE_ENABLE
#define POSIX_CONFIG_MQUEUE_ENABLE 1
#endif

#ifndef POSIX_CONFIG_SEMAPHORE_ENABLE
#define POSIX_CONFIG_SEMAPHORE_ENABLE 1
#endif

#ifndef POSIX_CONFIG_TIMER_ENABLE
#define POSIX_CONFIG_TIMER_ENABLE 1
#endif

#ifndef POSIX_CONFIG_DIRENT_ENABLE
#define POSIX_CONFIG_DIRENT_ENABLE 1
#endif

/*
 * POSIX_CONFIG_DIRENT_ENABLE must set to 1 before this enable
 * ramfs must be added to the prj before this enable
 */
#ifndef POSIX_CONFIG_DIRENT_TMPFILE_ENABLE
#define POSIX_CONFIG_DIRENT_TMPFILE_ENABLE 0
#endif

#ifndef POSIX_CONFIG_PRCTL_ENABLE
#define POSIX_CONFIG_PRCTL_ENABLE 1
#endif

#ifndef POSIX_CONFIG_ENVIRO_ENABLE
#define POSIX_CONFIG_ENVIRO_ENABLE 1
#endif

#define PTHREAD_CONFIG_USER_INFO_POS 0

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* POSIX_CONFIG_H */

