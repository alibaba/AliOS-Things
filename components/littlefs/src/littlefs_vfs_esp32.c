/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/statfs.h>
#include "lfs.h"
#include "lfs_util.h"
#include "lfs_conf.h"
#include "aos/kernel.h"

#include "aos_hal_flash.h"
#include "sys/errno.h"
#ifdef FS_TIMESTAMP_WORKAROUND
#include <time.h>
#endif

#include "sdkconfig.h"
#include "littlefs.h"

#include "esp_vfs.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* lookahead size should be 8 bytes aligned */
#define _ALIGN8(n) (((n) + 7) & (~7))
#define GET_LOOKAHEAD_SIZE(blks) (_ALIGN8((blks) >> 4) > 16 ? _ALIGN8((blks) >> 4) : 16)

#define LFS_INVALID_SIZE ((lfs_size_t)(-1))
#define WAIT_FOREVER 0xFFFFFFFF

typedef aos_mutex_t lfs_lock_t;

hal_partition_t _lfs_parttion[CONFIG_LITTLEFS_CNT] = {
    HAL_PARTITION_LITTLEFS,
#if defined(CONFIG_MULTI_FS)
    HAL_PARTITION_LITTLEFS2
#endif
};

typedef struct {
    struct lfs_config *config;
    lfs_t             *lfs;
    lfs_lock_t        *lock;
    hal_partition_t   part;
    int               fd;
    char              *mountpath;
    bool              mounted;
    int max_files;
    lfs_file_t *files;
} lfs_manager_t;

typedef struct _lfsvfs_dir_t {
    DIR    dir;
    lfs_dir_t    lfsdir;
    struct dirent cur_dirent;
} lfsvfs_dir_t;

static bool g_partition_formatted[CONFIG_LITTLEFS_CNT] = {false};
static lfs_manager_t *g_lfs_manager[CONFIG_LITTLEFS_CNT] = {NULL};

static const char *aos_task_name(void)
{
    aos_task_t task;
    TaskHandle_t ptask;
    task = aos_task_self();
    ptask = (TaskHandle_t)(task);
    return pcTaskGetTaskName(ptask);
}
static lfs_size_t littlefs_mtd_get_block_cnt(const struct lfs_config *c)
{
    lfs_size_t ret = LFS_INVALID_SIZE;
    int err;
    hal_logic_partition_t *pPartitionInfo;
    int idx;
    hal_partition_t part = *(int *)c->context;
    err = aos_hal_flash_info_get(part, &pPartitionInfo);
    if (err == 0) {
        ret = pPartitionInfo->partition_length / c->block_size;
    }
    return ret;
}

