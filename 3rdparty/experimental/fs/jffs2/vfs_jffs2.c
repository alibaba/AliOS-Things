/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <k_api.h>
#include <errno.h>
#include "vfs_inode.h"
#include "vfs_register.h"
#include <hal/soc/nor.h>
#include "jffs2_port.h"
#include "include/linux/jffs2.h"
#include "include/os-alios.h"

static const char* jffs2_mnt_path = "/jffs2";

static kmutex_t g_jffs2_lock;
static jffs2_mtab_entry *g_mte = NULL;
static nor_dev_t nor_dev;

typedef struct _jffs2_dir_t
{
    aos_dir_t dir;
    aos_dirent_t cur_dirent;
} jffs2_dir_t;

static char* translate_relative_path(const char *path)
{
    int len, prefix_len;
    char *relpath, *p;

    if (!path)
        return NULL;

    len = strlen(path);
    prefix_len = strlen(jffs2_mnt_path);
    if (strncmp(jffs2_mnt_path, path, prefix_len) != 0)
        return NULL;

    len = len - prefix_len;

    relpath = (char *)krhino_mm_alloc(len + 1);
    if (!relpath)
        return NULL;

    memset(relpath, 0, len + 1);
    if (len > 0) {
        p = (char *)(path + prefix_len + 1);
        memcpy(relpath, p, len - 1);
    }

    relpath[len] = '\0';

    return relpath;
}

static int jffs2_result_to_vfs(int res)
{
    if (res < 0) return res;
    if (res > 0) return -res;
    return 0;
}

static int _jffs2_open(file_t *fp, const char *path, int flags)
{
    int ret;
    char *relpath = NULL;
    jffs2_file *file = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    file = (jffs2_file *)krhino_mm_alloc(sizeof(jffs2_file));
    if (!file)
        return -ENOMEM;

    file->f_mte = g_mte;
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_open(g_mte, 0, relpath, flags, file);
    krhino_mutex_unlock(&g_jffs2_lock);
    if (ret == 0) {
        fp->f_arg = (void *)file;
    } else {
        krhino_mm_free(file);
    }

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);
}

static int _jffs2_close(file_t *fp)
{
    int ret;
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_close(file);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret == 0) {
        krhino_mm_free(file);
        fp->f_arg = NULL;
    }

    return jffs2_result_to_vfs(ret);
}

static ssize_t _jffs2_read(file_t *fp, char *buf, size_t len)
{
    ssize_t nbytes;
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    nbytes = jffs2_fo_read(file, buf, len);
    krhino_mutex_unlock(&g_jffs2_lock);

    return nbytes;
}

static ssize_t _jffs2_write(file_t *fp, const char *buf, size_t len)
{
    ssize_t nbytes;
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    nbytes = jffs2_fo_write(file, buf, len);
    krhino_mutex_unlock(&g_jffs2_lock);

    return nbytes;
}

static off_t _jffs2_lseek(file_t *fp, off_t off, int whence)
{
    int ret;
    off_t pos;
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    pos = off;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_lseek(file, &pos, whence);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret == 0)
        return pos;

    return jffs2_result_to_vfs(ret);
}

static int _jffs2_sync(file_t *fp)
{
    int ret;
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_fsync(file, 0);
    krhino_mutex_unlock(&g_jffs2_lock);

    return jffs2_result_to_vfs(ret);
}

static int _jffs2_stat(file_t *fp, const char *path, struct stat *st)
{
    int ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_stat(g_mte, g_mte->root, relpath, st);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);
}

static int _jffs2_unlink(file_t *fp, const char *path)
{
    int ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_ops_unlink(g_mte, g_mte->root, relpath);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);
}

static int _jffs2_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    int ret;
    char *oldname = NULL;
    char *newname = NULL;

    oldname = translate_relative_path(oldpath);
    if (!oldname)
        return -EINVAL;

    newname = translate_relative_path(newpath);
    if (!newname) {
        krhino_mm_free(oldname);
        return -EINVAL;
    }

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_ops_rename(g_mte, g_mte->root, oldname, g_mte->root, newname);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(oldname);
    krhino_mm_free(newname);
    return jffs2_result_to_vfs(ret);
}

static aos_dir_t* _jffs2_opendir(file_t *fp, const char *path)
{
    jffs2_dir_t *dp = NULL;
    jffs2_file *file = NULL;
    char *relpath = NULL;
    int ret;

    relpath = translate_relative_path(path);
    if (!relpath)
        return NULL;

    file = (jffs2_file *)krhino_mm_alloc(sizeof(jffs2_file));
    if (!file) {
        krhino_mm_free(relpath);
        return NULL;
    }

    dp = (jffs2_dir_t *)krhino_mm_alloc(sizeof(jffs2_dir_t) + JFFS2_PATH_MAX);
    if (!dp) {
        krhino_mm_free(file);
        krhino_mm_free(relpath);
        return NULL;
    }
    memset(dp, 0, sizeof(jffs2_dir_t) + JFFS2_PATH_MAX);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_opendir(g_mte, g_mte->root, relpath, file);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret == 0) {
        fp->f_arg = (void *)file;
    } else {
        krhino_mm_free(file);
        krhino_mm_free(dp);
        dp = NULL;
    }

    krhino_mm_free(relpath);
    return (aos_dir_t *)dp;
}

