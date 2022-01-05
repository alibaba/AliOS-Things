/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <aos/mtd.h>
#include <aos/mtdpart.h>

#define AOS_MTD_PART_NAME_TYPE_STD 0    /* AliOS Things standard partition name. */
#define AOS_MTD_PART_NAME_TYPE_VENDOR 1 /* Vendor partition name. */
#define AOS_MTD_MAX_NUM_CHIP 16  /* Maxmium number of supported mtd chips. */

/* Keep a mapping for partition name and device name,
 * just in case users want to operation parition based
 * on device name (such as dd command).
 */
struct mtd_info_entry {
    struct mtd_part_info pinfo;
    aos_mtd_t *mtd;
};

struct mtd_info_ctx {
    int np;
    struct mtd_info_entry *entries;
};

static struct mtd_info_ctx g_mtd_info[AOS_MTD_MAX_NUM_CHIP];

static aos_mtd_t *mtd_part_alloc(aos_mtd_t *master)
{
    aos_mtd_t *slave;

    slave = malloc(sizeof(aos_mtd_t));
    if (slave == NULL)
        return NULL;

    slave->master = master;
    /* Inherit information from master.*/
    *slave = *master;

    return slave;
}

int aos_mtd_part_info_get(struct mtd_part_info **info, int *cnt)
{
    struct mtd_part_info *p ;

    *cnt = 0;
    *info = NULL;
    for (int chip_idx = 0; g_mtd_info[chip_idx].entries; chip_idx++) {
        *cnt += g_mtd_info[chip_idx].np;
        p = realloc(*info, *cnt * sizeof(struct mtd_part_info));
        if (p == NULL) {
            free(*info);
            return -ENOMEM;
        }
        *info = p;

        p = *info + *cnt;
        for (int i = 0; i < g_mtd_info[chip_idx].np; i++, p++)
            memcpy(p, &(g_mtd_info[chip_idx].entries[i].pinfo), sizeof(struct mtd_part_info));
    }
    return 0;
}

static aos_mtd_t *aos_mtd_get_by_device(uint32_t id)
{
    aos_status_t ret;
    aos_mtd_t *mtd = NULL;
    aos_dev_ref_t ref = AOS_DEV_REF_INIT_VAL;

    ret = aos_dev_get(&ref, AOS_DEV_TYPE_MTD, id);
    if (ret != 0)
        return NULL;

    mtd = aos_container_of((&ref)->dev, aos_mtd_t, dev);
    if ((mtd == NULL) || (mtd->dev.type != AOS_DEV_TYPE_MTD))
        return NULL;

    return mtd;
}

static aos_mtd_t *aos_mtd_get_by_part_name(const char *partition_name, int type)
{
    int ret;
    uint32_t id = 0;
    int chip_idx;
    int i;

    for (chip_idx = 0; g_mtd_info[chip_idx].entries; chip_idx++) {
        for (i = 0; i < g_mtd_info[chip_idx].np; i++) {
            if (type == AOS_MTD_PART_NAME_TYPE_STD)
                ret = strcmp(partition_name, g_mtd_info[chip_idx].entries[i].pinfo.part_name_std);
            else
                ret = strcmp(partition_name, g_mtd_info[chip_idx].entries[i].pinfo.part_name);
            if (ret == 0)
                return aos_mtd_get_by_device(id);
            id++;
        }
    }

    return NULL;
}

/* Keep this api for backward compatibility. */
aos_mtd_t *aos_mtd_get_by_std_part(const char *partition_name)
{
    return aos_mtd_get_by_part_name(partition_name, AOS_MTD_PART_NAME_TYPE_STD);
}

/* Keep this api for backward compatibility. */
aos_mtd_t *aos_mtd_get_by_vendor_part(const char *partition_name)
{
    return aos_mtd_get_by_part_name(partition_name, AOS_MTD_PART_NAME_TYPE_VENDOR);
}

static int mtd_open(inode_t *node, file_t *fp)
{
    return 0;
}

static int mtd_close(file_t *fp)
{
    return 0;
}

static ssize_t mtd_read(file_t *fp, void *buffer, size_t size)
{
    off_t pos = fp->offset;
    aos_dev_t *dev = fp->node->i_arg;
    aos_mtd_t *mtd = aos_container_of(dev, aos_mtd_t, dev);

    return aos_mtd_read(mtd, pos, buffer, size);
}

static ssize_t mtd_write(file_t *fp, const void *buffer, size_t size)
{
    off_t pos = fp->offset;
    aos_dev_t *dev = fp->node->i_arg;
    aos_mtd_t *mtd = aos_container_of(dev, aos_mtd_t, dev);

    return aos_mtd_write(mtd, pos, buffer, size);
}

