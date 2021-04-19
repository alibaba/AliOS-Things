/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <fcntl.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <k_api.h>

#include "lfs.h"
#include "lfs_util.h"

#include "aos/errno.h"
#include "aos/kernel.h"

#include "aos/hal/flash.h"

#ifdef AOS_COMP_NFTL
#include <inc/nftl.h>
#endif

#include "vfs_types.h"
#include "vfs_api.h"
#include "littlefs.h"
#include "lfs_conf.h"

#if defined(CONFIG_FS_USE_PARTITION) && CONFIG_FS_USE_PARTITION
#include <yoc/partition.h>
#endif

/* lookahead size should be 8 bytes aligned */
#define _ALIGN8(n) (((n)+7) & (~7))
#define LFS_LOOKAHEAD_SIZE (_ALIGN8(CONFIG_LFS_BLOCK_NUMS>>4) > 16 ? _ALIGN8(CONFIG_LFS_BLOCK_NUMS>>4) : 16)
#if (CONFIG_LITTLEFS_CNT > 1)
#define LFS2_LOOKAHEAD_SIZE (_ALIGN8(CONFIG_LFS2_BLOCK_NUMS>>4) > 16 ? _ALIGN8(CONFIG_LFS2_BLOCK_NUMS>>4) : 16)
#endif /* CONFIG_LITTLEFS_CNT > 1 */

typedef aos_mutex_t lfs_lock_t;

#ifdef CONFIG_FLASH_TRACE
#define FLASH_TRACE(fmt, ...) \
    printf("flash_trace %s:" fmt "\n", __func__, __VA_ARGS__)
#else
#define FLASH_TRACE(fmt, ...)
#endif

#ifdef CONFIG_VFS_TRACE
#define VFS_TRACE(fmt, ...) \
    printf("vfs_trace %s:" fmt "\n", __func__, __VA_ARGS__)
#else
#define VFS_TRACE(fmt, ...)
#endif

typedef struct {
    struct lfs_config *config;
    lfs_t             *lfs;
    lfs_lock_t        *lock;
    hal_partition_t   part;
    char              *mountpath;
    bool              mounted;
} lfs_manager_t;

typedef struct _lfsvfs_dir_t {
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

#if defined(CONFIG_FS_USE_PARTITION) && CONFIG_FS_USE_PARTITION
static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                                   lfs_off_t off, void *dst, lfs_size_t size)
{
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d %d %d", idx, block, off, size);

    uint32_t off_set = off + c->block_size * block;
    return partition_read(idx, off_set, dst, size);
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                                    lfs_off_t off, const void *dst, lfs_size_t size)
{
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d %d %d", idx, block, off, size);

    uint32_t off_set = off + c->block_size * block;
    return partition_write(idx, off_set, (void *)dst, size);
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d", idx, block);

    partition_info_t *part_info = hal_flash_get_info(idx);
    uint32_t off_set = c->block_size * block;

    return partition_erase(idx, off_set, c->block_size / part_info->sector_size);
}
#else
static int32_t littlefs_block_read(const struct lfs_config *c, lfs_block_t block,
                                   lfs_off_t off, void *dst, lfs_size_t size)
{
    int ret;

    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d %d %d", idx, block, off, size);

#ifdef AOS_COMP_NFTL
    int target_partition;
    switch (idx) {
    /* lfs1 */
    case HAL_PARTITION_LITTLEFS:
        target_partition = NFTL_PARTITION2;
        break;
    /* lfs2 */
    case HAL_PARTITION_LITTLEFS2:
        target_partition = NFTL_PARTITION0;
        break;
    default:
        LFS_ERROR("%s invalid parition number %d", __func__, idx);
        return LFS_ERR_IO;
    }
    ret = nftl_flash_read(target_partition, block, off, dst, size);
#else
    uint32_t addr;
    addr = c->block_size * block + off;
    ret = hal_flash_read(idx, &addr, dst, size);
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);
    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_write(const struct lfs_config *c, lfs_block_t block,
                                    lfs_off_t off, const void *dst, lfs_size_t size)
{
    int ret;

    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d %d %d", idx, block, off, size);

#ifdef AOS_COMP_NFTL
    int target_partition;
    switch (idx) {
    /* lfs1 */
    case HAL_PARTITION_LITTLEFS:
        target_partition = NFTL_PARTITION2;
        break;
    /* lfs2 */
    case HAL_PARTITION_LITTLEFS2:
        target_partition = NFTL_PARTITION0;
        break;
    default:
        LFS_ERROR("%s invalid parition number %d", __func__, idx);
        return LFS_ERR_IO;
    }
    ret = nftl_flash_write(target_partition, block, off, dst, size);
#else
    uint32_t addr;
    addr = c->block_size * block + off;
    ret = hal_flash_write(idx, &addr, dst, size);
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);

    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}

