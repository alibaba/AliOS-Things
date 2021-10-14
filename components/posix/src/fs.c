/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stddef.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <utime.h>
#include <aos/vfs.h>

#include "internal/common.h"

int statfs(const char *path, struct statfs *buf)
{
    int ret;
    struct aos_statfs statfs_temp;

    CHECK_POSIX_PARAM(path);
    CHECK_POSIX_PARAM(buf);

    ret = aos_statfs(path, &statfs_temp);
    CHECK_AOS_RET(ret);
    if (ret == 0) {
        buf->f_type    = statfs_temp.f_type;
        buf->f_bsize   = statfs_temp.f_bsize;
        buf->f_blocks  = statfs_temp.f_blocks;
        buf->f_bfree   = statfs_temp.f_bfree;
        buf->f_bavail  = statfs_temp.f_bavail;
        buf->f_files   = statfs_temp.f_files;
        buf->f_ffree   = statfs_temp.f_ffree;
        buf->f_fsid    = statfs_temp.f_fsid;
        buf->f_namelen = statfs_temp.f_namelen;
    }

    return ret;
}

int remove(const char *path)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_remove(path);
    CHECK_AOS_RET(ret);

    return ret;
}

int fsync(int fd)
{
    int ret;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    ret = aos_sync(fd);
    CHECK_AOS_RET(ret);

    return ret;
}

int fdatasync(int fd)
{
    int ret;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    ret = aos_sync(fd);
    CHECK_AOS_RET(ret);

    return ret;
}

void sync(void)
{
    aos_allsync();
}

int access(const char *path, int mode)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_access(path, mode);
    CHECK_AOS_RET(ret);

    return ret;
}


char *getcwd(char *buf, size_t size)
{
    if (buf == NULL) {
        return NULL;
    }

    return aos_getcwd(buf, size);
}

int creat(const char *path, mode_t mode)
{
    int ret;

    CHECK_POSIX_PARAM(path);

    ret = open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
    CHECK_AOS_RET(ret);

    return ret;
}

long pathconf(const char *path, int name)
{
    long ret;

    CHECK_POSIX_PARAM(path);

    ret = aos_pathconf(path, name);
    CHECK_AOS_RET(ret);

    return ret;
}

long fpathconf(int fd, int name)
{
    long ret;

    if (fd < 0) {
        errno = EBADF;
        return -1;
    }

    ret = aos_fpathconf(fd, name);
    CHECK_AOS_RET(ret);

    return ret;
}

int utime(const char *path, const struct utimbuf *buf)
{
    int ret;
    struct aos_utimbuf utimbuf_temp;

    CHECK_POSIX_PARAM(path);

    if (buf == NULL) {
        utimbuf_temp.actime = time(NULL);
        utimbuf_temp.modtime = utimbuf_temp.actime;
    } else {
        utimbuf_temp.actime = buf->actime;
        utimbuf_temp.modtime = buf->modtime;
    }

    ret = aos_utime(path, &utimbuf_temp);
    CHECK_AOS_RET(ret);

    return ret;
}