static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, void *dst, lfs_size_t size)
{
    int ret;
    hal_partition_t part = *(int *)c->context;
    uint32_t addr;
    addr = c->block_size * block + off;
    ret = aos_hal_flash_read(part, &addr, dst, size);
    if (ret) {
        LFS_ERROR("%s ret: %d", __func__, ret);
    }
    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *dst, lfs_size_t size)
{
    int ret;
    hal_partition_t part = *(int *)c->context;
    uint32_t addr;
    addr = c->block_size * block + off;
    ret = aos_hal_flash_write(part, &addr, dst, size);
    if (ret) {
        LFS_ERROR("%s ret: %d", __func__, ret);
    }
    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret;
    hal_partition_t part = *(int *)c->context;
    uint32_t addr;
    addr = c->block_size * block;
    ret = aos_hal_flash_erase(part, addr, c->block_size);
    if (ret) {
        LFS_ERROR("%s ret: %d", __func__, ret);
    }
    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_sync(const struct lfs_config *c)
{
    return 0;
}

struct lfs_config default_cfg[CONFIG_LITTLEFS_CNT] = {
    {
        // block device operations
        .read  = littlefs_block_read,
        .prog  = littlefs_block_write,
        .erase = littlefs_block_erase,
        .sync  = littlefs_block_sync,
#ifdef NFTL_GC_NOTIFY
        .nty_gc = littlefs_block_notify_gabage,
#endif

        // block device configuration
        .read_size = 256,
        .prog_size = 256,
        .block_size = 32768,
        .block_count = 152,
        .cache_size = 256,
        .lookahead_size = 16,
        .block_cycles = 1000,
    },
#if defined(CONFIG_MULTI_FS)
    {
        // block device operations
        .read  = littlefs_block_read,
        .prog  = littlefs_block_write,
        .erase = littlefs_block_erase,
        .sync  = littlefs_block_sync,
#ifdef NFTL_GC_NOTIFY
        .nty_gc = littlefs_block_notify_gabage,
#endif

        // block device configuration
        .read_size = 256,
        .prog_size = 256,
        .block_size = 32768,
        .block_count = 152,
        .cache_size = 256,
        .lookahead_size = 16,
        .block_cycles = 1000,
    },
#endif
};

/* Global FS lock init */
static void lfs_lock_create(lfs_lock_t *lock)
{
    if (lock) {
        aos_mutex_new(lock);
        LFS_TRACE("%s: lock %p created", aos_task_name(), lock);
    }
}

/* Global FS lock destroy */
static void lfs_lock_destory(lfs_lock_t *lock)
{
    if (lock) {
        aos_mutex_free(lock);
        LFS_TRACE("%s: lock %p destroyed", aos_task_name(), lock);
    }
}

static void lfs_lock(lfs_lock_t *lock)
{
    if (lock) {
        aos_mutex_lock(lock, WAIT_FOREVER);
        LFS_TRACE("%s: lock %p locked", aos_task_name(), lock);
    }
}

static void lfs_unlock(lfs_lock_t *lock)
{
    if (lock) {
        LFS_TRACE("%s: lock %p to be unlocked", aos_task_name(), lock);
        aos_mutex_unlock(lock);
    }
}

static int32_t mode_convert(int32_t flags)
{
    int32_t mode, res = 0;

    mode = flags & O_ACCMODE;
    if (mode == O_RDONLY) {
        res |= LFS_O_RDONLY;
    } else if (mode == O_WRONLY) {
        res |= LFS_O_WRONLY;
    } else if (mode == O_RDWR) {
        res |= LFS_O_RDWR;
    }

    if (flags & O_CREAT) {
        res |= LFS_O_CREAT;
    }
    if (flags & O_EXCL) {
        res |= LFS_O_EXCL;
    }
    if (flags & O_TRUNC) {
        res |= LFS_O_TRUNC;
    }
    if (flags & O_APPEND) {
        res |= LFS_O_CREAT | LFS_O_APPEND;
    }
    return res;
}

static int lfs_ret_value_convert(int lfs_ret)
{
    int ret;
    if (lfs_ret > 0) {
        return lfs_ret;
    }
    switch (lfs_ret) {
    case LFS_ERR_OK:
        ret = 0;
        break;
    case LFS_ERR_IO:
        ret = -EIO;
        break;
    case LFS_ERR_CORRUPT:
        ret = -EIO;
        break;
    case LFS_ERR_NOENT:
        ret = -ENOENT;
        break;
    case LFS_ERR_EXIST:
        ret = -EEXIST;
        break;
    case LFS_ERR_NOTDIR:
        ret = -ENOTDIR;
        break;
    case LFS_ERR_ISDIR:
        ret = -EISDIR;
        break;
    case LFS_ERR_NOTEMPTY:
        ret = -ENOTEMPTY;
        break;
    case LFS_ERR_BADF:
        ret = -EBADF;
        break;
    case LFS_ERR_FBIG:
        ret = -EFBIG;
        break;
    case LFS_ERR_INVAL:
        ret = -EINVAL;
        break;
    case LFS_ERR_NOSPC:
        ret = -ENOSPC;
        break;
    case LFS_ERR_NOMEM:
        ret = -ENOMEM;
        break;
    case LFS_ERR_NOATTR:
        ret = -ENODATA;
        break;
    case LFS_ERR_NAMETOOLONG:
        ret = -ENAMETOOLONG;
        break;
    default:
        ret = lfs_ret;
        break;
    }
    return ret;
}


static int32_t _lfs_deinit(void)
{
    int i = 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i] == NULL) {
            continue;
        }

        if (g_lfs_manager[i]->lock != NULL) {
            lfs_lock_destory(g_lfs_manager[i]->lock);
        }

    #ifndef LFS_STATIC_OBJECT
        if (g_lfs_manager[i]->lfs != NULL) {
            lfs_free(g_lfs_manager[i]->lfs);
        }

        if (g_lfs_manager[i]->lock != NULL) {
            lfs_free(g_lfs_manager[i]->lock);
        }

        memset(g_lfs_manager[i], 0, sizeof(lfs_manager_t));
        if (g_lfs_manager[i]) {
            lfs_free(g_lfs_manager[i]);
        }
    #endif

        close(g_lfs_manager[i]->fd);

        if (g_lfs_manager[i]->config) {
            if (g_lfs_manager[i]->config->read_buffer) {
                lfs_free(g_lfs_manager[i]->config->read_buffer);
            }
            if (g_lfs_manager[i]->config->prog_buffer) {
                lfs_free(g_lfs_manager[i]->config->prog_buffer);
            }
            if (g_lfs_manager[i]->config->lookahead_buffer) {
                lfs_free(g_lfs_manager[i]->config->lookahead_buffer);
            }
        }
        if (g_lfs_manager[i]->files) {
            lfs_free(g_lfs_manager[i]->files);
        }
        g_lfs_manager[i] = NULL;

        i++;
    }

    return 0;
}