static int32_t littlefs_block_erase(const struct lfs_config *c, lfs_block_t block)
{
    int ret;

    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d", idx, block);

#ifdef AOS_COMP_NFTL
    int target_partition;
    switch (idx) {
    /* lfs1 */
    case HAL_PARTITION_LITTLEFS:
        target_partition = NFTL_PARTITION2;
        break;
    /* lfs2 */
    case HAL_PARTITION_LITTLEFS2:
        target_partition = NFTL_PARTITION0;
        break;
    default:
        LFS_ERROR("%s invalid parition number %d", __func__, idx);
        return LFS_ERR_IO;
    }
    ret = nftl_flash_erase(target_partition, block);
#else
    uint32_t addr;
    addr = c->block_size * block;
    ret = hal_flash_erase(idx, addr, c->block_size);
#endif

    if (ret) LFS_ERROR("%s ret: %d", __func__, ret);

    return ret == 0 ? ret : LFS_ERR_CORRUPT;
}
#endif

#ifdef NFTL_GC_NOTIFY
static int32_t littlefs_block_notify_gabage(const struct lfs_config *c, lfs_block_t block)
{
    int ret;

    int idx = get_partition_from_cfg(c);
    if (idx < 0) return idx;

    FLASH_TRACE("%d %d", idx, block);

#ifdef AOS_COMP_NFTL
    int target_partition;
    switch (idx) {
    /* lfs1 */
    case HAL_PARTITION_LITTLEFS:
        target_partition = NFTL_PARTITION2;
        break;
    /* lfs2 */
    case HAL_PARTITION_LITTLEFS2:
        target_partition = NFTL_PARTITION0;
        break;
    default:
        LFS_ERROR("%s invalid parition number %d", __func__, idx);
        return LFS_ERR_IO;
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
#if (CONFIG_LITTLEFS_CNT > 1)
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
        VFS_TRACE("%s: lock %p created", aos_task_name(), lock);
    }
}

static void lfs_lock(lfs_lock_t *lock)
{
    if (lock) {
        aos_mutex_lock(lock, AOS_WAIT_FOREVER);
        VFS_TRACE("%s: lock %p locked", aos_task_name(), lock);
    }
}

