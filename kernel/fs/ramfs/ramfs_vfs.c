/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "aos/errno.h"

#include "aos/vfs.h"

#include "ramfs_types.h"
#include "ramfs_api.h"
#include "ramfs_adapt.h"

#define MAX_RAMFS_FILE_NAME_BYTES 32

#define RAMFS_DEFAULT_MOUNT_PATH "/RAMFS"

typedef struct {
    aos_dir_t    aos_dir;
    ramfs_dir_t  ramfs_dir;
    aos_dirent_t aos_dirent;
} vfs_ramfs_dir_t;

static int32_t vfs_ramfs_open(file_t *fp, const char *path, int32_t flags)
{
    int32_t ret         = -EPERM;
    int32_t ramfs_flags = 0;

    ramfs_file_t ramfs_file;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    if (flags == O_RDWR) {
        ramfs_flags = RAMFS_MODE_WR | RAMFS_MODE_RD;
    } else if (flags == O_RDONLY) {
        ramfs_flags = RAMFS_MODE_RD;
    } else if (flags == O_WRONLY) {
        ramfs_flags = RAMFS_MODE_WR;
    } else {
        ramfs_flags = RAMFS_MODE_WR | RAMFS_MODE_RD;
    }

    ret = ramfs_open(&ramfs_file, path, ramfs_flags);

    if (ret == 0) {
        fp->f_arg = ramfs_file.entry;
    }

    return ret;
}

static int32_t vfs_ramfs_close(file_t *fp)
{
    ramfs_file_t ramfs_file;

    int32_t ret = -EPERM;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;

    ret = ramfs_close(&ramfs_file);

    return ret;
}

static int32_t vfs_ramfs_read(file_t *fp, char *buf, uint32_t len)
{
    ramfs_file_t ramfs_file;

    int32_t ret = -EPERM;
    int32_t res = -1;

    uint32_t read_bytes = 0;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;
    ramfs_file.rwp   = fp->offset;

    ret = ramfs_read(&ramfs_file, buf, len, &read_bytes);

    if ((ret == 0) && (read_bytes >= 0)) {
        fp->offset = ramfs_file.rwp;
        res        = read_bytes;
    } else {
        res = -1;
    }

    return res;
}

static int32_t vfs_ramfs_write(file_t *fp, const char *buf, uint32_t len)
{
    ramfs_file_t ramfs_file;

    int32_t ret = -EPERM;
    int32_t res = -1;

    uint32_t write_bytes = 0;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;
    ramfs_file.rwp = fp->offset;

    ret = ramfs_write(&ramfs_file, buf, len, &write_bytes);

    if ((ret == 0) && (write_bytes >= 0)) {
        fp->offset = ramfs_file.rwp;
        res        = write_bytes;
    } else {
        res = -1;
    }

    return res;
}