static lfs_manager_t native_lfs_manager[CONFIG_LITTLEFS_CNT] = {
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS, -1, NULL, false, 0, NULL},
#if defined(CONFIG_MULTI_FS)
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS2, -1, NULL, false, 0, NULL},
#endif /* CONFIG_MULTI_FS */
};

static lfs_t native_lfs[CONFIG_LITTLEFS_CNT];

static lfs_lock_t native_lfs_lock[CONFIG_LITTLEFS_CNT];

static int32_t _lfs_init(void)
{
    int i = 0;
    /* Create LFS struct */
    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i] != NULL) {
            return 0;
        }

        g_lfs_manager[i] = &native_lfs_manager[i];
        if (g_lfs_manager[i] == NULL) {
            goto ERROR;
        }
        memset(g_lfs_manager[i], 0, sizeof(lfs_manager_t));
        /* Set LFS partition index and mount point. */
        g_lfs_manager[i]->part = _lfs_parttion[i];

        if (i == 0) {
            g_lfs_manager[i]->mountpath = CONFIG_LFS_MOUNTPOINT;
        }
#if defined(CONFIG_MULTI_FS)
        else {
            g_lfs_manager[i]->mountpath = CONFIG_LFS_MOUNTPOINT2;
        }
#endif /* CONFIG_MULTI_FS */
        /* Set LFS default config */
        g_lfs_manager[i]->config = &default_cfg[i];
        g_lfs_manager[i]->config->context = &(g_lfs_manager[i]->part);
        // block device configuration
        default_cfg[i].read_size = CONFIG_LFS_PROG_SIZE;
        default_cfg[i].prog_size = CONFIG_LFS_PROG_SIZE;
        default_cfg[i].block_size = CONFIG_LFS_PROG_SIZE * CONFIG_LFS_PAGE_NUM_PER_BLOCK;
        LFS_ASSERT(default_cfg[i].read_size != LFS_INVALID_SIZE);
        LFS_ASSERT(default_cfg[i].prog_size != LFS_INVALID_SIZE);
        LFS_ASSERT(default_cfg[i].block_size != LFS_INVALID_SIZE);

        default_cfg[i].block_count = littlefs_mtd_get_block_cnt(g_lfs_manager[i]->config);
        default_cfg[i].lookahead_size = GET_LOOKAHEAD_SIZE(default_cfg[i].block_count);
        default_cfg[i].cache_size = default_cfg[i].prog_size;

        g_lfs_manager[i]->config->read_buffer = \
           lfs_malloc(default_cfg[i].read_size);
        g_lfs_manager[i]->config->prog_buffer = \
            lfs_malloc(default_cfg[i].prog_size);
        g_lfs_manager[i]->config->lookahead_buffer = \
            lfs_malloc(default_cfg[i].lookahead_size);
        LFS_ASSERT(default_cfg[i].read_buffer);
        LFS_ASSERT(default_cfg[i].prog_buffer);
        LFS_ASSERT(default_cfg[i].lookahead_buffer);
        g_lfs_manager[i]->max_files = CONFIG_MAX_FILES;
        g_lfs_manager[i]->files = \
            (lfs_file_t *)lfs_malloc(CONFIG_MAX_FILES * sizeof(lfs_file_t));
        LFS_ASSERT(g_lfs_manager[i]->files);
        memset(g_lfs_manager[i]->files, 0, CONFIG_MAX_FILES * sizeof(lfs_file_t));
    #ifdef AOS_COMP_NFTL
        default_cfg[i].block_cycles = -1;
    #else
        default_cfg[i].block_cycles = 500;
    #endif
        {
            /* Create LFS Global Lock */
            g_lfs_manager[i]->lock = &native_lfs_lock[i];
            if (g_lfs_manager[i]->lock == NULL) {
                goto ERROR;
            }
            lfs_lock_create(g_lfs_manager[i]->lock);
        }
        /* Create LFS struct */
        g_lfs_manager[i]->lfs = &native_lfs[i];
        if (g_lfs_manager[i]->lfs == NULL) {
            goto ERROR;
        }

        i++;
    }

    return 0;

