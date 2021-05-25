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
#include <k_api.h>
#include <sys/types.h>
#include <fcntl.h>

#include "lfs.h"
#include "lfs_util.h"

#include "aos/errno.h"
#include "aos/kernel.h"

#include "aos/hal/flash.h"

#ifdef AOS_COMP_NFTL
#include <inc/nftl.h>
#endif

#ifdef FS_TIMESTAMP_WORKAROUND
#include <time.h>
#endif

#include "vfs_types.h"
#include "vfs_api.h"
#include "littlefs.h"

/* lookahead size should be 8 bytes aligned */
#define _ALIGN8(n) (((n)+7) & (~7))
#define GET_LOOKAHEAD_SIZE(blks) (_ALIGN8((blks)>>4) > 16 ? _ALIGN8((blks)>>4) : 16)

#define LFS_INVALID_SIZE ((lfs_size_t)(-1))
#define WAIT_FOREVER 0xFFFFFFFF

#if CONFIG_U_FLASH_CORE
#define LITTLEFS_USING_MTD 1
#include <aos/mtd.h>
#else
#include <vfsdev/flash_dev.h>
#define LITTLEFS_USING_MTD 0
#endif

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
} lfs_manager_t;

typedef struct _lfsvfs_dir_t
{
    vfs_dir_t    dir;
    lfs_dir_t    lfsdir;
    vfs_dirent_t cur_dirent;
} lfsvfs_dir_t;

static bool g_partition_formatted[CONFIG_LITTLEFS_CNT] = {false};
static lfs_manager_t *g_lfs_manager[CONFIG_LITTLEFS_CNT] = {NULL};

static int vfs_to_lfs_idx(vfs_file_t *vfs)
{
    int i = 0;
    char *name;

    if (!vfs) return -1;

    LFS_ASSERT(vfs->node != NULL);
    name = vfs->node->i_name;
    LFS_ASSERT(name != NULL);

    while (i < CONFIG_LITTLEFS_CNT) {
        if (!memcmp(g_lfs_manager[i]->mountpath, name, strlen(name))) {
            /* return error for unounted fs */
            if (g_lfs_manager[i]->mounted == false) {
                LFS_ERROR("Something was wrong, parition %s is unmounted now.", name);
                return -1;
            }
            return i;
        } else {
            i++;
        }
    }

    return -1;
}

#ifdef AOS_COMP_NFTL
static int get_partition_from_cfg(const struct lfs_config *cfg)
{
    int i = 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i]->config == cfg) {
            return g_lfs_manager[i]->part;
        } else {
            i++;
        }
    }

    return HAL_PARTITION_ERROR;
}
#endif

#if !LITTLEFS_USING_MTD
static int get_fd_from_cfg(const struct lfs_config *cfg)
{
    int i = 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i]->config == cfg) {
            return g_lfs_manager[i]->fd;
        } else {
            i++;
        }
    }

    return -1;
}

static hal_mtdpart_info_t * littlefs_get_mtd_info(hal_partition_t part, int *idx)
{
    int cnt, ret;
    hal_mtdpart_info_t *arr;

    ret = hal_flash_mtdpart_info_get(&arr, &cnt);
    if (ret) {
        LFS_ERROR("Failed to get mtd partition info!\r\n");
        return NULL;
    }

    int i = 0;
    while (i < cnt) {
        if (arr[i].p == part) break;
        else i++;
    }

    if (i >= cnt) {
        LFS_ERROR("No parition found for index %d!", part);
        return NULL;
    }

    *idx = i;

    return arr;
}
#endif // !LITTLEFS_USING_MTD

static lfs_size_t littlefs_mtd_get_block_cnt(const struct lfs_config *c)
{
    lfs_size_t ret = LFS_INVALID_SIZE;

#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    if (mtd) ret = (mtd->size / mtd->block_size);
#else
    int idx;
    hal_partition_t part = *(int *)c->context;
    hal_mtdpart_info_t *arr = littlefs_get_mtd_info(part, &idx);

    if (arr) {
        ret = arr[idx].siz / arr[idx].bsiz;
        free(arr);
    }
#endif // !LITTLEFS_USING_MTD

    return ret;
}

static lfs_size_t littlefs_mtd_get_read_size(const struct lfs_config *c)
{
    lfs_size_t ret = LFS_INVALID_SIZE;

#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    if (mtd) ret = mtd->sector_size;
#else
    int idx;
    hal_partition_t part = *(int *)c->context;
    hal_mtdpart_info_t *arr = littlefs_get_mtd_info(part, &idx);

    if (arr) {
        ret = arr[idx].ssiz;
        free(arr);
    }
#endif // !LITTLEFS_USING_MTD

    return ret;
}

static lfs_size_t littlefs_mtd_get_prog_size(const struct lfs_config *c)
{
    lfs_size_t ret = LFS_INVALID_SIZE;

#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    if (mtd) ret = mtd->sector_size;
#else
    int idx;
    hal_partition_t part = *(int *)c->context;
    hal_mtdpart_info_t *arr = littlefs_get_mtd_info(part, &idx);

    if (arr) {
        ret = arr[idx].ssiz;
        free(arr);
    }
#endif // !LITTLEFS_USING_MTD

    return ret;
}

