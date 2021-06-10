/*
 * Copyright (C) 2015-2020 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <dirent.h>

#include "amp_config.h"
#include "amp_platform.h"
#include "aos_system.h"
#include "aos_fs.h"
#include "aos/vfs.h"

int aos_fs_init(void)
{
    return 0;
}

int aos_open(const char *path, int flags)
{
    return open(path, flags, 0777);
}

int aos_close(int fd)
{
    return close(fd);
}

off_t aos_lseek(int fd, off_t offset, int whence) {
    return lseek(fd, offset, whence);
}

ssize_t aos_read(int fd, void *buf, size_t nbytes)
{
    return read(fd, buf, nbytes);
}

ssize_t aos_write(int fd, const void *buf, size_t nbytes)
{
    return write(fd, buf, nbytes);
}

int aos_sync(int fd)
{
    (void)fd;
    sync();
}

int aos_rmdir(const char *path)
{
    rmdir(path);
}

int aos_stat(const char *path, struct aos_stat *st)
{
    int ret;
    struct stat info;

    ret = stat(path, &info);
    st->st_mode = info.st_mode;
    st->st_size = info.st_size;
    return ret;
}

int aos_statfs(const char *path, struct aos_statfs *buf)
{
    int ret;
    struct aos_statfs info;

    ret = statfs(path, &info);
    buf->f_bsize  = info.f_bsize;
    buf->f_blocks = info.f_blocks;
    buf->f_bavail = info.f_bavail;
    return ret;
}

int aos_unlink(const char *path)
{
    int ret;
    ret = unlink(path);
    return ret;
}

int aos_remove(const char *path)
{
    int ret;
    ret = remove(path);
    return ret;
}

aos_dir_t *aos_opendir(const char *path)
{
    return opendir(path);
}

int aos_closedir(aos_dir_t *dir)
{
    return closedir((DIR*)dir);
}

aos_dirent_t *aos_readdir(aos_dir_t *dir)
{
    return readdir((DIR*)dir);
}

int aos_mkdir(const char *path)
{
    return mkdir(path, 0x777);
}

int aos_rmdir_r(const char *path)
{
    struct aos_stat s;
    int    ret = -1;
    char  *dir, *p;
    int    path_len;

    DIR             *pdir = NULL;
    struct dirent  *entry = NULL;

    if (!path)
        return -EINVAL;

    path_len = strlen(path) + 1;
    dir      = aos_malloc(path_len);
    if (dir == NULL) {
        return -1;
    }

    memcpy(dir, path, path_len);

    p = dir + strlen(dir) - 1;
    while ((*p == '/') && (p > dir)) {
        *p = '\0';
        p--;
    }

    if (stat(dir, &s) || !S_ISDIR(s.st_mode)) {
        aos_printf("%s is neither existed nor a directory\n", dir);
        goto out;
    }

    pdir = opendir(dir);
    if (!pdir) {
        aos_printf("opendir %s failed - %s\n", dir, strerror(errno));
        goto out;
    }

    ret = 0;
    while ((ret == 0) && (entry = (struct dirent *)aos_readdir(pdir))) {
        char fpath[128];

        snprintf(fpath, 128, "%s/%s", dir, entry->d_name);

        ret = stat(fpath, &s);
        if (ret) {
            aos_printf("stat %s failed\n", fpath);
            break;
        }

        if (!strcmp(entry->d_name, "."))
            continue;
        if (!strcmp(entry->d_name, ".."))
            continue;

        if (S_ISDIR(s.st_mode))
            ret = aos_rmdir_r(fpath);
        else
            ret = unlink(fpath);
    }

    closedir(pdir);
    if (ret == 0) {
        ret = rmdir(dir);
        if (ret)
            aos_printf("rmdir %s failed\n", dir);
    }
out:
    aos_free(dir);
    return ret;
}

