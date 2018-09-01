/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <dirent.h>
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

int access(const char *filenpath, int mode)
{
    return aos_access(filenpath, mode);
}