static lfs_size_t littlefs_mtd_get_block_size(const struct lfs_config *c)
{
    lfs_size_t ret = (lfs_size_t)(-1);

#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    if (mtd) ret = mtd->block_size;
#else
    int idx;
    hal_partition_t part = *(int *)c->context;
    hal_mtdpart_info_t *arr = littlefs_get_mtd_info(part, &idx);

    if (arr) {
        ret = arr[idx].bsiz;
        free(arr);
    }
#endif // !LITTLEFS_USING_MTD

    return ret;
}

static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, void *dst, lfs_size_t size)
{
    int ret;

#ifdef AOS_COMP_NFTL
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    int target_partition;
    switch (idx) {
        /* DATA */
        case HAL_PARTITION_LITTLEFS: target_partition = NFTL_PARTITION2; break;
        /* system A */
        case HAL_PARTITION_LITTLEFS2: target_partition = NFTL_PARTITION0; break;
        /* system B */
        case HAL_PARTITION_LITTLEFS3: target_partition = NFTL_PARTITION1; break;
        default: LFS_ERROR("%s invalid parition number %d", __func__, idx); return LFS_ERR_IO;
    }
    ret = nftl_flash_read(target_partition, block, off, dst, size);
#else
#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    ret = aos_mtd_read(mtd, c->block_size * block + off, dst, size);
    ret = (ret == size) ? 0 : -1;
#else
    int fd = get_fd_from_cfg(c);
    if (fd < 0) return fd;

    uint32_t addr;
    addr = c->block_size * block + off;
    lseek(fd, (off_t)addr, SEEK_SET);
    ret = read(fd, dst, size);
    ret = (ret == size) ? 0 : -1;
#endif // LITTLEFS_USING_MTD
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);
    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                              lfs_off_t off, const void *dst, lfs_size_t size)
{
    int ret;

#ifdef AOS_COMP_NFTL
    int target_partition;
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    switch (idx) {
        /* DATA */
        case HAL_PARTITION_LITTLEFS: target_partition = NFTL_PARTITION2; break;
        /* system A */
        case HAL_PARTITION_LITTLEFS2: target_partition = NFTL_PARTITION0; break;
        /* system B */
        case HAL_PARTITION_LITTLEFS3: target_partition = NFTL_PARTITION1; break;
        default: LFS_ERROR("%s invalid parition number %d", __func__, idx); return LFS_ERR_IO;
    }
    ret = nftl_flash_write(target_partition, block, off, dst, size);
#else
#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    ret = aos_mtd_write(mtd, c->block_size * block + off, dst, size);
    ret = (ret == size) ? 0 : -1;
#else
    int fd = get_fd_from_cfg(c);
    if (fd < 0) return fd;

    uint32_t addr;
    addr = c->block_size * block + off;
    lseek(fd, (off_t)addr, SEEK_SET);
    ret = write(fd, dst, size);
    ret = (ret == size) ? 0 : -1;
#endif // LITTLEFS_USING_MTD
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);

    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret;

#ifdef AOS_COMP_NFTL
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    int target_partition;
    switch (idx) {
        /* DATA */
        case HAL_PARTITION_LITTLEFS: target_partition = NFTL_PARTITION2; break;
        /* system A */
        case HAL_PARTITION_LITTLEFS2: target_partition = NFTL_PARTITION0; break;
        /* system B */
        case HAL_PARTITION_LITTLEFS3: target_partition = NFTL_PARTITION1; break;
        default: LFS_ERROR("%s invalid parition number %d", __func__, idx); return LFS_ERR_IO;
    }
    ret = nftl_flash_erase(target_partition, block);
#else
#if LITTLEFS_USING_MTD
    aos_mtd_t *mtd = c->context;
    ret = aos_mtd_erase(mtd, c->block_size * block, c->block_size);
#else // LITTLEFS_USING_MTD
    int fd = get_fd_from_cfg(c);
    if (fd < 0) return fd;

    uint32_t addr;
    addr = c->block_size * block;
    lseek(fd, (off_t)addr, SEEK_SET);
    ret = ioctl(fd, IOC_FLASH_ERASE_FLASH, c->block_size);
#endif // LITTLEFS_USING_MTD
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);

    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

#ifdef NFTL_GC_NOTIFY
static int32_t littlefs_block_notify_gabage(const struct lfs_config *c, lfs_block_t block)
{
    int ret;

    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    LFS_TRACE("%d %d", idx, block);

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == HAL_PARTITION_LITTLEFS2 || idx == HAL_PARTITION_LITTLEFS3) {
        LFS_DEBUG("parition %d is not allowed to change!", idx);
        return LFS_ERR_IO;
    }
#endif