static int32_t vfs_ramfs_access(file_t *fp, const char *path, int32_t amode)
{
    int32_t ret = -EPERM;

    if ((amode == R_OK) || (amode == W_OK) || (amode == X_OK)) {
        return 0;
    }

    ret = ramfs_access(path, amode);

    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

static off_t vfs_ramfs_lseek(file_t *fp, off_t off, int32_t whence)
{
    ramfs_file_t ramfs_file;

    int32_t offset = 0;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;

    offset = fp->offset;

    if (whence == SEEK_SET) {
        offset = off;
    } else if (whence == SEEK_CUR) {
        offset += off;
    } else if (whence == SEEK_END) {
        offset += ramfs_file.entry->size - 1;
    }

    if ((offset < ramfs_file.entry->size) && (offset >= 0)) {
        fp->offset = offset;
    }

    return fp->offset;
}

static int32_t vfs_ramfs_link(file_t *fp, const char *path1, const char *path2)
{
    return ramfs_link(path1, path2);
}

static int32_t vfs_ramfs_unlink(file_t *fp, const char *path)
{
    return ramfs_unlink(path);
}

static int32_t vfs_ramfs_remove(file_t *fp, const char *path)
{
    return ramfs_remove(path);
}

static int32_t vfs_ramfs_rename(file_t *fp, const char *old, const char *new)
{
    return ramfs_rename(old, new);
}

static aos_dir_t *vfs_ramfs_opendir(file_t *fp, const char *path)
{
    vfs_ramfs_dir_t *dp  = NULL;
    aos_dir_t       *ret = NULL;
    int32_t          res = -1;

    dp = ramfs_mm_alloc(sizeof(vfs_ramfs_dir_t) + MAX_RAMFS_FILE_NAME_BYTES);

    if (dp != NULL) {
        fp->f_arg = dp;

        res = ramfs_opendir(&dp->ramfs_dir, path);

        if (res == 0) {
            ret = &(dp->aos_dir);
        }
    }

    return ret;
}

static int32_t vfs_ramfs_closedir(file_t *fp, aos_dir_t *dir)
{
    if (fp->f_arg != NULL) {
        ramfs_mm_free(fp->f_arg);
    }

    return 0;
}

static int32_t vfs_ramfs_stat(file_t *fp, const char *path, struct aos_stat *st)
{
    ramfs_stat_t ramfs_st;

    int32_t ret = 0;

    if (st == NULL) {
        return -1;
    }

    memset(&ramfs_st, 0, sizeof(ramfs_st));

    ret = ramfs_stat(path, &ramfs_st);

    if (ret == 0) {
        st->st_size = ramfs_st.st_size;
        st->st_mode = S_IXUSR | S_IXGRP | S_IXOTH;

        if ((ramfs_st.st_mode & RAMFS_MODE_RD) == RAMFS_MODE_RD) {
            st->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
        }

        if ((ramfs_st.st_mode & RAMFS_MODE_WR) == RAMFS_MODE_WR) {
            st->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        }

        if (ramfs_st.is_dir == 1) {
            st->st_mode |= S_IFDIR;
        } else {
            st->st_mode |= S_IFREG;
        }

    } else {
        ret = -1;
    }

    return ret;
}

static int32_t vfs_ramfs_fstat(file_t *fp, struct aos_stat *st)
{
    ramfs_stat_t ramfs_st;
    ramfs_file_t ramfs_file;

    int32_t ret = 0;

    if (st == NULL) {
        return -1;
    }

    memset(&ramfs_st, 0, sizeof(ramfs_st));
    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;

    ret = ramfs_stat(ramfs_file.entry->fn, &ramfs_st);

    if (ret == 0) {
        st->st_size = ramfs_st.st_size;
        st->st_mode = S_IXUSR | S_IXGRP | S_IXOTH;

        if ((ramfs_st.st_mode & RAMFS_MODE_RD) == RAMFS_MODE_RD) {
            st->st_mode |= S_IRUSR | S_IRGRP | S_IROTH;
        }

        if ((ramfs_st.st_mode & RAMFS_MODE_WR) == RAMFS_MODE_WR) {
            st->st_mode |= S_IWUSR | S_IWGRP | S_IWOTH;
        }

        if (ramfs_st.is_dir == 1) {
            st->st_mode |= S_IFDIR;
        } else {
            st->st_mode |= S_IFREG;
        }

    } else {
        ret = -1;
    }

    return ret;
}

static int32_t vfs_ramfs_statfs(file_t *fp, const char *path, struct aos_statfs *buf)
{
    if (buf == NULL) {
        return -1;
    }

    buf->f_type    = RAMFS_MAGIC;
    buf->f_bsize   = 0xFFFFFFFF;
    buf->f_blocks  = 0xFFFFFFFF;
    buf->f_bfree   = 0xFFFFFFFF;
    buf->f_bavail  = 0xFFFFFFFF;
    buf->f_files   = 0xFFFFFFFF;
    buf->f_ffree   = 0xFFFFFFFF;
    buf->f_fsid    = RAMFS_MAGIC;
    buf->f_namelen = MAX_RAMFS_FILE_NAME_BYTES;

    return 0;
}

static int32_t vfs_ramfs_mkdir(file_t *fp, const char *path)
{
    return ramfs_mkdir(path);
}

static aos_dirent_t *vfs_ramfs_readdir(file_t *fp, aos_dir_t *dir)
{
    vfs_ramfs_dir_t *dp  = NULL;
    aos_dirent_t    *ret = NULL;
    int32_t          res = -1;

    dp = fp->f_arg;

    if (dp->aos_dirent.d_name != NULL) {
        res = ramfs_readdir(&(dp->ramfs_dir), dp->aos_dirent.d_name);

        if (res == 0) {
            ret = &dp->aos_dirent;
        }
    }

    return ret;
}

static int32_t vfs_ramfs_pathconf(file_t *fp, const char *path, int32_t name)
{
    return ramfs_pathconf(name);
}

static int32_t vfs_ramfs_fpathconf(file_t *fp, int32_t name)
{
    return ramfs_pathconf(name);
}

static int32_t vfs_ramfs_utime(file_t *fp, const char *path, const struct aos_utimbuf *times)
{
    return -1;
}

static void vfs_ramfs_rewinddir(file_t *fp, aos_dir_t *dir)
{
    vfs_ramfs_dir_t *dp  = NULL;

    dp = fp->f_arg;
    dp->ramfs_dir.last_entry = NULL;
}

static int vfs_ramfs_rmdir(file_t *fp, const char *path)
{
    return ramfs_rmdir(path);
}

fs_ops_t fs_ops_ramfs = { .open      = &vfs_ramfs_open,
                          .close     = &vfs_ramfs_close,
                          .read      = &vfs_ramfs_read,
                          .write     = &vfs_ramfs_write,
                          .access    = &vfs_ramfs_access,
                          .lseek     = &vfs_ramfs_lseek,
                          .sync      = NULL,
                          .stat      = &vfs_ramfs_stat,
                          .fstat     = &vfs_ramfs_fstat,
                          .statfs    = &vfs_ramfs_statfs,
                          .link      = &vfs_ramfs_link,
                          .unlink    = &vfs_ramfs_unlink,
                          .remove    = &vfs_ramfs_remove,
                          .rename    = &vfs_ramfs_rename,
                          .opendir   = &vfs_ramfs_opendir,
                          .readdir   = &vfs_ramfs_readdir,
                          .closedir  = &vfs_ramfs_closedir,
                          .mkdir     = &vfs_ramfs_mkdir,
                          .seekdir   = NULL,
                          .ioctl     = NULL,
                          .pathconf  = &vfs_ramfs_pathconf,
                          .fpathconf = &vfs_ramfs_fpathconf,
                          .utime     = &vfs_ramfs_utime,
                          .rewinddir = &vfs_ramfs_rewinddir,
                          .rmdir     = vfs_ramfs_rmdir };

int32_t ramfs_register(const char *mount_path)
{
    int32_t ret = -1;

    ramfs_init();

    if (mount_path != NULL) {
        ret = ramfs_mkdir(mount_path);

        if (ret == 0) {
            return aos_register_fs(mount_path, &fs_ops_ramfs, NULL);
        }
    } else {
        ret = ramfs_mkdir(RAMFS_DEFAULT_MOUNT_PATH);

        if (ret == 0) {
            return aos_register_fs(RAMFS_DEFAULT_MOUNT_PATH, &fs_ops_ramfs, NULL);
        }
    }

    return ret;
}

