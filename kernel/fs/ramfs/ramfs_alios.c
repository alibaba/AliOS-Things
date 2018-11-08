/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include "ramfs_alios.h"
#include "k_api.h"

#define RAMFS_PATH "/ramfs"

int        vfs_ramfs_open(file_t *fp, const char *path, int flags);
int        vfs_ramfs_close(file_t *fp);
ssize_t    vfs_ramfs_read(file_t *fp, char *buf, size_t len);
ssize_t    vfs_ramfs_write(file_t *fp, const char *buf, size_t len);
int        vfs_ramfs_access(file_t *fp, const char *path, int amode);
off_t      vfs_ramfs_lseek(file_t *fp, off_t off, int whence);
int        vfs_ramfs_unlink(file_t *fp, const char *path);
aos_dir_t *vfs_ramfs_opendir(file_t *fp, const char *path);
int        vfs_ramfs_closedir(file_t *fp, aos_dir_t *dir);
int        vfs_ramfs_stat(file_t *fp, const char *path, struct stat *st);
int        vfs_ramfs_statfs(file_t *fp, const char *path, struct statfs *buf);
int        vfs_ramfs_mkdir(file_t *fp, const char *path);
aos_dirent_t *vfs_ramfs_readdir(file_t *fp, aos_dir_t *dir);

fs_ops_t fs_ops_ramfs = { .open     = &vfs_ramfs_open,
                          .close    = &vfs_ramfs_close,
                          .read     = &vfs_ramfs_read,
                          .write    = &vfs_ramfs_write,
                          .access   = &vfs_ramfs_access,
                          .lseek    = &vfs_ramfs_lseek,
                          .sync     = NULL,
                          .stat     = &vfs_ramfs_stat,
                          .statfs   = &vfs_ramfs_statfs,
                          .unlink   = &vfs_ramfs_unlink,
                          .rename   = NULL,
                          .opendir  = &vfs_ramfs_opendir,
                          .readdir  = &vfs_ramfs_readdir,
                          .closedir = &vfs_ramfs_closedir,
                          .mkdir    = &vfs_ramfs_mkdir,
                          .ioctl    = NULL };

int ramfs_register(void)
{
    int ret = -1;

    ramfs_init();

    ret = ramfs_mkdir(RAMFS_PATH);

    if (ret == 0) {
        ret = aos_register_fs(RAMFS_PATH, &fs_ops_ramfs, NULL);
    }

    return ret;
}

int vfs_ramfs_open(file_t *fp, const char *path, int flags)
{
    int          ret = -EPERM;
    ramfs_file_t ramfs_file;
    int          ramfs_flags = 0;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    if (flags == O_RDWR) {
        ramfs_flags = RAMFS_MODE_WR | RAMFS_MODE_RD;
    } else if (flags == O_RDONLY) {
        ramfs_flags = RAMFS_MODE_RD;
    } else if (flags == O_WRONLY) {
        ramfs_flags = RAMFS_MODE_WR;
    } else {
        ramfs_flags = RAMFS_MODE_WR | RAMFS_MODE_RD;
        ;
    }

    ret = ramfs_open(&ramfs_file, path, ramfs_flags);

    if (ret == 0) {
        fp->f_arg = ramfs_file.ent;
    }

    return ret;
}

int vfs_ramfs_close(file_t *fp)
{
    int          ret = -EPERM;
    ramfs_file_t ramfs_file;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.ent = (ramfs_ent_t *)fp->f_arg;

    ret = ramfs_close(&ramfs_file);

    return ret;
}

ssize_t vfs_ramfs_read(file_t *fp, char *buf, size_t len)
{
    int          ret = -EPERM;
    ramfs_file_t ramfs_file;
    uint32_t     read_bytes = 0;
    int          res        = -1;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.ent = (ramfs_ent_t *)fp->f_arg;
    ramfs_file.rwp = fp->offset;

    ret = ramfs_read(&ramfs_file, buf, len, &read_bytes);

    if ((ret == 0) && (read_bytes >= 0)) {
        fp->offset = ramfs_file.rwp;
        res        = read_bytes;
    } else {
        res = -1;
    }

    return res;
}

ssize_t vfs_ramfs_write(file_t *fp, const char *buf, size_t len)
{
    int          ret = -EPERM;
    ramfs_file_t ramfs_file;
    uint32_t     write_bytes = 0;
    int          res         = -1;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.ent = (ramfs_ent_t *)fp->f_arg;
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

int vfs_ramfs_access(file_t *fp, const char *path, int amode)
{
    int ret = -EPERM;

    if ((amode == R_OK) || (amode == W_OK) || (amode == X_OK)) {
        return 0;
    }

    ret = ramfs_access(path, amode);

    if (ret != 0) {
        ret = -1;
    }

    return ret;
}

off_t vfs_ramfs_lseek(file_t *fp, off_t off, int whence)
{
    ramfs_file_t ramfs_file;
    off_t        offset = 0;

    memset(&ramfs_file, 0, sizeof(ramfs_file));

    ramfs_file.ent = (ramfs_ent_t *)fp->f_arg;

    offset = fp->offset;

    if (whence == SEEK_SET) {
        offset = off;
    } else if (whence == SEEK_CUR) {
        offset += off;
    } else if (whence == SEEK_END) {
        offset += ramfs_file.ent->size - 1;
    } else {
        ;
    }

    if ((offset < ramfs_file.ent->size) && (offset >= 0)) {
        fp->offset = offset;
    }

    return fp->offset;
}

int vfs_ramfs_unlink(file_t *fp, const char *path)
{
    int ret = -1;

    ret = ramfs_remove(path);

    return ret;
}

aos_dir_t *vfs_ramfs_opendir(file_t *fp, const char *path)
{
    vfs_ramfs_dir_t *vfs_ramfs_dir = NULL;
    aos_dir_t       *ret           = NULL;
    int              res           = -1;

    vfs_ramfs_dir =
      krhino_mm_alloc(sizeof(vfs_ramfs_dir_t) + MAX_RAMFS_FILE_NAME_BYTES);

    if (vfs_ramfs_dir != NULL) {
        fp->f_arg = vfs_ramfs_dir;

        res = ramfs_dir_open(&vfs_ramfs_dir->ramfs_dir, path);

        if (res == 0) {
            ret = &(vfs_ramfs_dir->aos_dir);
        }
    }

    return ret;
}

int vfs_ramfs_closedir(file_t *fp, aos_dir_t *dir)
{
    if (fp->f_arg != NULL) {
        krhino_mm_free(fp->f_arg);
    }

    return 0;
}

int vfs_ramfs_stat(file_t *fp, const char *path, struct stat *st)
{
    ramfs_stat_t ramfs_st;
    int          ret = 0;

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

int vfs_ramfs_statfs(file_t *fp, const char *path, struct statfs *buf)
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

int vfs_ramfs_mkdir(file_t *fp, const char *path)
{
    int ret = -1;

    ret = ramfs_mkdir(path);

    return ret;
}

aos_dirent_t *vfs_ramfs_readdir(file_t *fp, aos_dir_t *dir)
{
    vfs_ramfs_dir_t *vfs_ramfs_dir = NULL;
    aos_dirent_t    *ret           = NULL;
    int              res           = -1;

    vfs_ramfs_dir = fp->f_arg;

    if (vfs_ramfs_dir->aos_dirent.d_name != NULL) {
        res = ramfs_dir_read(&(vfs_ramfs_dir->ramfs_dir),
                             vfs_ramfs_dir->aos_dirent.d_name);

        if (res == 0) {
            ret = &vfs_ramfs_dir->aos_dirent;
        }
    }

    return ret;
}