#ifdef AOS_COMP_NFTL
    int target_partition;
    switch (idx) {
        /* DATA */
        case HAL_PARTITION_LITTLEFS: target_partition = NFTL_PARTITION2; break;
        /* system A */
        case HAL_PARTITION_LITTLEFS2: target_partition = NFTL_PARTITION0; break;
        /* system B */
        case HAL_PARTITION_LITTLEFS3: target_partition = NFTL_PARTITION1; break;
        default: LFS_ERROR("%s invalid parition number %d", __func__, idx); return LFS_ERR_IO;
    }
    ret = nftl_flash_notify_gabage(target_partition, block);
#else
    ret = LFS_ERR_OK;
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);

    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}
#endif

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

/* Relative path convert */
static char *path_convert(const char *path, int *idx)
{
    int i;
    int32_t len, prefix;
    char *target_path, *p, *mountpath;

    if (path == NULL) {
        return NULL;
    }

    len = strlen(path);

    for (i = 0; i < CONFIG_LITTLEFS_CNT; i++) {
        LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
        mountpath = g_lfs_manager[i]->mountpath;

        prefix = strlen(mountpath);
        if (strncmp(mountpath, path, prefix) != 0) {
            continue;
        } else {
            *idx = i;
            break;
        }
    }

    if (i >= CONFIG_LITTLEFS_CNT) {
        *idx = -1;
        return NULL;
    } else {
        /* return error for unounted fs */
        if (g_lfs_manager[i]->mounted == false) {
            LFS_ERROR("Something was wrong, parition %s is unmounted now.", path);
            *idx = -1;
            return NULL;
        }
    }

    len = len - prefix;
    target_path =(char *)lfs_malloc(len + 1);
    if (target_path == NULL) {
        return NULL;
    }

    memset(target_path, 0, len + 1);
    if (len > 0) {
        p = (char *)(path + prefix + 1);
        memcpy(target_path, p, len - 1);
    }

    target_path[len] = '\0';
    return target_path;
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

    switch(lfs_ret) {
        case LFS_ERR_OK: ret = 0; break;
        case LFS_ERR_IO: ret = -EIO; break;
        case LFS_ERR_CORRUPT: ret = -EIO; break;
        case LFS_ERR_NOENT: ret = -ENOENT; break;
        case LFS_ERR_EXIST: ret = -EEXIST; break;
        case LFS_ERR_NOTDIR: ret = -ENOTDIR; break;
        case LFS_ERR_ISDIR: ret = -EISDIR; break;
        case LFS_ERR_NOTEMPTY: ret = -ENOTEMPTY; break;
        case LFS_ERR_BADF: ret = -EBADF; break;
        case LFS_ERR_FBIG: ret = -EFBIG; break;
        case LFS_ERR_INVAL: ret = -EINVAL; break;
        case LFS_ERR_NOSPC: ret = -ENOSPC; break;
        case LFS_ERR_NOMEM: ret = -ENOMEM; break;
        case LFS_ERR_NOATTR: ret = -ENODATA; break;
        case LFS_ERR_NAMETOOLONG: ret = -ENAMETOOLONG; break;
        default: ret = lfs_ret; break;
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
        if (g_lfs_manager[i]) lfs_free(g_lfs_manager[i]);
    #endif

        close(g_lfs_manager[i]->fd);

        if (g_lfs_manager[i]->config->read_buffer) lfs_free(g_lfs_manager[i]->config->read_buffer);
        if (g_lfs_manager[i]->config->prog_buffer) lfs_free(g_lfs_manager[i]->config->prog_buffer);
        if (g_lfs_manager[i]->config->lookahead_buffer) lfs_free(g_lfs_manager[i]->config->lookahead_buffer);

        g_lfs_manager[i] = NULL;

        i++;
    }

    return 0;
}

#ifdef LFS_STATIC_OBJECT
static lfs_manager_t native_lfs_manager[CONFIG_LITTLEFS_CNT] = {
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS, -1, NULL, false},
#if defined(CONFIG_MULTI_FS)
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS2, -1, NULL, false},
#endif /* CONFIG_MULTI_FS */
};

static lfs_t native_lfs[CONFIG_LITTLEFS_CNT];
#endif

static lfs_lock_t native_lfs_lock[CONFIG_LITTLEFS_CNT];

static int32_t _lfs_init(void)
{
    int i = 0;
#if LITTLEFS_USING_MTD
    const char *mtd_part[2] = {MTD_PART_NAME_LITTLEFS, MTD_PART_NAME_LITTLEFS2};
#else
    int fd;
    char dev_str[16];
#endif

    /* Create LFS struct */
    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i] != NULL) {
            return 0;
        }

    #ifdef LFS_STATIC_OBJECT
        g_lfs_manager[i] = &native_lfs_manager[i];
    #else
        g_lfs_manager[i] = (lfs_manager_t *)lfs_malloc(sizeof(lfs_manager_t));
    #endif
        if (g_lfs_manager[i] == NULL) {
            goto ERROR;
        }

        memset(g_lfs_manager[i], 0, sizeof(lfs_manager_t));

        /* Set LFS partition index and mount point. */
        g_lfs_manager[i]->part = _lfs_parttion[i];

        if (i == 0) {
            g_lfs_manager[i]->part = HAL_PARTITION_LITTLEFS;
            g_lfs_manager[i]->mountpath = CONFIG_LFS_MOUNTPOINT;
        }
