/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <string.h>

#include "vfs_types.h"
#include "vfs_api.h"
#include "fs/ramfs.h"

#include "ramfs_api.h"
#include "ramfs_adapt.h"
#include "ramfs_types.h"

#define MAX_RAMFS_FILE_NAME_BYTES 32

#define RAMFS_DEFAULT_MOUNT_PATH "/RAMFS"

typedef struct {
    vfs_dir_t    vfs_dir;
    ramfs_dir_t  ramfs_dir;
    vfs_dirent_t vfs_dirent;
} ramfs_vfs_dir_t;

static int32_t ramfs_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    int32_t ret         = RAMFS_ERR_NOT_IMP;
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

static int32_t ramfs_vfs_close(vfs_file_t *fp)
{
    ramfs_file_t ramfs_file;

    int32_t ret = RAMFS_ERR_NOT_IMP;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.entry = (ramfs_entry_t *)fp->f_arg;

    ret = ramfs_close(&ramfs_file);

    return ret;
}

static int32_t ramfs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    ramfs_file_t ramfs_file;

    int32_t ret = RAMFS_ERR_NOT_IMP;
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

static int32_t ramfs_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    ramfs_file_t ramfs_file;

    int32_t ret = RAMFS_ERR_NOT_IMP;
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

static int32_t ramfs_vfs_access(vfs_file_t *fp, const char *path, int32_t amode)
{
    int32_t ret = RAMFS_ERR_NOT_IMP;

    if ((amode == R_OK) || (amode == W_OK) || (amode == X_OK)) {
        return 0;
    }

    ret = ramfs_access(path, amode);

    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

static uint32_t ramfs_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    ramfs_file_t ramfs_file;

    int64_t offset = 0;

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

static int32_t ramfs_vfs_link(vfs_file_t *fp, const char *path1, const char *path2)
{
    return ramfs_link(path1, path2);
}

static int32_t ramfs_vfs_unlink(vfs_file_t *fp, const char *path)
{
    return ramfs_unlink(path);
}

static int32_t ramfs_vfs_remove(vfs_file_t *fp, const char *path)
{
    return ramfs_remove(path);
}

static int32_t ramfs_vfs_rename(vfs_file_t *fp, const char *old, const char *new)
{
    return ramfs_rename(old, new);
}

static vfs_dir_t *ramfs_vfs_opendir(vfs_file_t *fp, const char *path)
{
    ramfs_vfs_dir_t *dp  = NULL;
    vfs_dir_t       *ret = NULL;
    int32_t          res = -1;

    dp = ramfs_mm_alloc(sizeof(ramfs_vfs_dir_t) + MAX_RAMFS_FILE_NAME_BYTES);

    if (dp != NULL) {
        fp->f_arg = dp;

        res = ramfs_opendir(&dp->ramfs_dir, path);

        if (res == 0) {
            ret = &(dp->vfs_dir);
        }
    }

    return ret;
}

static int32_t ramfs_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    if (fp->f_arg != NULL) {
        ramfs_mm_free(fp->f_arg);
    }

    return 0;
}

static int32_t ramfs_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
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

static int32_t ramfs_vfs_fstat(vfs_file_t *fp, vfs_stat_t *st)
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

static int32_t ramfs_vfs_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *buf)
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

static int32_t ramfs_vfs_mkdir(vfs_file_t *fp, const char *path)
{
    return ramfs_mkdir(path);
}

static vfs_dirent_t *ramfs_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    ramfs_vfs_dir_t *dp  = NULL;
    vfs_dirent_t    *ret = NULL;
    int32_t          res = -1;

    dp = fp->f_arg;

    if (dp->vfs_dirent.d_name != NULL) {
        res = ramfs_readdir(&(dp->ramfs_dir), dp->vfs_dirent.d_name);

        if (res == 0) {
            ret = &dp->vfs_dirent;
        }
    }

    return ret;
}

static int32_t ramfs_vfs_pathconf(vfs_file_t *fp, const char *path, int32_t name)
{
    return ramfs_pathconf(name);
}

static int32_t ramfs_vfs_fpathconf(vfs_file_t *fp, int32_t name)
{
    return ramfs_pathconf(name);
}

static int32_t ramfs_vfs_utime(vfs_file_t *fp, const char *path, const vfs_utimbuf_t *times)
{
    return RAMFS_ERR_NOT_IMP;
}

static void ramfs_vfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    ramfs_vfs_dir_t *dp  = NULL;

    dp = fp->f_arg;
    dp->ramfs_dir.last_entry = NULL;
}

static int ramfs_vfs_rmdir(vfs_file_t *fp, const char *path)
{
    return ramfs_rmdir(path);
}

static int ramfs_vfs_sync(vfs_file_t *fp)
{
    return 0;
}

vfs_filesystem_ops_t ramfs_ops = {
    .open      = &ramfs_vfs_open,
    .close     = &ramfs_vfs_close,
    .read      = &ramfs_vfs_read,
    .write     = &ramfs_vfs_write,
    .access    = &ramfs_vfs_access,
    .lseek     = &ramfs_vfs_lseek,
    .sync      = &ramfs_vfs_sync,
    .stat      = &ramfs_vfs_stat,
    .fstat     = &ramfs_vfs_fstat,
    .statfs    = &ramfs_vfs_statfs,
    .link      = &ramfs_vfs_link,
    .unlink    = &ramfs_vfs_unlink,
    .remove    = &ramfs_vfs_remove,
    .rename    = &ramfs_vfs_rename,
    .opendir   = &ramfs_vfs_opendir,
    .readdir   = &ramfs_vfs_readdir,
    .closedir  = &ramfs_vfs_closedir,
    .mkdir     = &ramfs_vfs_mkdir,
    .seekdir   = NULL,
    .ioctl     = NULL,
    .pathconf  = &ramfs_vfs_pathconf,
    .fpathconf = &ramfs_vfs_fpathconf,
    .utime     = &ramfs_vfs_utime,
    .rewinddir = &ramfs_vfs_rewinddir,
    .rmdir     = &ramfs_vfs_rmdir 
};

int32_t ramfs_register(const char *mount_path)
{
    int32_t ret = RAMFS_ERR_NOT_IMP;

    ramfs_init();

    if (mount_path != NULL) {
        ret = ramfs_mkdir(mount_path);

        if (ret == 0) {
            return vfs_register_fs(mount_path, &ramfs_ops, NULL);
        }
    } else {
        ret = ramfs_mkdir(RAMFS_DEFAULT_MOUNT_PATH);

        if (ret == 0) {
            return vfs_register_fs(RAMFS_DEFAULT_MOUNT_PATH, &ramfs_ops, NULL);
        }
    }

    return ret;
}
