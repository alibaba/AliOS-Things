/*
 * Copyright (C) 2018-2021 Alibaba Group Holding Limited
 */

#include <dirent.h>
#include <aos/errno.h>
#include <aos/kernel.h>

#include "internal/common.h"

DIR *opendir(const char *dirname)
{
    if (dirname == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return (DIR *)aos_opendir(dirname);
}

int closedir(DIR *dirp)
{
    int ret;

    CHECK_POSIX_PARAM(dirp);

    ret = aos_closedir((aos_dir_t *)dirp);
    CHECK_AOS_RET(ret);

    return ret;
}

struct dirent *readdir(DIR *dirp)
{
    if (dirp == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return (struct dirent *)aos_readdir(dirp);
}

long telldir(DIR *dirp)
{
    long ret;

    CHECK_POSIX_PARAM(dirp);

    ret = aos_telldir((aos_dir_t *)dirp);
    CHECK_AOS_RET(ret);

    return ret;
}

void seekdir(DIR *dirp, long loc)
{
    if (dirp == NULL) {
        return;
    }

    return aos_seekdir((aos_dir_t *)dirp, loc);
}

int mkdir(const char *path, mode_t mode)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_mkdir(path);
    CHECK_AOS_RET(ret);

    return ret;
}

int rmdir(const char *path)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_rmdir(path);
    CHECK_AOS_RET(ret);

    return ret;
}

void rewinddir(DIR *dirp)
{
    if (dirp == NULL) {
        return;
    }

    aos_rewinddir((aos_dir_t *)dirp);
}

int chdir(const char *path)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_chdir(path);
    CHECK_AOS_RET(ret);

    return ret;
}