#if defined(CONFIG_MULTI_FS)
        else {
            g_lfs_manager[i]->part = HAL_PARTITION_LITTLEFS2;
            g_lfs_manager[i]->mountpath = CONFIG_LFS_MOUNTPOINT2;
        }
#endif /* CONFIG_MULTI_FS */

        // open flash device
    #if !LITTLEFS_USING_MTD
        memset(dev_str, 0, sizeof(dev_str));
        snprintf(dev_str, sizeof(dev_str) - 1,
                 "/dev/flash%d", g_lfs_manager[i]->part);
        fd = open(dev_str, 0);
        if (fd < 0) {
            // TODO: close open fd?
            LFS_ERROR("Faild to open device %s", dev_str);
            goto ERROR;
        }

        g_lfs_manager[i]->fd = fd;
    #endif // !LITTLEFS_USING_MTD

        /* Set LFS default config */
        g_lfs_manager[i]->config = &default_cfg[i];

    #if LITTLEFS_USING_MTD
        g_lfs_manager[i]->config->context = aos_mtd_open(mtd_part[i]);
    #else
        g_lfs_manager[i]->config->context = &(g_lfs_manager[i]->part);
    #endif

        // block device configuration
        default_cfg[i].read_size = littlefs_mtd_get_read_size(g_lfs_manager[i]->config);
        default_cfg[i].prog_size = littlefs_mtd_get_prog_size(g_lfs_manager[i]->config);
        default_cfg[i].block_size = littlefs_mtd_get_block_size(g_lfs_manager[i]->config);
        LFS_ASSERT(default_cfg[i].read_size != LFS_INVALID_SIZE);
        LFS_ASSERT(default_cfg[i].prog_size != LFS_INVALID_SIZE);
        LFS_ASSERT(default_cfg[i].block_size != LFS_INVALID_SIZE);

        default_cfg[i].block_count = littlefs_mtd_get_block_cnt(g_lfs_manager[i]->config);
        default_cfg[i].lookahead_size = GET_LOOKAHEAD_SIZE(default_cfg[i].block_count);
        default_cfg[i].cache_size = default_cfg[i].prog_size;

    #ifdef AOS_COMP_NFTL
        default_cfg[i].block_cycles = -1;
    #else
        default_cfg[i].block_cycles = 500;
    #endif

#ifdef CONFIG_LFS_SYSTEM_READONLY
        if (i == 0)
#endif
        {
            /* Create LFS Global Lock */
            g_lfs_manager[i]->lock = &native_lfs_lock[i];
            if (g_lfs_manager[i]->lock == NULL) {
                goto ERROR;
            }
            lfs_lock_create(g_lfs_manager[i]->lock);
        }
#ifdef CONFIG_LFS_SYSTEM_READONLY
        else {
            g_lfs_manager[i]->lock = NULL;
        }
#endif

        /* Create LFS struct */

    #ifdef LFS_STATIC_OBJECT
        g_lfs_manager[i]->lfs = &native_lfs[i];
    #else
        g_lfs_manager[i]->lfs = (lfs_t *)lfs_malloc(sizeof(lfs_t));
    #endif
        if (g_lfs_manager[i]->lfs == NULL) {
            goto ERROR;
        }

        g_lfs_manager[i]->config->read_buffer = lfs_malloc(default_cfg[i].read_size);
        g_lfs_manager[i]->config->prog_buffer = lfs_malloc(default_cfg[i].prog_size);
        g_lfs_manager[i]->config->lookahead_buffer = lfs_malloc(default_cfg[i].lookahead_size);
        LFS_ASSERT(default_cfg[i].read_buffer);
        LFS_ASSERT(default_cfg[i].prog_buffer);
        LFS_ASSERT(default_cfg[i].lookahead_buffer);

        i++;
    }

    return 0;

ERROR:
    _lfs_deinit();
    return -1;
}