ERROR:
    _lfs_deinit();
    return -1;
}

static int get_next_fd(lfs_manager_t *lfs_manager)
{
    for (size_t i = 0; i < lfs_manager->max_files; ++i) {
        if ((lfs_manager->files[i].flags & LFS_F_OPENED) == 0) {
            return (int) i;
        }
    }
    return -1;
}

static void file_cleanup(lfs_manager_t *lfs_manager, int fd)
{
    memset(&lfs_manager->files[fd], 0, sizeof(lfs_manager_t));
}

static int32_t lfs_vfs_open(void *ctx, const char *path, int flags, int mode)
{
    int ret;
    lfs_file_t *file = NULL;
    lfs_manager_t *lfs_manager = NULL;
    int fd;
    lfs_manager = (lfs_manager_t *)ctx;
    lfs_lock(lfs_manager->lock);
    fd = get_next_fd(lfs_manager);
    if (fd < 0) {
        lfs_unlock(lfs_manager->lock);
        return -EINVAL;
    }
    file = &(lfs_manager->files[fd]);
    ret = lfs_file_open(lfs_manager->lfs, file,  path, mode_convert(flags));
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_DEBUG("%s %s failed, ret - %d", __func__, path, ret);
        file_cleanup(lfs_manager, fd);
        errno = -ret;
        return -1;
    }
    return fd;
}

