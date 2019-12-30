/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <errno.h>
#include <unistd.h>
#include "os-alios.h"

#include "fs/cramfs.h"
#include "vfs_types.h"
#include "vfs_api.h"

#include "cramfs_fs.h"

static const char *cramfs_mnt_path = "/cramfs";
static kmutex_t g_cramfs_lock;
static struct cramfs_dirent *g_root = NULL;

typedef struct _cramfs_dir_t {
    vfs_dir_t dir;
    struct cramfs_dirent cramfs_dir;
    vfs_dirent_t cur_dirent;
} cramfs_dir_t;

static char *translate_relative_path(const char *path)
{
    int32_t len, prefix_len;
    char *relpath, *p;

    if (!path) {
        return NULL;
    }

    len = strlen(path);
    prefix_len = strlen(cramfs_mnt_path);
    if (strncmp(cramfs_mnt_path, path, prefix_len) != 0) {
        return NULL;
    }

    len = len - prefix_len;

    relpath = (char *)krhino_mm_alloc(len + 1);
    if (!relpath) {
        return NULL;
    }

    memset(relpath, 0, len + 1);
    if (len > 0) {
        p = (char *)(path + prefix_len + 1);
        memcpy(relpath, p, len - 1);
    }

    relpath[len] = '\0';

    return relpath;
}

static int32_t cramfs_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    int32_t ret;
    char *relpath = NULL;
    struct cramfs_dirent *dirent = NULL;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    dirent = (struct cramfs_dirent *)krhino_mm_alloc(sizeof(struct cramfs_dirent));
    if (!dirent) {
        return -ENOMEM;
    }

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_open(g_root, relpath, flags, dirent);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret == 0) {
        fp->f_arg = (void *)dirent;
    } else {
        krhino_mm_free(dirent);
    }

    krhino_mm_free(relpath);
    return ret;
}

static int32_t cramfs_vfs_close(vfs_file_t *fp)
{
    int32_t ret;
    struct cramfs_dirent *dirent = NULL;

    dirent = (struct cramfs_dirent *)(fp->f_arg);
    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_close(dirent);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret == 0) {
        krhino_mm_free(dirent);
        fp->f_arg = NULL;
    }

    return ret;
}

static int32_t cramfs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    int32_t nbytes;
    struct cramfs_dirent *dirent = NULL;

    dirent = (struct cramfs_dirent *)(fp->f_arg);

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    nbytes = cramfs_read(dirent, buf, len);
    krhino_mutex_unlock(&g_cramfs_lock);

    return nbytes;
}

static uint32_t cramfs_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    int32_t ret;
    int64_t pos;

    struct cramfs_dirent *dirent = NULL;

    dirent = (struct cramfs_dirent *)(fp->f_arg);
    pos = off;

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_lseek(dirent, &pos, whence);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret == 0) {
        return pos;
    }

    return ret;
}

static int32_t cramfs_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    int32_t ret;
    char *relpath = NULL;
    struct stat file_st;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return -EINVAL;
    }

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_stat(g_root, relpath, &file_st);
    krhino_mutex_unlock(&g_cramfs_lock);

    st->st_mode = file_st.st_mode;
    st->st_size = file_st.st_size;

    krhino_mm_free(relpath);
    return ret;
}

static int32_t cramfs_vfs_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *buf)
{
    if (buf == NULL) {
        return -1;
    }

    buf->f_type = CRAMFS_MAGIC;
    buf->f_bsize = 0xFFFFFFFF;
    buf->f_blocks = 0xFFFFFFFF;
    buf->f_bfree = 0xFFFFFFFF;
    buf->f_bavail = 0xFFFFFFFF;
    buf->f_files = 0xFFFFFFFF;
    buf->f_ffree = 0xFFFFFFFF;
    buf->f_fsid = CRAMFS_MAGIC;
    buf->f_namelen = CRAMFS_MAXPATHLEN;

    return 0;
}

static vfs_dir_t *cramfs_vfs_opendir(vfs_file_t *fp, const char *path)
{
    int32_t ret;
    cramfs_dir_t *dp = NULL;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath) {
        return NULL;
    }

    dp = (cramfs_dir_t *)krhino_mm_alloc(sizeof(cramfs_dir_t) + CRAMFS_MAXPATHLEN);
    if (!dp) {
        krhino_mm_free(relpath);
        return NULL;
    }
    memset(dp, 0, sizeof(cramfs_dir_t) + CRAMFS_MAXPATHLEN);

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_opendir(g_root, relpath, &dp->cramfs_dir);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret != 0) {
        krhino_mm_free(dp);
        dp = NULL;
    }

    krhino_mm_free(relpath);
    return (vfs_dir_t *)dp;
}

