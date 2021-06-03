/*
 * Copyright (C) 2018-2021 Alibaba Group Holding Limited
 */

#include <dirent.h>
#include <pthread.h>
#include <aos/kernel.h>

#include "internal/common.h"

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
    int ret = aos_lseek(fildes, offset, whence);
    CHECK_AOS_RET(ret);
    return ret;
}

int stat(const char *path, struct stat *buf)
{
    struct aos_stat stat_temp;
    int             ret = -1;

    ret = aos_stat(path, &stat_temp);
    if (ret == 0) {
        buf->st_mode = stat_temp.st_mode;
        buf->st_size = stat_temp.st_size;
        buf->st_atime = stat_temp.st_actime;
        buf->st_mtime = stat_temp.st_modtime;
    }

    return ret;
}

int fstat(int fh, struct stat *buf)
{
    struct aos_stat stat_temp;
    int             ret = -1;

    ret = aos_fstat(fh, &stat_temp);
    if (ret == 0) {
        buf->st_mode = stat_temp.st_mode;
        buf->st_size = stat_temp.st_size;
        buf->st_atime = stat_temp.st_actime;
        buf->st_mtime = stat_temp.st_modtime;
    }

    return ret;
}

int statfs(const char *path, struct statfs *buf)
{
    struct aos_statfs statfs_temp;
    int               ret = -1;

    ret = aos_statfs(path, &statfs_temp);
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

int link(const char *path1, const char *path2)
{
    int ret = aos_link(path1, path2);
    CHECK_AOS_RET(ret);
    return ret;
}

int unlink(const char *path)
{
    int ret = aos_unlink(path);
    CHECK_AOS_RET(ret);
    return ret;
}

int remove(const char *path)
{
    int ret = aos_remove(path);
    CHECK_AOS_RET(ret);
    return ret;
}

int rename(const char *oldname, const char *newname)
{
    int ret = aos_rename(oldname, newname);
    CHECK_AOS_RET(ret);
    return ret;
}

int fsync(int fd)
{
    int ret = aos_sync(fd);
    CHECK_AOS_RET(ret);
    return ret;
}

int fdatasync(int fildes)
{
    int ret = aos_sync(fildes);
    CHECK_AOS_RET(ret);
    return ret;
}

void sync(void)
{
    aos_allsync();
}

int mkdir(const char *path, mode_t mode)
{
    int ret = aos_mkdir(path);
    CHECK_AOS_RET(ret);
    return ret;
}

int rmdir(const char *path)
{
    int ret = aos_rmdir(path);
    CHECK_AOS_RET(ret);
    return ret;
}

void rewinddir(DIR *dirp)
{
    aos_rewinddir((aos_dir_t *)dirp);
}

int access(const char *filenpath, int mode)
{
    int ret = aos_access(filenpath, mode);
    CHECK_AOS_RET(ret);
    return ret;
}

int chdir(const char *path)
{
    int ret = aos_chdir(path);
    CHECK_AOS_RET(ret);
    return ret;
}

char *getcwd(char *buf, size_t size)
{
    return aos_getcwd(buf, size);
}

int creat(const char *path, mode_t mode)
{
    int ret = open(path, O_WRONLY|O_CREAT|O_TRUNC, mode);
    CHECK_AOS_RET(ret);
    return ret;
}

long pathconf(const char *path, int name)
{
    long ret = aos_pathconf(path, name);
    CHECK_AOS_RET(ret);
    return ret;
}

long fpathconf(int fildes, int name)
{
    long ret = aos_fpathconf(fildes, name);
    CHECK_AOS_RET(ret);
    return ret;
}

pthread_mutex_t g_tmpnam_mutex = PTHREAD_MUTEX_INITIALIZER;

char *tmpnam(char *s)
{
    static int temp_name_series = 0;

    char *temp_name_prefix = TEMP_FILE_NAME_MAGIC;
    int   ret           = -1;
    char  temp_name_series_buf[8];

    if (temp_name_series >= TMP_MAX) {
        return NULL;
    }

    memset(temp_name_series_buf, 0, sizeof(temp_name_series_buf));

    ret = pthread_mutex_lock(&g_tmpnam_mutex);
    if (ret != 0) {
        return NULL;
    }

    strncpy(s, temp_name_prefix, PATH_MAX);
    sprintf(temp_name_series_buf, "_%d", temp_name_series);
    strncat(s, temp_name_series_buf, PATH_MAX - strlen(s) - 1);

    temp_name_series++;

    pthread_mutex_unlock(&g_tmpnam_mutex);

    return s;
}

FILE *tmpfile(void)
{
    char *pname = NULL;
    char  path[PATH_MAX];

    pname = tmpnam(path);
    if (pname == NULL) {
        return NULL;
    }

    return fopen(pname, "w+");
}

int utime(const char *path, const struct utimbuf *times)
{
    struct aos_utimbuf utimbuf_temp;
    int             ret = -1;

    utimbuf_temp.actime = times->actime;
    utimbuf_temp.modtime = times->modtime;

    ret = aos_utime(path, &utimbuf_temp);
    CHECK_AOS_RET(ret);

    return ret;
}
