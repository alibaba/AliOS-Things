/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <dirent.h>
#include <stdarg.h>
#include "aos/network.h"

DIR *opendir(const char *dirname)
{
    return (DIR *)aos_opendir(dirname);
}

struct dirent *readdir(DIR *dirp)
{
    return (struct dirent *)aos_readdir(dirp);
}

int closedir(DIR *dirp)
{
    return aos_closedir((aos_dir_t *)dirp);
}

long telldir(DIR *dirp)
{
    return aos_telldir((aos_dir_t *)dirp);
}

void seekdir(DIR *dirp, long loc)
{
    return aos_seekdir((aos_dir_t *)dirp, loc);
}

off_t lseek(int fildes, off_t offset, int whence)
{
    return aos_lseek(fildes, offset, whence);
}

int stat(const char *path, struct stat *buf)
{
    return aos_stat(path, buf);
}

int statfs(const char *path, struct statfs *buf)
{
    return aos_statfs(path, buf);
}

int unlink(const char *path)
{
    return aos_unlink(path);
}

int remove(const char *filename)
{
    return aos_unlink(filename);
}

int rename(const char *oldname, const char *newname)
{
    return aos_rename(oldname, newname);
}

int fsync(int fd)
{
    return aos_sync(fd);
}

int mkdir(const char *path, mode_t mode)
{
    return aos_mkdir(path);
}

int rmdir(const char *path)
{
    return aos_rmdir(path);
}

void rewinddir(DIR *dirp)
{
    aos_rewinddir((aos_dir_t *)dirp);
}

int ioctl(int fildes, int request, ... /* arg */)
{
    long arg = 0;
    void *argp = NULL;
    va_list args;

    va_start(args, request);

    if ((fildes >= AOS_CONFIG_VFS_FD_OFFSET)
        && (fildes <= (AOS_CONFIG_VFS_FD_OFFSET + MAX_FILE_NUM - 1))) {
        arg = va_arg(args, int);
        return aos_ioctl(fildes, request, arg);
    } else if ((fildes >= FD_AOS_SOCKET_OFFSET)
               && (fildes <= (FD_AOS_EVENT_OFFSET + FD_AOS_NUM_EVENTS  - 1))) {
        argp = va_arg(args, void *);
        return lwip_ioctl(fildes, request, argp);
    } else {
        return -1;
    }
}

int access(const char *filenpath, int mode)
{
    return aos_access(filenpath, mode);
}