static int mtd_ioctl(file_t *fp, int cmd, unsigned long arg)
{
    aos_dev_t *dev = fp->node->i_arg;
    aos_mtd_t *mtd = aos_container_of(dev, aos_mtd_t, dev);
    struct mtd_erase_info *einfo;

    switch (cmd) {
        case IOC_MTD_GET_SIZE:
            *((unsigned int *)arg) = mtd->size;
            return 0;
        case IOC_MTD_GET_OFFSET:
            *((off_t *)arg) = mtd->offset;
            return 0;
        case IOC_MTD_ERASE:
            einfo = (struct mtd_erase_info *)arg;
            return aos_mtd_erase(mtd, einfo->offset, einfo->length);
        default:
            return -EINVAL;
    }
}

static uint32_t mtd_lseek(file_t *fp, int64_t off, int32_t whence)
{
    switch (whence) {
    case SEEK_CUR:
        off = fp->offset + off;
        break;
    /* case SEEK_END: break; */
    case SEEK_SET:
        fp->offset = off;
        break;
    }

    return off;
}

static void mtd_unregister(struct aos_dev *dev)
{
    return;
}

static aos_status_t mtd_get(aos_dev_ref_t *ref)
{
    return 0;
}

static void mtd_put(aos_dev_ref_t *ref)
{
    return;
}

static aos_dev_ops_t mtd_dev_ops = {
    .unregister = mtd_unregister,
    .get = mtd_get,
    .put = mtd_put,
};

#ifdef AOS_COMP_VFS
static struct file_ops mtd_dev_fops = {
    .open = mtd_open,
    .close = mtd_close,
    .read = mtd_read,
    .write = mtd_write,
    .ioctl = mtd_ioctl,
    .poll = NULL,
    .lseek = mtd_lseek,
};
#endif

int aos_mtd_register(aos_mtd_t *master, const struct mtd_part *parts, int np)
{
    int ret;
    aos_mtd_t *mtd_device = NULL;
    static int dev_idx = 0; /* Use static to support multiple chips. */
    static int chip_idx = 0; /* Define here because flash chip is unpluggable. */
    static int is_master = 1;
    int i = 0;
    char device_name[MTD_DEVICE_NAME_MAX + 1];

    if ((master == NULL) || (parts == NULL) || (np <= 0))
        return -EINVAL;

    g_mtd_info[chip_idx].entries = calloc(np, sizeof(struct mtd_info_entry));
    if (g_mtd_info[chip_idx].entries == NULL)
        return -ENOMEM;
    g_mtd_info[chip_idx].np = np;

    while (np > 0) {
        if (is_master) {
            mtd_device = master;
            mtd_device->master = master;
            is_master = 0;
        } else {
            mtd_device = mtd_part_alloc(master);
            if (mtd_device == NULL) {
                free(g_mtd_info[chip_idx].entries);
                return -ENOMEM;
            }
        }

        /* Set mtd device information. */
        mtd_device->offset = parts->offset;
        mtd_device->size = parts->size;
        mtd_device->dev.type = AOS_DEV_TYPE_MTD;
        mtd_device->dev.id = dev_idx++;
        memset(device_name, 0, sizeof(device_name));
        snprintf(device_name, sizeof(device_name) - 1, MTD_DEVICE_NAME_FORMAT, mtd_device->dev.id);
#ifdef AOS_COMP_VFS
        /*TODO: device name should like mtdblock0p1 */
        strncpy(mtd_device->dev.vfs_helper.name, device_name, AOS_DEV_NAME_MAX_LEN);
        mtd_device->dev.vfs_helper.ops = &mtd_dev_fops;
#endif
        mtd_device->dev.ops = &mtd_dev_ops;

        /* Set partition information.*/
        g_mtd_info[chip_idx].entries[i].pinfo.offset = parts->offset;
        g_mtd_info[chip_idx].entries[i].pinfo.size = parts->size;
        strncpy(g_mtd_info[chip_idx].entries[i].pinfo.part_name,
            parts->name, MTD_PARTITION_NAME_MAX - 1);
        strncpy(g_mtd_info[chip_idx].entries[i].pinfo.part_name_std,
            parts->name_std, MTD_PARTITION_NAME_MAX - 1);
        strncpy(g_mtd_info[chip_idx].entries[i].pinfo.dev_name,
            device_name, MTD_DEVICE_NAME_MAX - 1);
        g_mtd_info[chip_idx].entries[i].mtd = mtd_device;

        ret = aos_dev_register(&(mtd_device->dev));
        if (ret != 0) {
            free(g_mtd_info[chip_idx].entries);
            if (!is_master)
                free(mtd_device);
            return -1;
        }

        np--;
        parts++;
        i++;
    }
    chip_idx++;

    return 0;
}