static int32_t lfs_vfs_open(vfs_file_t *fp, const char *path, int flags)
{
    int ret;
    char *target_path = NULL;
    lfs_file_t *file = NULL;

    int idx;
    target_path = path_convert(path, &idx);
    if (target_path == NULL || idx < 0) {
        return -EINVAL;
    }

    if (!strlen(target_path)) {
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if ((idx == 1) && ((flags & O_ACCMODE) != O_RDONLY)) {
        LFS_ERROR("Failed to open %s due to unacceptable flags %d, "
                 "this partition is readonly!", path, flags);
        return -EINVAL;
    }
#endif

    file = (lfs_file_t *)lfs_malloc(sizeof(lfs_file_t));
    if (file == NULL) {
        lfs_free(target_path);
        return -EINVAL;
    }

    lfs_lock(g_lfs_manager[idx]->lock);
#ifdef CONFIG_FS_OPEN_TIME_MEASURE
    long long t1, t2;
    t1 = krhino_sys_time_get();
#endif
    ret = lfs_file_open(g_lfs_manager[idx]->lfs, file,  target_path, mode_convert(flags));
#ifdef CONFIG_FS_OPEN_TIME_MEASURE
    t2 = krhino_sys_time_get();
    printf("%s gap: %lld\r\n", __func__, t2 - t1);
#endif
    lfs_unlock(g_lfs_manager[idx]->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("%s %s failed, ret - %d", __func__, path, ret);
        lfs_free(target_path);
        lfs_free(file);
        return ret;
    } else {
        fp->f_arg = (void *)file;
    }

    lfs_free(target_path);

    return ret;
}

static int32_t lfs_vfs_close(vfs_file_t *fp)
{
    int ret;
    lfs_file_t *file = (lfs_file_t *)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_close(g_lfs_manager[idx]->lfs, file);
    lfs_unlock(g_lfs_manager[idx]->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("%s failed, ret - %d", __func__, ret);
        return ret;
    }

    lfs_free(file);

    return ret;
}

static int32_t lfs_vfs_read(vfs_file_t *fp, char *buf, uint32_t len)
{
    int ret;

    lfs_file_t *file = (lfs_file_t *)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_read(g_lfs_manager[idx]->lfs, file, buf, len);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s return %d", __func__, ret);
    return lfs_ret_value_convert(ret);
}

static int32_t lfs_vfs_write(vfs_file_t *fp, const char *buf, uint32_t len)
{
    int ret;

    lfs_file_t *file = (lfs_file_t *)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to write(len: %d) since "
                 "this partition is readonly!", len);
        return -EINVAL;
    }
#endif

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_write(g_lfs_manager[idx]->lfs, file, buf, len);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s return %d", __func__, ret);
    return lfs_ret_value_convert(ret);
}

static uint32_t lfs_vfs_lseek(vfs_file_t *fp, int64_t off, int32_t whence)
{
    int32_t ret;

    lfs_file_t *file = (lfs_file_t *)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_seek(g_lfs_manager[idx]->lfs, file, off, whence);
    lfs_unlock(g_lfs_manager[idx]->lock);

    return lfs_ret_value_convert(ret);
}

static int32_t lfs_vfs_sync(vfs_file_t *fp)
{
    int ret;

    lfs_file_t *file = (lfs_file_t *)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_sync(g_lfs_manager[idx]->lfs, file);
    lfs_unlock(g_lfs_manager[idx]->lock);

    return lfs_ret_value_convert(ret);
}

static int32_t lfs_vfs_fstat(vfs_file_t *fp, vfs_stat_t *st)
{
    struct lfs_info s;
    int32_t ret;

    lfs_file_t *file = (lfs_file_t*)(fp->f_arg);

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_fstat(g_lfs_manager[idx]->lfs, file, &s);
    lfs_unlock(g_lfs_manager[idx]->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret == 0){
        st->st_size = s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR) ? S_IFDIR : S_IFREG);
#ifdef FS_TIMESTAMP_WORKAROUND
        st->st_actime = time(NULL);
        st->st_modtime = time(NULL);
#endif
    }

    return ret;
}

static int32_t lfs_vfs_stat(vfs_file_t *fp, const char *path, vfs_stat_t *st)
{
    struct lfs_info s;
    int ret;
    char *target_path = NULL;

    int idx;
    target_path = path_convert(path, &idx);
    if (target_path == NULL) {
        return -EINVAL;
    }

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_stat(g_lfs_manager[idx]->lfs, target_path, &s);
    lfs_unlock(g_lfs_manager[idx]->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret == 0) {
        st->st_size = (s.type == LFS_TYPE_DIR) ? 0: s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR ? S_IFDIR : S_IFREG));
#ifdef FS_TIMESTAMP_WORKAROUND
        time_t t = time(NULL);
        st->st_actime = t;
        st->st_modtime = t;
#endif
    }

    lfs_free(target_path);

    return ret;
}

static int32_t lfs_vfs_access(vfs_file_t *fp, const char *path, int mode)
{
    vfs_stat_t s;
    int ret;

    ret = lfs_vfs_stat(fp, path, &s);
    ret = lfs_ret_value_convert(ret);
    if (ret == 0) {
        switch(mode) {
            case F_OK: return 0;
            case R_OK: return s.st_mode & S_IRUSR ? 0 : 1;
            case W_OK: return s.st_mode & S_IWUSR ? 0 : 1;
            case X_OK: return s.st_mode & S_IXUSR ? 0 : 1;
        }
    }

    return ret;
}

static int32_t lfs_vfs_statfs(vfs_file_t *fp, const char *path, vfs_statfs_t *sfs)
{
    int32_t block_used;
    int ret;

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_fs_size(g_lfs_manager[idx]->lfs);
    lfs_unlock(g_lfs_manager[idx]->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret >= 0) {
        block_used = ret;
        memset(sfs, 0, sizeof(vfs_statfs_t));
        sfs->f_type = 0xd3fc;
        sfs->f_bsize = g_lfs_manager[idx]->config->block_size;
        sfs->f_blocks = g_lfs_manager[idx]->config->block_count;
        sfs->f_bfree = g_lfs_manager[idx]->config->block_count - block_used;
        sfs->f_bavail = sfs->f_bfree;
        sfs->f_files = 1024;
    }

    return ret;
}

