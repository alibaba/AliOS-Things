/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include "spiffs_config.h"
#include "spiffs.h"
#include "spiffs_nucleus.h"
#include <hal/hal.h>
#include <vfs_inode.h>
#include <vfs_register.h>

static const char* spiffs_mnt_path = "/spiffs";

typedef struct {
    spiffs *fs;
    aos_mutex_t lock;
    spiffs_config *cfg;
    uint8_t *work;
    uint32_t work_sz;
    uint8_t *fds;
    uint32_t fds_sz;
#if SPIFFS_CACHE
    uint8_t *cache;
    uint32_t cache_sz;
#endif
} spiffs_mgr_t;

typedef struct {
    aos_dir_t dir;
    spiffs_DIR d;
    aos_dirent_t cur_dirent;
}spiffs_dir_t;

static spiffs_mgr_t *g_spiffs_mgr = NULL;

static int32_t spiffs_hal_read(uint32_t addr, uint32_t size, uint8_t *dst)
{
    return hal_flash_read((hal_partition_t)SPIFFS_CFG_PARTITION, &addr, dst, size);
}

static int32_t spiffs_hal_write(uint32_t addr, uint32_t size, uint8_t *dst)
{
    return hal_flash_write((hal_partition_t)SPIFFS_CFG_PARTITION, &addr, dst, size);
}

static int32_t spiffs_hal_erase(uint32_t addr, uint32_t size)
{
    return hal_flash_erase((hal_partition_t)SPIFFS_CFG_PARTITION, addr, size);
}

void _spiffs_lock(spiffs *fs)
{
    aos_mutex_lock(&(((spiffs_mgr_t *)(fs->user_data))->lock), AOS_WAIT_FOREVER);
}

void _spiffs_unlock(spiffs *fs)
{
    aos_mutex_unlock(&(((spiffs_mgr_t *)(fs->user_data))->lock));
}