aos_mtd_t *aos_mtd_open(const char *name)
{
    aos_mtd_t *mtd_dev = NULL;

    if (name == NULL) {
        return NULL;
    }

    mtd_dev = aos_mtd_get_by_vendor_part(name);
    if (mtd_dev != NULL) {
        return mtd_dev;
    }

    /* Try to find standard partition name if it can't be found in
     * vendor partition name.
     */
    mtd_dev = aos_mtd_get_by_std_part(name);

    return mtd_dev;
}

aos_status_t aos_mtd_close(aos_mtd_t *mtd)
{
    return 0;
}

ssize_t aos_mtd_read(aos_mtd_t *mtd, off_t offset, void *buf, size_t size)
{
    struct mtd_io_desc info = {0};

    if (mtd == NULL || offset < 0 || offset >= (off_t)mtd->size ||
        buf == NULL || size > mtd->size - offset || size <= 0) {
        return -EINVAL;
    }

    info.datbuf = buf;
    info.datlen = size;

    return mtd->ops->read(mtd->master, offset + mtd->offset, &info);
}

ssize_t aos_mtd_oob_read(aos_mtd_t *mtd, off_t offset, struct mtd_io_desc *info)
{
    if (mtd == NULL || info == NULL) {
        return -EINVAL;
    }

    info->datretlen = 0;
    info->oobretlen = 0;

    if (offset < 0 || offset >= (off_t)mtd->size)
        return -EINVAL;

    if (info->datbuf && (info->datlen > (mtd->size - offset)))
        return -EINVAL;

    return mtd->ops->read(mtd->master, offset + mtd->offset, info);
}

ssize_t aos_mtd_write(aos_mtd_t *mtd, off_t offset, const void *buf, size_t size)
{
    struct mtd_io_desc info = {0};

    if (mtd == NULL || offset < 0 || offset >= (off_t)mtd->size ||
        buf == NULL || size > mtd->size - offset || size <= 0) {
        return -EINVAL;
    }
    if (mtd->ops->write == NULL) {
        return -ENOTSUP;
    }

    info.datbuf = (uint8_t*)buf;
    info.datlen = size;

    return mtd->ops->write(mtd->master, offset + mtd->offset, &info);
}

ssize_t aos_mtd_oob_write(aos_mtd_t *mtd, off_t offset, struct mtd_io_desc *info)
{
    if ((mtd == NULL) || (info == NULL)) {
        return -EINVAL;
    }

    if (mtd->ops->write == NULL) {
        return -ENOTSUP;
    }

    info->datretlen = 0;
    info->oobretlen = 0;

    if (offset < 0 || offset >= (off_t)mtd->size) {
        return -EINVAL;
    }

    if (info->datbuf && (info->datlen > (mtd->size - offset))) {
        return -EINVAL;
    }

    return mtd->ops->write(mtd->master, offset + mtd->offset, info);
}

aos_status_t aos_mtd_erase(aos_mtd_t *mtd, off_t offset, size_t size)
{
    if (mtd == NULL || size <= 0) {
        return -EINVAL;
    }

    if (mtd->ops->erase == NULL) {
        return -ENOTSUP;
    }

    if (offset < 0 || (offset + size) > mtd->size) {
        return -EINVAL;
    }
    offset += mtd->offset;

    return mtd->ops->erase(mtd->master, offset, size);
}

aos_status_t aos_mtd_block_erase(aos_mtd_t *mtd, uint32_t block)
{
    uint32_t total_blks;
    off_t offset;

    if (mtd == NULL) {
        return -EINVAL;
    }

    if (mtd->ops->erase == NULL) {
        return -ENOTSUP;
    }

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks) {
        return -EINVAL;
    }
    offset = mtd->offset + mtd->block_size * block;

    return mtd->ops->erase(mtd->master, offset, mtd->block_size);
}

aos_status_t aos_mtd_block_mark_bad(aos_mtd_t *mtd, uint32_t block)
{
    uint32_t total_blks, offset_blk;

    if (mtd == NULL) {
        return -EINVAL;
    }

    if (mtd->ops->markbad == NULL) {
        return -ENOTSUP;
    }

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks) {
        return -EINVAL;
    }
    offset_blk = mtd->offset / mtd->block_size;

    return mtd->ops->markbad(mtd->master, block + offset_blk);
}

aos_status_t aos_mtd_block_is_bad(aos_mtd_t *mtd, uint32_t block)
{
    uint32_t total_blks, offset_blk;

    if (mtd == NULL) {
        return -EINVAL;
    }

    if (mtd->ops->isbad == NULL) {
        return -ENOTSUP;
    }

    total_blks = mtd->size / mtd->block_size;
    if (block >= total_blks) {
        return -EINVAL;
    }
    offset_blk = mtd->offset / mtd->block_size;

    return mtd->ops->isbad(mtd->master, block + offset_blk);
}