static int32_t lfs_vfs_remove(vfs_file_t *fp, const char *path)
{
    int ret;
    char *target_path = NULL;

    int idx;
    target_path = path_convert(path, &idx);
    if (target_path == NULL) {
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to remove %s since "
                 "this partition is readonly!", path);
        return -EINVAL;
    }
#endif

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_remove(g_lfs_manager[idx]->lfs, target_path);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s return %d", __func__, path, ret);
    ret = lfs_ret_value_convert(ret);

    lfs_free(target_path);

    return ret;
}

static int32_t lfs_vfs_rename(vfs_file_t *fp, const char *oldpath, const char *newpath)
{
    int32_t ret;

    char *oldname = NULL;
    char *newname = NULL;

    int idx;
    oldname = path_convert(oldpath, &idx);
    if (!oldname) {
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to rename from %s since "
                 "this partition is readonly!", oldpath);
        return -EINVAL;
    }
#endif

    newname = path_convert(newpath, &idx);
    if (!newname) {
        lfs_free(oldname);
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to rename to %s since "
                 "this partition is readonly!", newpath);
        return -EINVAL;
    }
#endif

    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_rename(g_lfs_manager[idx]->lfs, oldname, newname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s->%s return %d", __func__, oldpath, newpath, ret);
    ret = lfs_ret_value_convert(ret);

    lfs_free(oldname);
    lfs_free(newname);

    return ret;
}

static vfs_dir_t *lfs_vfs_opendir(vfs_file_t *fp, const char *path)
{
    lfsvfs_dir_t *lfsvfs_dir = NULL;
    char *relpath = NULL;
    int32_t ret;

    int idx;
    relpath = path_convert(path, &idx);
    if (!relpath) {
        return NULL;
    }

    lfsvfs_dir = (lfsvfs_dir_t *)lfs_malloc(sizeof(lfsvfs_dir_t) + LFS_NAME_MAX + 1);
    if (!lfsvfs_dir) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = ENOMEM;
#endif
        lfs_free(relpath);
        return NULL;
    }

    memset(lfsvfs_dir, 0, sizeof(lfsvfs_dir_t) + LFS_NAME_MAX + 1);

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_dir_open(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir, relpath);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s return %d", __func__, ret);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = -ret;
#endif
        lfs_free(relpath);
        lfs_free(lfsvfs_dir);
        return NULL;
    } else {
        fp->f_arg = (void *)lfsvfs_dir;
    }

    lfs_free(relpath);

    return (vfs_dir_t *)lfsvfs_dir;
}

static vfs_dirent_t *lfs_vfs_readdir(vfs_file_t *fp, vfs_dir_t *dir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t*)dir;
    struct lfs_info info;
    int32_t ret;

    if (!lfsvfs_dir) {
        return NULL;
    }

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return NULL;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_dir_read(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir, &info);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret != true) {
        ret = lfs_ret_value_convert(ret);
#ifdef CONFIG_VFS_USE_ERRNO
        errno = -ret;
#endif
        return NULL;
    }

    if (info.name[0] == 0) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = EINVAL;
#endif
        return NULL;
    }

    lfsvfs_dir->cur_dirent.d_ino = 0;
    lfsvfs_dir->cur_dirent.d_type = info.type;
    strncpy(lfsvfs_dir->cur_dirent.d_name, info.name, LFS_NAME_MAX);
    lfsvfs_dir->cur_dirent.d_name[LFS_NAME_MAX] = '\0';

    return &lfsvfs_dir->cur_dirent;
}

static int32_t lfs_vfs_closedir(vfs_file_t *fp, vfs_dir_t *dir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)dir;
    int32_t ret;

    if (!lfsvfs_dir) {
        return -EINVAL;
    }

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_dir_close(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s return %d", __func__, ret);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        return ret;
    }

    lfs_free(lfsvfs_dir);

    return ret;
}

static int32_t lfs_vfs_mkdir(vfs_file_t *fp, const char *path)
{
    int32_t ret;
    char *pathname = NULL;

    int idx;
    pathname = path_convert(path, &idx);
    if (!pathname) {
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to rename to %s since "
                 "this partition is readonly!", path);
        return -EINVAL;
    }
#endif

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_mkdir(g_lfs_manager[idx]->lfs, pathname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if ((ret < LFS_ERR_OK) && (ret != LFS_ERR_EXIST)) {
        LFS_ERROR("%s %s return %d", __func__, path, ret);
    }

    ret = lfs_ret_value_convert(ret);

    lfs_free(pathname);

    return ret;
}

