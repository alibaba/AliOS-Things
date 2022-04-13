/*
 * Copyright (C) 2018-2020 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <dirent.h>
#include <aos/vfs.h>

pthread_mutex_t g_tmpnam_lock = PTHREAD_MUTEX_INITIALIZER;

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

    return aos_statfs(path, (struct aos_statfs *)buf);
}

int link(const char *path1, const char *path2)
{
    return aos_link(path1, path2);
}

int unlink(const char *path)
{
    return aos_unlink(path);
}

int remove(const char *path)
{
    return aos_remove(path);
}

int rename(const char *oldname, const char *newname)
{
    return aos_rename(oldname, newname);
}

int fsync(int fd)
{
    return aos_fsync(fd);
}

int fdatasync(int fildes)
{
    return aos_fsync(fildes);
}

void sync(void)
{
    aos_sync();
}

int mkdir(const char *path, mode_t mode)
{
    int ret = 0;
    ret = aos_mkdir(path);
    ret = ret == 0 ? ret : -1;
    return ret;
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
    int ret = 0;
    ret = aos_access(filenpath, mode);
    ret = ret == 0 ? ret : -1;
    return ret;
}

int chdir(const char *path)
{
    return aos_chdir(path);
}

char *getcwd(char *buf, size_t size)
{
    return aos_getcwd(buf, size);
}

int creat(const char *path, mode_t mode)
{
    return open(path, O_WRONLY | O_CREAT | O_TRUNC, mode);
}

long pathconf(const char *path, int name)
{
    return aos_pathconf(path, name);
}

long fpathconf(int fildes, int name)
{
    return aos_fpathconf(fildes, name);
}

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

    ret = pthread_mutex_lock(&g_tmpnam_lock);
    if (ret != 0) {
        return NULL;
    }

    strcpy(s, temp_name_prefix);
    sprintf(temp_name_series_buf, "_%d", temp_name_series);
    strcat(s, temp_name_series_buf);

    temp_name_series++;

    pthread_mutex_unlock(&g_tmpnam_lock);

    return s;
}

FILE *tmpfile(void)
{
    char *pname = NULL;
    int   ret   = 0;
    char  path[PATH_MAX];

    pname = tmpnam(path);
    if (pname == NULL) {
        return NULL;
    }

    return fopen(pname, O_RDWR);
}

int utime(const char *path, const struct utimbuf *times)
{
    struct aos_utimbuf utimbuf_temp;
    int             ret = -1;

    utimbuf_temp.actime = times->actime;
    utimbuf_temp.modtime = times->modtime;

    ret = aos_utime(path, &utimbuf_temp);

    return ret;
}