static int32_t lfs_vfs_close(void *ctx, int fd)
{
    int ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfs_lock(lfs_manager->lock);
    lfs_file_t *file = &lfs_manager->files[fd];
    ret = lfs_file_close(lfs_manager->lfs, file);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_DEBUG("%s failed, ret - %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    file_cleanup(lfs_manager, fd);
    return ret;
}

static int32_t lfs_vfs_read(void *ctx, int fd, void *dst, size_t size)
{
    int ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfs_file_t *file = (lfs_file_t *)&(lfs_manager->files[fd]);

    lfs_lock(lfs_manager->lock);
    ret = lfs_file_read(lfs_manager->lfs, file, dst, size);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_write(void *ctx, int fd, const void *data, size_t size)
{
    int ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfs_file_t *file = (lfs_file_t *)&(lfs_manager->files[fd]);

    lfs_lock(lfs_manager->lock);
    ret = lfs_file_write(lfs_manager->lfs, file, data, size);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static off_t lfs_vfs_lseek(void *ctx, int fd, off_t offset, int mode)
{
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfs_file_t *file = (lfs_file_t *)&(lfs_manager->files[fd]);
    lfs_lock(lfs_manager->lock);
    ret = lfs_file_seek(lfs_manager->lfs, file, offset, mode);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_fsync(void *ctx, int fd)
{
    int ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfs_file_t *file = (lfs_file_t *)&(lfs_manager->files[fd]);

    lfs_lock(lfs_manager->lock);
    ret = lfs_file_sync(lfs_manager->lfs, file);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_fstat(void *ctx, int fd, struct stat *st)
{
    struct lfs_info s;
    int ret;
    lfs_manager_t *lfs_manager;
    lfs_file_t *file;

    lfs_manager = (lfs_manager_t *)ctx;
    lfs_lock(lfs_manager->lock);
    file = &(lfs_manager->files[fd]);
    ret = lfs_fstat(lfs_manager->lfs, file, &s);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret == 0) {
        st->st_size = (s.type == LFS_TYPE_DIR) ? 0 : s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR ? S_IFDIR : S_IFREG));
        st->st_mtime = 0;
        st->st_atime = 0;
        st->st_ctime = 0;

    }
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static inline mode_t get_stat_mode(bool is_dir)
{
    return S_IRWXU | S_IRWXG | S_IRWXO |
            ((is_dir) ? S_IFDIR : S_IFREG);
}

static int32_t lfs_vfs_stat(void *ctx, const char *path, struct stat *st)
{
    struct lfs_info s;
    int ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    if (strcmp(path, "/") == 0) {
        /* FatFS f_stat function does not work for the drive root.
         * Just pretend that this is a directory.
         */
        memset(st, 0, sizeof(*st));
        st->st_mode = get_stat_mode(true);
        return 0;
    }
    memset(st, 0, sizeof(*st));
    lfs_lock(lfs_manager->lock);
    ret = lfs_stat(lfs_manager->lfs, path, &s);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret == 0) {
        st->st_size = (s.type == LFS_TYPE_DIR) ? 0 : s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR ? S_IFDIR : S_IFREG));
        st->st_mtime = 0;
        st->st_atime = 0;
        st->st_ctime = 0;
    }
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_access(void *ctx, const char *path, int amode)
{
    struct stat s;
    int ret;

    ret = lfs_vfs_stat(ctx, path, &s);
    ret = lfs_ret_value_convert(ret);
    if (ret == 0) {
        switch (amode) {
        case F_OK:
            return 0;
        case R_OK:
            return s.st_mode & S_IRUSR ? 0 : 1;
        case W_OK:
            return s.st_mode & S_IWUSR ? 0 : 1;
        case X_OK:
            return s.st_mode & S_IXUSR ? 0 : 1;
        }
    }
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_statfs(void *ctx, const char *path, struct statfs *sfs)
{
    int32_t block_used;
    int ret;
    lfs_manager_t *lfs_manager;

    lfs_manager = (lfs_manager_t *)ctx;
    lfs_lock(lfs_manager->lock);

    ret = lfs_fs_size(lfs_manager->lfs);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret >= 0) {
        block_used = ret;
        memset(sfs, 0, sizeof(struct statfs));
        sfs->f_type = 0xd3fc;
        sfs->f_bsize = lfs_manager->config->block_size;
        sfs->f_blocks = lfs_manager->config->block_count;
        sfs->f_bfree = lfs_manager->config->block_count - block_used;
        sfs->f_bavail = sfs->f_bfree;
        sfs->f_files = 1024;
        ret = 0;
    } else {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }

    return ret;
}

static int32_t lfs_vfs_unlink(void *ctx, const char *path)
{
    int ret;
    lfs_manager_t *lfs_manager;

    lfs_manager = (lfs_manager_t *)ctx;
    lfs_lock(lfs_manager->lock);
    ret = lfs_remove(lfs_manager->lfs, path);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_rename(void *ctx, const char *src, const char *dst)
{
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    const char *oldname = src;
    const char *newname = dst;

    lfs_lock(lfs_manager->lock);
    ret = lfs_rename(lfs_manager->lfs, oldname, newname);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static DIR *lfs_vfs_opendir(void* ctx, const char* name)
{
    lfsvfs_dir_t *lfsvfs_dir = NULL;
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    lfsvfs_dir = (lfsvfs_dir_t *)lfs_malloc(sizeof(lfsvfs_dir_t) + LFS_NAME_MAX + 1);
    if (!lfsvfs_dir) {
        return NULL;
    }

    memset(lfsvfs_dir, 0, sizeof(lfsvfs_dir_t) + LFS_NAME_MAX + 1);

    lfs_lock(lfs_manager->lock);
    ret = lfs_dir_open(lfs_manager->lfs, &lfsvfs_dir->lfsdir, name);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        lfs_free(lfsvfs_dir);
        errno = -ret;
        return NULL;
    }

    return (DIR *)lfsvfs_dir;
}

static struct dirent *lfs_vfs_readdir(void *ctx, DIR *pdir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)pdir;
    struct lfs_info info;
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    if (!lfsvfs_dir) {
        return NULL;
    }

    lfs_lock(lfs_manager->lock);
    ret = lfs_dir_read(lfs_manager->lfs, &lfsvfs_dir->lfsdir, &info);
    lfs_unlock(lfs_manager->lock);

    if (ret != true) {
        ret = lfs_ret_value_convert(ret);
        errno = -ret;
        return NULL;
    }

    if (info.name[0] == 0) {
        return NULL;
    }

    lfsvfs_dir->cur_dirent.d_ino = 0;
    lfsvfs_dir->cur_dirent.d_type = info.type;
    strncpy(lfsvfs_dir->cur_dirent.d_name, info.name, LFS_NAME_MAX);
    lfsvfs_dir->cur_dirent.d_name[LFS_NAME_MAX] = '\0';

    return &lfsvfs_dir->cur_dirent;
}

static int32_t lfs_vfs_closedir(void *ctx, DIR *pdir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)pdir;
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    if (!lfsvfs_dir) {
        return -EINVAL;
    }
    lfs_lock(lfs_manager->lock);
    ret = lfs_dir_close(lfs_manager->lfs, &lfsvfs_dir->lfsdir);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    lfs_free(lfsvfs_dir);

    return ret;
}