static void lfs_unlock(lfs_lock_t *lock)
{
    if (lock) {
        VFS_TRACE("%s: lock %p to be unlocked", aos_task_name(), lock);
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
    target_path =(char *)aos_zalloc(len + 1);
    if (target_path == NULL) {
        return NULL;
    }

    if (len > 0) {
        p = (char *)(path + prefix);
        memcpy(target_path, p, len);
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

static int32_t _lfs_deinit(int i)
{
    g_lfs_manager[i] = NULL;
    return 0;
}

static lfs_manager_t native_lfs_manager[CONFIG_LITTLEFS_CNT] = {
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS, NULL, false},
#if (CONFIG_LITTLEFS_CNT > 1)
    {NULL, NULL, NULL, HAL_PARTITION_LITTLEFS2, NULL, false},
#endif /* CONFIG_LITTLEFS_CNT > 1 */
};

static lfs_t native_lfs[CONFIG_LITTLEFS_CNT];

#if !(defined(CONFIG_FS_USE_PARTITION) && CONFIG_FS_USE_PARTITION)
static char native_lfs_read_buffer[CONFIG_LITTLEFS_CNT][CONFIG_LFS_PROG_SIZE];
static char native_lfs_prog_buffer[CONFIG_LITTLEFS_CNT][CONFIG_LFS_PROG_SIZE];
static uint8_t _lookahead_buf1[LFS_LOOKAHEAD_SIZE];
#if (CONFIG_LITTLEFS_CNT > 1)
static uint8_t _lookahead_buf2[LFS2_LOOKAHEAD_SIZE];
#endif
static uint8_t* native_lfs_lookahead_buffer[CONFIG_LITTLEFS_CNT] = {
    _lookahead_buf1,
#if (CONFIG_LITTLEFS_CNT > 1)
    _lookahead_buf2,
#endif
};
#endif

static lfs_lock_t native_lfs_lock[CONFIG_LITTLEFS_CNT];

static int32_t _lfs_init(const char *partition_name, int i)
{
    if (i >= (sizeof(g_lfs_manager) / sizeof(g_lfs_manager[0]))) {
        LFS_ERROR("%s failed, Max lfs partition limit hit!", __func__);
        return -1;
    }

    /* Create LFS struct */
    if (g_lfs_manager[i] != NULL) {
        return 0;
    }

    g_lfs_manager[i] = &native_lfs_manager[i];

    memset(g_lfs_manager[i], 0, sizeof(lfs_manager_t));

#if defined(CONFIG_FS_USE_PARTITION) && CONFIG_FS_USE_PARTITION
    partition_t lfs_hdl;
    lfs_hdl = partition_open(partition_name);
    if (lfs_hdl < 0) {
        LFS_ERROR("%s partitiion open ailed!", __func__);
        return -1;
    }

    /* Set LFS partition index and mount point. */
    g_lfs_manager[i]->part        = lfs_hdl;
    g_lfs_manager[i]->config      = &default_cfg[i];

    partition_info_t *part_info   = hal_flash_get_info(lfs_hdl);
    default_cfg[i].block_size     = part_info->sector_size;
    default_cfg[i].block_count    = part_info->length / part_info->sector_size;
    default_cfg[i].read_size      = 256;
    default_cfg[i].prog_size      = 256;
    default_cfg[i].cache_size     = 256;
    default_cfg[i].lookahead_size = 16;
    default_cfg[i].block_cycles   = 1000;
#else
    /* Set LFS partition index and mount point. */
    g_lfs_manager[i]->part = HAL_PARTITION_LITTLEFS;

    if (i == 0) {
        g_lfs_manager[i]->part = HAL_PARTITION_LITTLEFS;
    }
#if (CONFIG_LITTLEFS_CNT > 1)
    else {
        g_lfs_manager[i]->part = HAL_PARTITION_LITTLEFS2;
    }
#endif /* CONFIG_LITTLEFS_CNT */

    /* Set LFS default config */
    g_lfs_manager[i]->config  = &default_cfg[i];
    default_cfg[i].read_size  = CONFIG_LFS_PROG_SIZE;
    default_cfg[i].prog_size  = CONFIG_LFS_PROG_SIZE;
    default_cfg[i].block_size = CONFIG_LFS_PAGE_NUM_PER_BLOCK * default_cfg[i].prog_size;

    if (i == 0) {
        default_cfg[i].block_count    = CONFIG_LFS_BLOCK_NUMS;
        default_cfg[i].lookahead_size = LFS_LOOKAHEAD_SIZE;
    }
#if (CONFIG_LITTLEFS_CNT > 1)
    else {
        default_cfg[i].block_count    = CONFIG_LFS2_BLOCK_NUMS;
        default_cfg[i].lookahead_size = LFS2_LOOKAHEAD_SIZE;
    }
#endif

    default_cfg[i].cache_size = default_cfg[i].prog_size;

#ifdef AOS_COMP_NFTL
    default_cfg[i].block_cycles = -1;
#else
    default_cfg[i].block_cycles = 1000;
#endif

    g_lfs_manager[i]->config->read_buffer = native_lfs_read_buffer[i];
    g_lfs_manager[i]->config->prog_buffer = native_lfs_prog_buffer[i];
    g_lfs_manager[i]->config->lookahead_buffer = native_lfs_lookahead_buffer[i];
#endif /* CONFIG_FS_USE_PARTITION */

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

    return 0;

ERROR:
    _lfs_deinit(i);
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

    file = (lfs_file_t *)aos_zalloc(sizeof(lfs_file_t));
    if (file == NULL) {
        aos_free(target_path);
        return -EINVAL;
    }

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_file_open(g_lfs_manager[idx]->lfs, file,  target_path, mode_convert(flags));
    lfs_unlock(g_lfs_manager[idx]->lock);

    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_ERROR("%s %s failed, ret - %d", __func__, path, ret);
        aos_free(target_path);
        aos_free(file);
        return ret;
    } else {
        fp->f_arg = (void *)file;
    }

    aos_free(target_path);

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

    aos_free(file);

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
    if (ret == 0) {
        st->st_size = s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR) ? S_IFDIR : S_IFREG);
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
        st->st_size = s.size;
        st->st_mode = S_IRWXU | S_IRWXG | S_IRWXO |
                      ((s.type == LFS_TYPE_DIR ? S_IFDIR : S_IFREG));
    }

    aos_free(target_path);

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

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_remove(g_lfs_manager[idx]->lfs, target_path);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s return %d", __func__, path, ret);
    ret = lfs_ret_value_convert(ret);

    aos_free(target_path);

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

    newname = path_convert(newpath, &idx);
    if (!newname) {
        aos_free(oldname);
        return -EINVAL;
    }

    idx = vfs_to_lfs_idx(fp);
    if (idx < 0 || idx >= CONFIG_LITTLEFS_CNT) return -1;

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_rename(g_lfs_manager[idx]->lfs, oldname, newname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s->%s return %d", __func__, oldpath, newpath, ret);
    ret = lfs_ret_value_convert(ret);

    aos_free(oldname);
    aos_free(newname);

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

    lfsvfs_dir = (lfsvfs_dir_t *)aos_malloc(sizeof(lfsvfs_dir_t) + LFS_NAME_MAX + 1);
    if (!lfsvfs_dir) {
#ifdef CONFIG_VFS_USE_ERRNO
        errno = ENOMEM;
#endif
        aos_free(relpath);
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
        aos_free(relpath);
        aos_free(lfsvfs_dir);
        return NULL;
    } else {
        fp->f_arg = (void *)lfsvfs_dir;
    }

    aos_free(relpath);

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

    aos_free(lfsvfs_dir);

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

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_mkdir(g_lfs_manager[idx]->lfs, pathname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if ((ret < LFS_ERR_OK) && (ret != LFS_ERR_EXIST)) {
        LFS_ERROR("%s %s return %d", __func__, path, ret);
    }

    ret = lfs_ret_value_convert(ret);

    aos_free(pathname);

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

    lfs_lock(g_lfs_manager[idx]->lock);
    ret = lfs_remove(g_lfs_manager[idx]->lfs, pathname);
    lfs_unlock(g_lfs_manager[idx]->lock);

    if (ret < LFS_ERR_OK) LFS_ERROR("%s %s return %d", __func__, path, ret);
    ret = lfs_ret_value_convert(ret);

    aos_free(pathname);

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
#ifdef POSIX_DEVICE_IO_NEED
    .ioctl      = NULL,
#endif
    .utime      = &lfs_vfs_utime,
};

static int lfs_format_and_mount(int i)
{
    int ret = 0;

    LFS_WARN("Formatting %s ...", g_lfs_manager[i]->mountpath);
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

int lfs_vfs_mount(int i)
{
    int ret;

    if (i >= (sizeof(g_lfs_manager) / sizeof(g_lfs_manager[0]))) {
        LFS_ERROR("%s failed, Max lfs partition limit hit!", __func__);
        return -1;
    }

    _lfs_init(NULL, i);

    LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
    LFS_WARN("Mounting %s ...", g_lfs_manager[i]->mountpath);
    lfs_lock(g_lfs_manager[i]->lock);

    ret = lfs_mount(g_lfs_manager[i]->lfs, g_lfs_manager[i]->config);
    ret = lfs_ret_value_convert(ret);
    if (ret < 0) {
        LFS_WARN("Failed to mount %s!", g_lfs_manager[i]->mountpath);
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

    LFS_ASSERT(ret >= 0);

    g_lfs_manager[i]->mounted = true;
    lfs_unlock(g_lfs_manager[i]->lock);

    return ret;

ERROR:
    lfs_unlock(g_lfs_manager[i]->lock);

    return ret;
}

int lfs_vfs_unmount(int i)
{
    int ret;

    if (i >= (sizeof(g_lfs_manager) / sizeof(g_lfs_manager[0]))) {
        LFS_ERROR("%s failed, Max lfs partition limit hit!", __func__);
        return -1;
    }

    LFS_ASSERT(g_lfs_manager[i]->mountpath != NULL);
    LFS_WARN("Unmounting %s ...", g_lfs_manager[i]->mountpath);
    ret = lfs_unmount(g_lfs_manager[i]->lfs);
    if (ret != 0) goto end;

    _lfs_deinit(i);

end:
    return lfs_ret_value_convert(ret);
}

int32_t littlefs_register(const char *partition_name, const char *mnt_path)
{
    int ret, i = 0;
    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i] == NULL) {
            _lfs_init(partition_name, i);
            LFS_WARN("Registering %s, idx %d ...", mnt_path, i);
            g_lfs_manager[i]->mountpath = strdup(mnt_path);
            if (!g_lfs_manager[i]->mountpath) {
                LFS_ERROR("%s failed to malloc", __func__);
                return -1;
            }

            ret = lfs_vfs_mount(i);
            if (ret < 0) {
                LFS_ERROR("%s failed to mount lfs!", __func__);
                return ret;
            }

            ret = vfs_register_fs(g_lfs_manager[i]->mountpath, &littlefs_ops, NULL);

            if (ret != 0) return ret;
            else break;
        }

        i++;
    }

    if (i >= CONFIG_LITTLEFS_CNT) {
        LFS_ERROR("No more partition supported!");
        return -1;
    }

    return 0;
}

int32_t littlefs_unregister(const char *mnt_path)
{
    int ret, i = 0;

    while (i < CONFIG_LITTLEFS_CNT) {
        if (g_lfs_manager[i]->mountpath && \
            (strcmp(g_lfs_manager[i]->mountpath, mnt_path) == 0)) {
            lfs_lock(g_lfs_manager[i]->lock);

            ret = lfs_vfs_unmount(i);
            if (ret < 0) {
                return ret;
            }

            LFS_WARN("Unregistering %s ...", g_lfs_manager[i]->mountpath);
            ret = vfs_unregister_fs(g_lfs_manager[i]->mountpath);
            if (ret != 0) return ret;

            aos_free(g_lfs_manager[i]->mountpath);
            g_lfs_manager[i]->mountpath = NULL;
            g_lfs_manager[i] = NULL;

            lfs_unlock(g_lfs_manager[i]->lock);
        }

        i++;
    }

    return 0;
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

#ifdef AOS_COMP_NFTL
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
#endif /* AOS_COMP_NFTL */