static char* translate_relative_path(const char *path)
{
    int len, prefix_len;
    char *relpath, *p;

    if (!path)
        return NULL;

    len = strlen(path);
    prefix_len = strlen(spiffs_mnt_path);
    if (strncmp(spiffs_mnt_path, path, prefix_len) != 0)
        return NULL;

    len = len - prefix_len;
    relpath = (char *)aos_malloc(len + 1);
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

static int _spiffs_ret_to_err(int ret)
{
    switch (ret) {
        case SPIFFS_OK:
            return 0;
        case SPIFFS_ERR_NOT_MOUNTED:
        case SPIFFS_ERR_NOT_A_FS:
            return -ENODEV;
        case SPIFFS_ERR_FULL:
            return -ENOSPC;
        case SPIFFS_ERR_BAD_DESCRIPTOR:
            return -EBADF;
        case SPIFFS_ERR_MOUNTED:
        case SPIFFS_ERR_FILE_EXISTS:
            return -EEXIST;
        case SPIFFS_ERR_NOT_FOUND:
        case SPIFFS_ERR_NOT_A_FILE:
        case SPIFFS_ERR_DELETED:
        case SPIFFS_ERR_FILE_DELETED:
            return -ENOENT;
        case SPIFFS_ERR_NAME_TOO_LONG:
            return -ENAMETOOLONG;
        case SPIFFS_ERR_RO_NOT_IMPL:
        case SPIFFS_ERR_RO_ABORTED_OPERATION:
            return -EROFS;
        default:
            return EIO;
    }
}

static int _spiffs_mode_conv(int flags)
{
    int acc_mode, res = 0;
    acc_mode = flags & O_ACCMODE;
    if (acc_mode == O_RDONLY) {
        res |= SPIFFS_O_RDONLY;
    } else if (acc_mode == O_WRONLY) {
        res |= SPIFFS_O_WRONLY;
    } else if (acc_mode == O_RDWR) {
        res |= SPIFFS_O_RDWR;
    }

    if ((flags & O_CREAT) && (flags & O_EXCL)) {
        res |= SPIFFS_O_CREAT | SPIFFS_O_EXCL;
    } else if ((flags & O_CREAT) && (flags & O_TRUNC)) {
        res |= SPIFFS_O_CREAT | SPIFFS_O_TRUNC;
    } else if (flags & O_APPEND) {
        res |= SPIFFS_O_CREAT | SPIFFS_O_APPEND;
    }
    return res;
}

static int _spiffs_open(file_t *fp, const char *path, int flags)
{
    int fd;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    fd = SPIFFS_open(g_spiffs_mgr->fs, relpath, _spiffs_mode_conv(flags), 0);
    if (fd > 0) {
        fp->f_arg = (void *)fd;
        fd = 0;
    } else {
        fd = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    aos_free(relpath);
    return fd;
}

static int _spiffs_close(file_t *fp)
{
    int fd;
    int ret = SPIFFS_ERR_FILE_CLOSED;

    fd = (int)(fp->f_arg);
    ret = SPIFFS_close(g_spiffs_mgr->fs, fd);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    } else {
        fp->f_arg = NULL;
    }

    return ret;
}

static ssize_t _spiffs_read(file_t *fp, char *buf, size_t len)
{
    ssize_t nbytes;
    int fd;

    fd = (int)(fp->f_arg);
    nbytes = SPIFFS_read(g_spiffs_mgr->fs, fd, buf, len);
    if (nbytes < 0) {
        nbytes = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    return nbytes;
}

static ssize_t _spiffs_write(file_t *fp, const char *buf, size_t len)
{
    ssize_t nbytes;
    int fd;

    fd = (int)(fp->f_arg);
    nbytes = SPIFFS_write(g_spiffs_mgr->fs, fd, (void *)buf, len);
    if (nbytes < 0) {
        nbytes = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    return nbytes;
}

static off_t _spiffs_lseek(file_t *fp, off_t off, int whence)
{
    off_t ret;
    int fd;

    fd = (int)(fp->f_arg);
    ret = SPIFFS_lseek(g_spiffs_mgr->fs, fd, off, whence);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    return ret;
}

static int _spiffs_sync(file_t *fp)
{
    int ret, fd;

    fd = (int)(fp->f_arg);
    ret = SPIFFS_fflush(g_spiffs_mgr->fs, fd);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    } else {
        ret = 0;
    }

    return ret;
}

static int _spiffs_stat(file_t *fp, const char *path, struct stat *st)
{
    int ret;
    spiffs_stat s;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    ret = SPIFFS_stat(g_spiffs_mgr->fs, relpath, &s);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    } else {
        st->st_size = s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == SPIFFS_TYPE_DIR)?S_IFDIR:S_IFREG);
    }

    aos_free(relpath);
    return ret;
}

static int _spiffs_unlink(file_t *fp, const char *path)
{
    int ret;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return -EINVAL;

    ret = SPIFFS_remove(g_spiffs_mgr->fs, relpath);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    aos_free(relpath);
    return ret;
}

static int _spiffs_rename(file_t *fp, const char *oldpath, const char *newpath)
{
    int ret;
    char *oldname = NULL;
    char *newname = NULL;

    oldname = translate_relative_path(oldpath);
    if (!oldname)
        return -EINVAL;

    newname = translate_relative_path(newpath);
    if (!newname) {
        aos_free(oldname);
        return -EINVAL;
    }

    ret = SPIFFS_rename(g_spiffs_mgr->fs, oldname, newname);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    aos_free(oldname);
    aos_free(newname);
    return ret;
}

static aos_dir_t* _spiffs_opendir(file_t *fp, const char *path)
{
    spiffs_dir_t *dp = NULL;
    char *relpath = NULL;

    relpath = translate_relative_path(path);
    if (!relpath)
        return NULL;

    dp = (spiffs_dir_t *)aos_malloc(sizeof(spiffs_dir_t) + SPIFFS_OBJ_NAME_LEN);
    if (!dp) {
        aos_free(relpath);
        return NULL;
    }

    memset(dp, 0, sizeof(spiffs_dir_t) + SPIFFS_OBJ_NAME_LEN);
    if (!SPIFFS_opendir(g_spiffs_mgr->fs, relpath, &dp->d)) {
        aos_free(relpath);
        aos_free(dp);
        SPIFFS_clearerr(g_spiffs_mgr->fs);
        return NULL;
    }
    
    aos_free(relpath);
    return (aos_dir_t *)dp;
}