static int32_t lfs_vfs_mkdir(void *ctx, const char *name, mode_t mode)
{
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;

    lfs_lock(lfs_manager->lock);
    ret = lfs_mkdir(lfs_manager->lfs, name);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s %s return %d", __func__, name, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static int32_t lfs_vfs_rmdir(void *ctx, const char *name)
{
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;

    lfs_lock(lfs_manager->lock);
    ret = lfs_remove(lfs_manager->lfs, name);
    lfs_unlock(lfs_manager->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static long lfs_vfs_telldir(void *ctx, DIR *pdir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)pdir;
    int32_t ret;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    if (!lfsvfs_dir) {
        return -EINVAL;
    }

    lfs_lock(lfs_manager->lock);
    ret = lfs_dir_tell(lfs_manager->lfs, &lfsvfs_dir->lfsdir);
    lfs_unlock(lfs_manager->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < LFS_ERR_OK) {
        LFS_DEBUG("%s return %d", __func__, ret);
        errno = -ret;
        return -1;
    }
    return ret;
}

static void lfs_vfs_seekdir(void *ctx, DIR *pdir, long offset)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)pdir;
    lfs_manager_t *lfs_manager = (lfs_manager_t *)ctx;
    if (!lfsvfs_dir) {
        return;
    }

    lfs_lock(lfs_manager->lock);
    lfs_dir_seek(lfs_manager->lfs, &lfsvfs_dir->lfsdir, (lfs_off_t)offset);
    lfs_unlock(lfs_manager->lock);
}

static int32_t lfs_vfs_utime(void *ctx, const char *path, const struct utimbuf *times)
{
    return lfs_ret_value_convert(LFS_ERR_OK);
}


static int lfs_format_and_mount(int i)
{
    int ret = 0;

    LFS_DEBUG("Formatting %s ...", g_lfs_manager[i]->mountpath);
    ret = lfs_format(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("Failed to format %s", g_lfs_manager[i]->mountpath);
        return ret;
    }

    ret = lfs_mount(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("Failed to mount %s!", g_lfs_manager[i]->mountpath);
        return ret;
    }

    return ret;
}