static int32_t lfs_vfs_rmdir (vfs_file_t *fp, const char *path)
{
    int32_t ret;
    char *pathname = NULL;

    int idx;
    pathname = path_convert(path, &idx);
    if (!pathname) {
        return -EINVAL;
    }

#ifdef CONFIG_LFS_SYSTEM_READONLY
    if (idx == 1) {
        LFS_ERROR("Failed to remove dir %s since "
                 "this partition is readonly!", path);
        return -EINVAL;
    }
#endif

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_remove(g_lfs_manager[idx]->lfs, pathname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s return %d", __func__, path, ret);
    ret = lfs_ret_value_convert(ret);

    lfs_free(pathname);

    return ret;
}

static void lfs_vfs_rewinddir(vfs_file_t *fp, vfs_dir_t *dir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)dir;

    if (!lfsvfs_dir) {
        return;
    }

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return;

    lfs_lock(g_lfs_manager[idx]->lock);
    lfs_dir_rewind(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir);
    lfs_unlock(g_lfs_manager[idx]->lock);
}

static int32_t lfs_vfs_telldir(vfs_file_t *fp, vfs_dir_t *dir)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t *)dir;
    int32_t ret;

    if (!lfsvfs_dir) {
        return -EINVAL;
    }

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_dir_tell(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir);
    lfs_unlock(g_lfs_manager[idx]->lock);

    return lfs_ret_value_convert(ret);
}

static void lfs_vfs_seekdir(vfs_file_t *fp, vfs_dir_t *dir, int32_t loc)
{
    lfsvfs_dir_t *lfsvfs_dir = (lfsvfs_dir_t*)dir;

    if (!lfsvfs_dir) {
        return;
    }

    int idx;
    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return;

    lfs_lock(g_lfs_manager[idx]->lock);
    lfs_dir_seek(g_lfs_manager[idx]->lfs, &lfsvfs_dir->lfsdir, (lfs_off_t)loc);
    lfs_unlock(g_lfs_manager[idx]->lock);
}

static int32_t lfs_vfs_utime(vfs_file_t *fp, const char *path, const vfs_utimbuf_t *times)
{
    return lfs_ret_value_convert(LFS_ERR_OK);
}

static vfs_filesystem_ops_t littlefs_ops = {
    .open       = &lfs_vfs_open,
    .close      = &lfs_vfs_close,
    .read       = &lfs_vfs_read,
    .write      = &lfs_vfs_write,
    .lseek      = &lfs_vfs_lseek,
    .sync       = &lfs_vfs_sync,
    .stat       = &lfs_vfs_stat,
    .unlink     = &lfs_vfs_remove,
    .remove     = &lfs_vfs_remove,
    .rename     = &lfs_vfs_rename,
    .access     = &lfs_vfs_access,
    .fstat      = &lfs_vfs_fstat,
    .statfs     = &lfs_vfs_statfs,
    .opendir    = &lfs_vfs_opendir,
    .readdir    = &lfs_vfs_readdir,
    .closedir   = &lfs_vfs_closedir,
    .mkdir      = &lfs_vfs_mkdir,
    .rmdir      = &lfs_vfs_rmdir,
    .rewinddir  = &lfs_vfs_rewinddir,
    .telldir    = &lfs_vfs_telldir,
    .seekdir    = &lfs_vfs_seekdir,
    .ioctl      = NULL,
    .utime      = &lfs_vfs_utime,
};

static int lfs_format_and_mount(int i)
{
    int ret = 0;

    LFS_DEBUG("Formatting %s ...", g_lfs_manager[i]->mountpath);
    ret = lfs_format(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("Failed to format %s", g_lfs_manager[i]->mountpath);
        return ret;;
    }

    ret = lfs_mount(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("Failed to mount %s!", g_lfs_manager[i]->mountpath);
        return ret;;
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
            if (ret < 0) goto ERROR;
            if (g_partition_formatted[i]) g_partition_formatted[i] = false;
        #endif
        }

        /* if in recovery mode, do format and mount again. */
        if (g_partition_formatted[i]) {
            LFS_DEBUG("%s in recovery mode, will format filesystem.", __func__);
            ret = lfs_format_and_mount(i);
            if (ret < 0) goto ERROR;
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
        if (ret != 0) goto end;
        i++;
    }

    _lfs_deinit();

end:
    return lfs_ret_value_convert(ret);
}

#ifdef CONFIG_LFS_CLI_TOOLS
#include <aos/cli.h>
#include "console.h"

static void print_help()
{
    printf("Filesystem CLI tools (currently on 'format' supported).\r\n\r\n");
    printf("Usage:\r\n    fs format </data|system>\r\n");
}