static aos_dirent_t* _jffs2_readdir(file_t *fp, aos_dir_t *dir)
{
    jffs2_dir_t *dp = (jffs2_dir_t *)dir;
    jffs2_file *file = NULL;
    int ret;

    file = (jffs2_file *)(fp->f_arg);
    memset(&dp->cur_dirent, 0, sizeof(aos_dirent_t) + JFFS2_PATH_MAX);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_dirread(file, &dp->cur_dirent, sizeof(aos_dirent_t) + JFFS2_PATH_MAX);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret <= 0) {
        return NULL;
    }

    return &(dp->cur_dirent);
}

static int _jffs2_closedir(file_t *fp, aos_dir_t *dir)
{
    int ret;
    jffs2_file *file = NULL;
    jffs2_dir_t *dp = (jffs2_dir_t *)dir;

    file = (jffs2_file *)(fp->f_arg);
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_close(file);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(file);
    krhino_mm_free(dp);

    return jffs2_result_to_vfs(ret);
}

static int _jffs2_mkdir(file_t *fp, const char *path)
{
    int ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_ops_mkdir(g_mte, g_mte->root, relpath);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);
}

static int _jffs2_rmdir(file_t *fp, const char *path)
{
    int ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_ops_rmdir(g_mte, g_mte->root, relpath);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);

}

static void _jffs2_rewinddir(file_t *fp, aos_dir_t *dir)
{
    jffs2_file *file = (jffs2_file *)(fp->f_arg);

    if (!file)
        return;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    file->f_offset = 0;
    krhino_mutex_unlock(&g_jffs2_lock);

    return;
}

static long _jffs2_telldir(file_t *fp, aos_dir_t *dir)
{
    jffs2_file *file = (jffs2_file *)(fp->f_arg);

    if (!file)
        return -1;

    return (file->f_offset);
}

static void _jffs2_seekdir(file_t *fp, aos_dir_t *dir, long loc)
{
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    if (!file)
        return;

    if (loc != 0) {
        printf("Seek dir only allow SEEK_SET to zero !!!\n");
        return;
    }

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    file->f_offset = loc;
    krhino_mutex_unlock(&g_jffs2_lock);

    return;
}

static int _jffs2_statfs(file_t *fp, const char *path, struct statfs *buf)
{
    struct super_block *jffs2_sb_data = NULL;

    if (buf == NULL) {
        return -1;
    }

    jffs2_sb_data = (struct super_block *)(g_mte->data);

    buf->f_type = JFFS2_MAGIC_BITMASK;
    buf->f_bsize = jffs2_sb_data->jffs2_sb.flash_size;
    buf->f_blocks = jffs2_sb_data->jffs2_sb.flash_size / jffs2_sb_data->jffs2_sb.sector_size;
    buf->f_bfree = jffs2_sb_data->jffs2_sb.free_size / jffs2_sb_data->jffs2_sb.sector_size;
    buf->f_bavail = jffs2_sb_data->jffs2_sb.flash_size / jffs2_sb_data->jffs2_sb.sector_size;
    buf->f_files = 0xFFFFFFFF;
    buf->f_ffree = 0xFFFFFFFF;
    buf->f_fsid = 2;
    buf->f_namelen = JFFS2_MAX_NAME_LEN;

    return 0;
}

int _jffs2_access(file_t *fp, const char *path, int amode)
{
    char *relpath = NULL;
    int ret = -1;

    relpath = translate_relative_path(path);
    if (relpath == NULL) {
        return -1;
    }

    ret = jffs2_access((struct _inode *)g_mte->root, relpath, amode);

    return ret;
}

static const fs_ops_t _jffs2_ops = {
    .open       = &_jffs2_open,
    .close      = &_jffs2_close,
    .read       = &_jffs2_read,
    .write      = &_jffs2_write,
    .lseek      = &_jffs2_lseek,
    .sync       = &_jffs2_sync,
    .stat       = &_jffs2_stat,
    .statfs     = &_jffs2_statfs,
    .unlink     = &_jffs2_unlink,
    .rename     = &_jffs2_rename,
    .opendir    = &_jffs2_opendir,
    .readdir    = &_jffs2_readdir,
    .closedir   = &_jffs2_closedir,
    .mkdir      = &_jffs2_mkdir,
    .rmdir      = &_jffs2_rmdir,
    .rewinddir  = &_jffs2_rewinddir,
    .telldir    = &_jffs2_telldir,
    .seekdir    = &_jffs2_seekdir,
    .access     = &_jffs2_access,
    .ioctl      = NULL
};

int vfs_jffs2_register(uint32_t start_addr, uint32_t length)
{
    int ret;

    nor_dev.base_addr = start_addr;
    nor_dev.config.chip_size = length;

    ret = hal_nor_init(&nor_dev);
    if (ret != 0)
        return jffs2_result_to_vfs(ret);

    krhino_mutex_create(&g_jffs2_lock, "G_JFFS2");

    g_mte = (jffs2_mtab_entry *)krhino_mm_alloc(sizeof(jffs2_mtab_entry));
    if (!g_mte)
        return -ENOMEM;
    memset(g_mte, 0, sizeof(jffs2_mtab_entry));

    g_mte->data = (uint32_t)(&nor_dev);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_mount(g_mte);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret != 0) {
        krhino_mutex_del(&g_jffs2_lock);
        krhino_mm_free(g_mte);
        return jffs2_result_to_vfs(ret);
    }

    return aos_register_fs(jffs2_mnt_path, &_jffs2_ops, NULL);
}

int vfs_jffs2_unregister(void)
{
    int ret;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_umount(g_mte);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret != 0)
        return jffs2_result_to_vfs(ret);

    krhino_mutex_del(&g_jffs2_lock);
    krhino_mm_free(g_mte);

    return aos_unregister_fs(jffs2_mnt_path);
}