int lfs_vfs_mount(void)
{
    int ret, i = 0;

    ret = _lfs_init();
    if (ret != 0) {
        return ret;
    }

    while (i < CONFIG_LITTLEFS_CNT) {
        LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
        LFS_DEBUG("Mounting %s ...", g_lfs_manager[i]->mountpath);
        lfs_lock(g_lfs_manager[i]->lock);

        ret = lfs_mount(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
        ret = lfs_ret_value_convert(ret);
        if (ret < 0) {
            LFS_ERROR("Failed to mount %s!", g_lfs_manager[i]->mountpath);
        #ifdef LITTLEFS_FORMAT
            ret = lfs_format_and_mount(i);
            if (ret < 0) {
                goto ERROR;
            }
            if (g_partition_formatted[i]) {
                g_partition_formatted[i] = false;
            }
        #endif
        }

        /* if in recovery mode, do format and mount again. */
        if (g_partition_formatted[i]) {
            LFS_DEBUG("%s in recovery mode, will format filesystem.", __func__);
            ret = lfs_format_and_mount(i);
            if (ret < 0) {
                goto ERROR;
            }
            g_partition_formatted[i] = false;
        }

        if (ret < 0) {
            LFS_ERROR("Failed to mount %s", g_lfs_manager[i]->mountpath);
            goto ERROR;
        }

        g_lfs_manager[i]->mounted = true;
        lfs_unlock(g_lfs_manager[i]->lock);
        i++;
    }

    return ret;

ERROR:
    lfs_unlock(g_lfs_manager[i]->lock);
    _lfs_deinit();

    return ret;
}

int lfs_vfs_unmount(void)
{
    int ret, i = 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
        LFS_DEBUG("Unmounting %s ...", g_lfs_manager[i]->mountpath);
        ret = lfs_unmount(g_lfs_manager[i]->lfs);
        if (ret != 0) {
            goto end;
        }
        i++;
    }

    _lfs_deinit();

end:
    return lfs_ret_value_convert(ret);
}

static bool g_lfs_registered = false;

const esp_vfs_t littlefs_ops = {
    .flags = ESP_VFS_FLAG_CONTEXT_PTR,
    .open_p       = &lfs_vfs_open,
    .close_p      = &lfs_vfs_close,
    .read_p       = &lfs_vfs_read,
    .write_p      = &lfs_vfs_write,
    .lseek_p      = &lfs_vfs_lseek,
    .fsync_p      = &lfs_vfs_fsync,
    .fstat_p      = &lfs_vfs_fstat,
    .stat_p       = &lfs_vfs_stat,
    .unlink_p     = &lfs_vfs_unlink,
    .rename_p     = &lfs_vfs_rename,
    .access_p     = &lfs_vfs_access,
    .opendir_p    = &lfs_vfs_opendir,
    .readdir_p    = &lfs_vfs_readdir,
    .closedir_p   = &lfs_vfs_closedir,
    .mkdir_p      = &lfs_vfs_mkdir,
    .rmdir_p      = &lfs_vfs_rmdir,
    .telldir_p    = &lfs_vfs_telldir,
    .seekdir_p    = &lfs_vfs_seekdir,
    .utime_p      = &lfs_vfs_utime,
    .statfs_p     = &lfs_vfs_statfs,
};

int32_t littlefs_register(void)
{
    if (g_lfs_registered) {
        return 0;
    }

    int ret = lfs_vfs_mount(), i = 0;
    if (ret == 0) {
        while (i < CONFIG_LITTLEFS_CNT) {
            LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
            LFS_DEBUG("Registering %s ...\r\n", g_lfs_manager[i]->mountpath);
            ret = esp_vfs_register(g_lfs_manager[i]->mountpath, &littlefs_ops, g_lfs_manager[i]);
            if (ret != 0) {
                return ret;
            }
            i++;
        }
    }

    if (ret == 0) {
        g_lfs_registered = true;
    }
    return ret;
}

int32_t littlefs_unregister(void)
{
    int ret, i = 0;

    if (!g_lfs_registered) {
        return 0;
    }

    while (i < CONFIG_LITTLEFS_CNT) {
        LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
        LFS_DEBUG("Unregistering %s ...", g_lfs_manager[i]->mountpath);
        ret = esp_vfs_unregister(g_lfs_manager[i]->mountpath);
        if (ret != 0) {
            return ret;
        }
        i++;
    }

    ret = lfs_vfs_unmount();
    if (ret < 0) {
        return ret;
    }

    if (ret == 0) {
        g_lfs_registered = false;
    }
    return ret;
}

