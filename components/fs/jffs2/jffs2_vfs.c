/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <k_api.h>
#include <errno.h>

#include "fs/jffs2.h"
#include "vfs_types.h"
#include "vfs_api.h"

#include "aos/hal/nor.h"

#include "jffs2_port.h"
#include "include/linux/jffs2.h"
#include "include/os-alios.h"

static const char* jffs2_mnt_path = "/jffs2";

static kmutex_t g_jffs2_lock;
static jffs2_mtab_entry *g_mte = NULL;
static nor_dev_t nor_dev;

typedef struct _jffs2_dir_t
{
    vfs_dir_t dir;
    vfs_dirent_t cur_dirent;
} jffs2_dir_t;

static char* translate_relative_path(const char *path)
{
    int32_t len, prefix_len;

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

static int32_t jffs2_result_to_vfs(int32_t res)
{
    if (res < 0) return res;
    if (res > 0) return -res;
    return 0;
}

static int32_t jffs2_vfs_open(vfs_file_t *fp, const char *path, int32_t flags)
{
    int32_t ret;

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

static int32_t jffs2_vfs_close(vfs_file_t *fp)
{
    int32_t ret;

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

static int32_t jffs2_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    int32_t nbytes;

    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    nbytes = jffs2_fo_read(file, buf, len);
    krhino_mutex_unlock(&g_jffs2_lock);

    return nbytes;
}

static int32_t jffs2_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    int32_t nbytes;

    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    nbytes = jffs2_fo_write(file, buf, len);
    krhino_mutex_unlock(&g_jffs2_lock);

    return nbytes;
}

static uint32_t jffs2_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    int32_t ret;
    int64_t pos;

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

static int32_t jffs2_vfs_sync(vfs_file_t *fp)
{
    int32_t ret;

    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_fsync(file, 0);
    krhino_mutex_unlock(&g_jffs2_lock);

    return jffs2_result_to_vfs(ret);
}

static int32_t jffs2_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    int32_t ret;

    char *relpath = NULL;
    struct stat file_st;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_stat(g_mte, g_mte->root, relpath, &file_st);
    krhino_mutex_unlock(&g_jffs2_lock);

    st->st_mode = file_st.st_mode;
    st->st_size = file_st.st_size;

    krhino_mm_free(relpath);
    return jffs2_result_to_vfs(ret);
}

static int32_t jffs2_vfs_unlink(vfs_file_t *fp, const char *path)
{
    int32_t ret;

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

static int32_t jffs2_vfs_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    int32_t ret;

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

static vfs_dir_t* jffs2_vfs_opendir(vfs_file_t *fp, const char *path)
{
    jffs2_dir_t *dp   = NULL;
    jffs2_file  *file = NULL;

    char *relpath = NULL;

    int32_t ret;

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

static vfs_dirent_t* jffs2_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    jffs2_dir_t *dp = (jffs2_dir_t *)dir;
    jffs2_file *file = NULL;
    int ret;

    file = (jffs2_file *)(fp->f_arg);
    memset(&dp->cur_dirent, 0, sizeof(vfs_dirent_t) + JFFS2_PATH_MAX);

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_dirread(file, &dp->cur_dirent, sizeof(vfs_dirent_t) + JFFS2_PATH_MAX);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret <= 0) {
        return NULL;
    }

    return &(dp->cur_dirent);
}

static int32_t jffs2_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    int32_t ret;

    jffs2_file  *file = NULL;
    jffs2_dir_t *dp   = (jffs2_dir_t *)dir;

    file = (jffs2_file *)(fp->f_arg);
    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_fo_close(file);
    krhino_mutex_unlock(&g_jffs2_lock);

    krhino_mm_free(file);
    krhino_mm_free(dp);

    return jffs2_result_to_vfs(ret);
}

static int32_t jffs2_vfs_mkdir(vfs_file_t *fp, const char *path)
{
    int32_t ret;

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

static int32_t jffs2_vfs_rmdir(vfs_file_t *fp, const char *path)
{
    int32_t ret;

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

static void jffs2_vfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    jffs2_file *file = (jffs2_file *)(fp->f_arg);

    if (!file)
        return;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    file->f_offset = 0;
    krhino_mutex_unlock(&g_jffs2_lock);

    return;
}

static int32_t jffs2_vfs_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    jffs2_file *file = (jffs2_file *)(fp->f_arg);

    if (!file)
        return -1;

    return (file->f_offset);
}

static void jffs2_vfs_seekdir(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc)
{
    jffs2_file *file = NULL;

    file = (jffs2_file *)(fp->f_arg);
    if (!file)
        return;

    if (loc != 0) {
        return;
    }

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    file->f_offset = loc;
    krhino_mutex_unlock(&g_jffs2_lock);

    return;
}

static int32_t jffs2_vfs_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *buf)
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

static const vfs_filesystem_ops_t jffs2_ops = {
    .open       = &jffs2_vfs_open,
    .close      = &jffs2_vfs_close,
    .read       = &jffs2_vfs_read,
    .write      = &jffs2_vfs_write,
    .lseek      = &jffs2_vfs_lseek,
    .sync       = &jffs2_vfs_sync,
    .stat       = &jffs2_vfs_stat,
    .statfs     = &jffs2_vfs_statfs,
    .unlink     = &jffs2_vfs_unlink,
    .rename     = &jffs2_vfs_rename,
    .opendir    = &jffs2_vfs_opendir,
    .readdir    = &jffs2_vfs_readdir,
    .closedir   = &jffs2_vfs_closedir,
    .mkdir      = &jffs2_vfs_mkdir,
    .rmdir      = &jffs2_vfs_rmdir,
    .rewinddir  = &jffs2_vfs_rewinddir,
    .telldir    = &jffs2_vfs_telldir,
    .seekdir    = &jffs2_vfs_seekdir,
    .access     = &jffs2_vfs_access,
    .ioctl      = NULL
};

int32_t jffs2_register(uint32_t start_addr, uint32_t length)
{
    int32_t ret;

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

    return vfs_register_fs(jffs2_mnt_path, &_jffs2_ops, NULL);
}

int32_t jffs2_unregister(void)
{
    int32_t ret;

    krhino_mutex_lock(&g_jffs2_lock, RHINO_WAIT_FOREVER);
    ret = jffs2_umount(g_mte);
    krhino_mutex_unlock(&g_jffs2_lock);

    if (ret != 0)
        return jffs2_result_to_vfs(ret);

    krhino_mutex_del(&g_jffs2_lock);
    krhino_mm_free(g_mte);

    return vfs_unregister_fs(jffs2_mnt_path);
}