static aos_dirent_t* _spiffs_readdir(file_t *fp, aos_dir_t *dir)
{
    spiffs_dir_t *dp;
    struct spiffs_dirent e;
    aos_dirent_t *out_dirent;

    dp = (spiffs_dir_t *)dir;
    if (!dp)
        return NULL;

    if (!SPIFFS_readdir(&dp->d, &e)) {
        SPIFFS_clearerr(g_spiffs_mgr->fs);
        return NULL;
    }

    if (e.name[0] == 0)
        return NULL;

    dp->cur_dirent.d_ino = 0;
    dp->cur_dirent.d_type = e.type;

    strncpy(dp->cur_dirent.d_name, (char *)e.name, SPIFFS_OBJ_NAME_LEN);
    dp->cur_dirent.d_name[SPIFFS_OBJ_NAME_LEN] = '\0';

    out_dirent = &dp->cur_dirent;
    return out_dirent;    
}

static int _spiffs_closedir(file_t *fp, aos_dir_t *dir)
{
    int ret;
    spiffs_dir_t *dp = (spiffs_dir_t *)dir;

    if (!dp)
        return -EINVAL;

    ret = SPIFFS_closedir(&dp->d);
    if (ret < 0) {
        ret = _spiffs_ret_to_err(SPIFFS_errno(g_spiffs_mgr->fs));
        SPIFFS_clearerr(g_spiffs_mgr->fs);
    }

    aos_free(dp);
    return ret;
}

static void _spiffs_deinit(void)
{
    if (!g_spiffs_mgr)
        return;

    if (g_spiffs_mgr->fs) {
        SPIFFS_unmount(g_spiffs_mgr->fs);
        aos_free(g_spiffs_mgr->fs);
    }

#if SPIFFS_CACHE
    if (g_spiffs_mgr->cache)
        aos_free(g_spiffs_mgr->cache);
#endif

    if (g_spiffs_mgr->fds)
        aos_free(g_spiffs_mgr->fds);

    if (g_spiffs_mgr->work)
        aos_free(g_spiffs_mgr->work);

    if (g_spiffs_mgr->cfg)
        aos_free(g_spiffs_mgr->cfg);

    aos_mutex_free(&g_spiffs_mgr->lock);
    aos_free(g_spiffs_mgr);
    g_spiffs_mgr = NULL;
}