static void handle_fs_cmd(int argc, char **argv)
{
    const char *rtype = argc > 1 ? argv[1] : "";

    if (argc == 1 || (strcmp(rtype, "help") == 0)) {
        print_help();
    } else if (strcmp(rtype, "format") == 0) {
        if (argc < 3) {
            LFS_ERROR("%s Error: no mount point provided!", __func__);
            return;
        } else {
            int i, ret;
            char *path = argv[2], *target;
            target = path_convert(path, &i);
            if (!target || (i < 0)) {
                LFS_ERROR("Failed to find info for mount point %s", path);
                return;
            } else {
                LFS_TRACE("Formatting %s, please wait ...", target);
                ret = lfs_format(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
                ret = lfs_ret_value_convert(ret);
                if (ret < 0) {
                    LFS_ERROR("Failed to format: ret: %d.", ret);
                    lfs_free(target);
                    return;
                }

                LFS_TRACE("Mountting %s, please wait ...", target);
                ret = lfs_mount(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
                ret = lfs_ret_value_convert(ret);
                if (ret < 0) LFS_ERROR("Failed to mount: ret: %d.", ret);
                lfs_free(target);
            }
        }
    } else {
        LFS_WARN("Not supported command %s.", rtype);
    }
}

FINSH_FUNCTION_EXPORT_CMD(handle_fs_cmd, fs, fs [cmd] [opt]);
#endif

static bool g_lfs_registered = false;

int32_t littlefs_register(void)
{
    if (g_lfs_registered) return 0;

    int ret = lfs_vfs_mount(), i = 0;
    if (ret == 0) {
        while (i < CONFIG_LITTLEFS_CNT) {
            LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
            LFS_DEBUG("Registering %s ...\r\n", g_lfs_manager[i]->mountpath);
            ret = vfs_register_fs(g_lfs_manager[i]->mountpath,
                                  &littlefs_ops, NULL);
            if (ret != 0) return ret;
            i++;
        }
    }

    if (ret == 0) g_lfs_registered = true;

    return ret;
}

int32_t littlefs_unregister(void)
{
    int ret, i = 0;

    if (!g_lfs_registered) return 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
        LFS_DEBUG("Unregistering %s ...", g_lfs_manager[i]->mountpath);
        ret = vfs_unregister_fs(g_lfs_manager[i]->mountpath);
        if (ret != 0) return ret;
        i++;
    }

    ret = lfs_vfs_unmount();
    if (ret < 0) {
        return ret;
    }

    if (ret == 0) g_lfs_registered = false;

    return ret;
}

int littlefs_format(const char *partition)
{
    int i = 0, ret = 0;

    LFS_DEBUG("%s entry", __func__);

    while (i < CONFIG_LITTLEFS_CNT) {
        lfs_lock(g_lfs_manager[i]->lock);

        if (strcmp(partition, g_lfs_manager[i]->mountpath) == 0) {
            LFS_DEBUG("Unmounting %s ...", partition);
            ret = lfs_unmount(g_lfs_manager[i]->lfs);
            ret = lfs_ret_value_convert(ret);
            if (ret < 0) {
                lfs_unlock(g_lfs_manager[i]->lock);
                LFS_ERROR("Failed to format %s, ret: %d", partition, ret);
                break;
            }

            g_lfs_manager[i]->mounted = false;

            LFS_DEBUG("Formatting %s ...", partition);
            ret = lfs_format(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
            ret = lfs_ret_value_convert(ret);
            if (ret < 0) {
                lfs_unlock(g_lfs_manager[i]->lock);
                LFS_ERROR("Failed to format %s, ret: %d", partition, ret);
                break;
            }

            /* Do NOT mount filesystem anymore, wait for rebooting */
            lfs_unlock(g_lfs_manager[i]->lock);
            break;
        }

        lfs_unlock(g_lfs_manager[i]->lock);

        i++;
    }

    LFS_DEBUG("%s exit", __func__);
    return ret < 0 ? ret : 0;
}

/* This API is expected to be called before nftl and littlefs initialization. */
int littlefs_format2(const char *partition)
{
    int ret = 0;

    if (strcmp(partition, CONFIG_LFS_MOUNTPOINT) == 0) {
        g_partition_formatted[0] = true;
    } else {
        LFS_WARN("partition %s is not supported.", partition);
    }

    return ret;
}

int littlefs_clean_partition(const char *partition)
{
    int i, ret;

    for (i = 0; i < CONFIG_LITTLEFS_CNT; i++) {
        if (!g_lfs_manager[i]->mountpath) continue;
        if (strcmp(g_lfs_manager[i]->mountpath, partition) == 0) break;
    }

    if (i >= CONFIG_LITTLEFS_CNT) {
        LFS_ERROR("Invalid partition %s provided.", partition);
        return -1;
    }

    lfs_lock(g_lfs_manager[i]->lock);
    ret = lfs_fs_cleanup(g_lfs_manager[i]->lfs);
    lfs_unlock(g_lfs_manager[i]->lock);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("%s failed in parition %s", __func__, partition);
    }

    return ret;
}

int lfs_list()
{
    for (int i = 0; i < CONFIG_LITTLEFS_CNT; i++) {
        if (!g_lfs_manager[i] || !g_lfs_manager[i]->mounted) continue;
        else printf(" - littlefs on flash device (index %d) mounted at %s\r\n", i, g_lfs_manager[i]->mountpath);
    }

    return 0;
}

bool is_valid_lfs(const char *path)
{
    for (int i = 0; i < CONFIG_LITTLEFS_CNT; i++) {
        if (!g_lfs_manager[i] || !g_lfs_manager[i]->mounted) continue;
        if (strcmp(path, g_lfs_manager[i]->mountpath) == 0) return true;
    }

    return false;
}