static vfs_dirent_t *cramfs_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    struct cramfs_dirent *dirent;
    cramfs_dir_t *dp = (cramfs_dir_t *)dir;

    memset(&dp->cur_dirent, 0, sizeof(vfs_dirent_t) + CRAMFS_MAXPATHLEN);

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    dirent = cramfs_readdir(&dp->cramfs_dir);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (!dirent) {
        return NULL;
    }

    dp->cur_dirent.d_ino = 0;
    dp->cur_dirent.d_type = dirent->d_inode.mode;
    memcpy(dp->cur_dirent.d_name, dirent->d_name, dirent->d_inode.namelen << 2);

    krhino_mm_free(dirent);
    return &(dp->cur_dirent);
}

static int32_t cramfs_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    int32_t ret;
    cramfs_dir_t *dp = (cramfs_dir_t *)dir;

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_closedir(&dp->cramfs_dir);
    krhino_mutex_unlock(&g_cramfs_lock);

    krhino_mm_free(dp);
    return ret;
}

static void cramfs_vfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    cramfs_dir_t *dp = (cramfs_dir_t *)dir;

    if (!dp) {
        return;
    }

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    dp->cramfs_dir.pos = 0;
    krhino_mutex_unlock(&g_cramfs_lock);

    return;
}

static int32_t cramfs_vfs_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    cramfs_dir_t *dp = (cramfs_dir_t *)dir;

    if (!dp) {
        return -1;
    }

    return (dp->cramfs_dir.pos);
}

static void cramfs_vfs_seekdir(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc)
{
    cramfs_dir_t *dp = (cramfs_dir_t *)dir;

    if (!dp) {
        return;
    }

    if (loc != 0) {
        cramfs_printk("Seek dir only allow SEEK_SET to zero !!!\n");
        return;
    }

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    dp->cramfs_dir.pos = loc;
    krhino_mutex_unlock(&g_cramfs_lock);

    return;
}

static int32_t cramfs_vfs_access(vfs_file_t *fp, const char *path, int32_t amode)
{
    char *relpath = NULL;
    struct cramfs_dirent *dirent = NULL;

    relpath = translate_relative_path(path);
    if (relpath == NULL) {
        return -1;
    }

    if ((amode == R_OK) || (amode == X_OK)) {
        return 0;
    }

    if (amode == W_OK) {
        return -1;
    }

    if (amode == F_OK) {
        dirent = cramfs_lookup(g_root, relpath);
        if (dirent == NULL) {
            return -1;
        } else {
            return 0;
        }
    }

    return -1;
}

static const vfs_filesystem_ops_t cramfs_ops = {
    .open       = &cramfs_vfs_open,
    .close      = &cramfs_vfs_close,
    .read       = &cramfs_vfs_read,
    .write      = NULL,
    .lseek      = &cramfs_vfs_lseek,
    .sync       = NULL,
    .stat       = &cramfs_vfs_stat,
    .statfs     = &cramfs_vfs_statfs,
    .unlink     = NULL,
    .rename     = NULL,
    .opendir    = &cramfs_vfs_opendir,
    .readdir    = &cramfs_vfs_readdir,
    .closedir   = &cramfs_vfs_closedir,
    .mkdir      = NULL,
    .rmdir      = NULL,
    .rewinddir  = &cramfs_vfs_rewinddir,
    .telldir    = &cramfs_vfs_telldir,
    .seekdir    = &cramfs_vfs_seekdir,
    .access     = &cramfs_vfs_access,
    .ioctl      = NULL
};

int32_t cramfs_register(uint32_t start_addr, uint32_t length)
{
    int32_t ret;

    ret = cramfs_flash_init(start_addr, length);
    if (ret != 0) {
        return -EIO;
    }

    krhino_mutex_create(&g_cramfs_lock, "CRAMFS");

    g_root = (struct cramfs_dirent *)krhino_mm_alloc(sizeof(struct cramfs_dirent));
    if (!g_root) {
        return -ENOMEM;
    }
    memset(g_root, 0, sizeof(struct cramfs_dirent));

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_mount(g_root);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret != 0) {
        krhino_mutex_del(&g_cramfs_lock);
        krhino_mm_free(g_root);
        return ret;
    }

    return vfs_register_fs(cramfs_mnt_path, &cramfs_ops, NULL);
}

int32_t cramfs_unregister(void)
{
    int32_t ret;

    krhino_mutex_lock(&g_cramfs_lock, RHINO_WAIT_FOREVER);
    ret = cramfs_umount(g_root);
    krhino_mutex_unlock(&g_cramfs_lock);

    if (ret != 0) {
        return ret;
    }

    krhino_mutex_del(&g_cramfs_lock);
    krhino_mm_free(g_root);

    return vfs_unregister_fs(cramfs_mnt_path);
}