static int _spiffs_init(void)
{
    if (g_spiffs_mgr)
        return 0;

    g_spiffs_mgr = (spiffs_mgr_t *)aos_malloc(sizeof(spiffs_mgr_t));
    if (!g_spiffs_mgr)
        return -ENOMEM;

    memset(g_spiffs_mgr, 0, sizeof(spiffs_mgr_t));

    /* init spiffs lock */
    if (aos_mutex_new(&g_spiffs_mgr->lock) != 0)
        goto err;

    /* init spiffs work buffer */
    g_spiffs_mgr->work_sz = CFG_SPIFFS_LOG_PAGE_SZ * 2;
    g_spiffs_mgr->work = (uint8_t *)aos_malloc(g_spiffs_mgr->work_sz);
    if (g_spiffs_mgr->work == NULL)
        goto err;
    memset(g_spiffs_mgr->work, 0, g_spiffs_mgr->work_sz);

    /* init spiffs fds */
    g_spiffs_mgr->fds_sz = sizeof(spiffs_fd) * CFG_SPIFFS_MAX_FILES;
    g_spiffs_mgr->fds = (uint8_t *)aos_malloc(g_spiffs_mgr->fds_sz);
    if (g_spiffs_mgr->fds == NULL)
        goto err;
    memset(g_spiffs_mgr->fds, 0, g_spiffs_mgr->fds_sz);

#if SPIFFS_CACHE
    /* init spiffs cache */
    g_spiffs_mgr->cache_sz = sizeof(spiffs_cache) + CFG_SPIFFS_MAX_FILES * 
                            (sizeof(spiffs_cache_page) + CFG_SPIFFS_LOG_PAGE_SZ);
    g_spiffs_mgr->cache = (uint8_t *)aos_malloc(g_spiffs_mgr->cache_sz);
    if (g_spiffs_mgr->cache == NULL)
        goto err;
    memset(g_spiffs_mgr->cache, 0, g_spiffs_mgr->cache_sz);
#endif

    /* init spiffs config */
    g_spiffs_mgr->cfg = (spiffs_config *)aos_malloc(sizeof(spiffs_config));
    if (g_spiffs_mgr->cfg == NULL)
        goto err;
    memset(g_spiffs_mgr->cfg, 0, sizeof(spiffs_config));

    g_spiffs_mgr->cfg->hal_read_f = spiffs_hal_read;
    g_spiffs_mgr->cfg->hal_write_f = spiffs_hal_write;
    g_spiffs_mgr->cfg->hal_erase_f = spiffs_hal_erase;

#if SPIFFS_SINGLETON == 0
    g_spiffs_mgr->cfg->phys_size = CFG_SPIFFS_PHYS_SZ;
    g_spiffs_mgr->cfg->phys_addr = CFG_SPIFFS_PHYS_ADDR;
    g_spiffs_mgr->cfg->phys_erase_block = CFG_SPIFFS_PHYS_ERASE_SZ;
    g_spiffs_mgr->cfg->log_block_size = CFG_SPIFFS_LOG_BLOCK_SZ;
    g_spiffs_mgr->cfg->log_page_size = CFG_SPIFFS_LOG_PAGE_SZ;
#endif

#if SPIFFS_FILEHDL_OFFSET
    g_spiffs_mgr->cfg->fh_ix_offset = SPIFFS_FILEHDL_OFFSET;
#endif

    /* init spiffs fs struct */
    g_spiffs_mgr->fs = (spiffs *)aos_malloc(sizeof(spiffs));
    if (g_spiffs_mgr->fs == NULL)
        goto err;
    memset(g_spiffs_mgr->fs, 0, sizeof(spiffs));
    
    g_spiffs_mgr->fs->user_data = (void *)g_spiffs_mgr->fs;

    return 0;

err:
    _spiffs_deinit();
    return -1;
}

static const fs_ops_t spiffs_ops = {
    .open       = &_spiffs_open,
    .close      = &_spiffs_close,
    .read       = &_spiffs_read,
    .write      = &_spiffs_write,
    .lseek      = &_spiffs_lseek,
    .sync       = &_spiffs_sync,
    .stat       = &_spiffs_stat,
    .unlink     = &_spiffs_unlink,
    .rename     = &_spiffs_rename,
    .opendir    = &_spiffs_opendir,
    .readdir    = &_spiffs_readdir,
    .closedir   = &_spiffs_closedir,
    .mkdir      = NULL,
    .ioctl      = NULL
};

int vfs_spiffs_register(void)
{
    int ret = SPIFFS_OK;

    ret = _spiffs_init();
    if (ret != SPIFFS_OK)
        return ret;

    // first try to mount
    ret = SPIFFS_mount(g_spiffs_mgr->fs, g_spiffs_mgr->cfg,
                       g_spiffs_mgr->work, g_spiffs_mgr->fds, g_spiffs_mgr->fds_sz,
                       g_spiffs_mgr->cache, g_spiffs_mgr->cache_sz, 0);

    if (ret != SPIFFS_OK) {
        if (ret == SPIFFS_ERR_NOT_A_FS) {
            if (SPIFFS_format(g_spiffs_mgr->fs) == SPIFFS_OK) {
                // second try to mount (already formatted)
                ret = SPIFFS_mount(g_spiffs_mgr->fs, g_spiffs_mgr->cfg,
                       g_spiffs_mgr->work, g_spiffs_mgr->fds, g_spiffs_mgr->fds_sz,
                       g_spiffs_mgr->cache, g_spiffs_mgr->cache_sz, 0);
                if (ret != SPIFFS_OK)
                    goto err;
            } else
                goto err;
        } else
            goto err;
    }

    return aos_register_fs(spiffs_mnt_path, &spiffs_ops, NULL);

err:
    _spiffs_deinit();
    return ret;
}

int vfs_spiffs_unregister(void)
{
    SPIFFS_unmount(g_spiffs_mgr->fs);
    _spiffs_deinit();
    return aos_unregister_fs(spiffs_mnt_path);
}
