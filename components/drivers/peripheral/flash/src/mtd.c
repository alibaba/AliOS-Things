/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <aos/mtd.h>
#include <aos/mtdpart.h>
#include <drivers/u_ld.h>

/**
 * keep a mapping for partition name and device name,
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

static struct mtd_info_ctx g_mtd_info = {0};

static aos_mtd_t* mtd_part_alloc(aos_mtd_t *master, const struct mtd_part *part)
{
    aos_mtd_t *slave;

    slave = malloc(sizeof(aos_mtd_t));
    if (slave == NULL)
        goto out;

    slave->master = master;

    *slave = *master;
    slave->size = part->size;
    slave->offset = part->offset;

out:

    return slave;
}

/* Caller is responsible to free the info memery */
int aos_mtd_part_info_get(struct mtd_part_info **info, int *cnt)
{
    if (g_mtd_info.np > 0) {
        int np = g_mtd_info.np;
        struct mtd_info_entry *entries = g_mtd_info.entries;
        struct mtd_part_info *p;

        *info = calloc(sizeof(struct mtd_part_info), np);
        if (*info == NULL) goto failure;

        p = *info;
        for (int i = 0; i < np; i++, p++) {
            memcpy(p, &(entries[i].pinfo), sizeof(struct mtd_part_info));
        }

        *cnt = np;
        return 0;
    }

failure:
    if (*info) free(*info);
    *info = NULL;
    *cnt = 0;
    return -1;
}

#if 0
static void aos_show_mtdparts()
{
    int cnt;
    char name[32];
    uint32_t size, offset;
    struct mtd_part_info *info = NULL;

    if (aos_mtd_part_info_get(&info, &cnt) == 0) {
        LOG("IDX :Name\tSize\tOffset\t\tDevice");
        LOG("----------------------------------------------------\r\n");
        for (int i = 0; i < cnt; i++) {
            memset(name, 0, sizeof(name));
            snprintf(name, sizeof(name), "MTD%d:%s\t", i+1, info[i].part_name);
            if (strlen(name) <= 8) {
                snprintf(name+strlen(name), sizeof(name) - strlen(name), "\t");
            }

            size = info[i].size >> 10; // in KB uint
            offset = info[i].offset;

            LOG("%s%dKB\t0x%08x\t/dev/%s", name, size, offset, info[i].dev_name);
        }

        free(info);
    }
}
#endif

aos_mtd_t* aos_mtd_get_by_device(const char *device_name)
{
    aos_mtd_t *mtd;

    mtd = (aos_mtd_t *)aos_device_find(device_name);
    if (mtd == NULL)
        return NULL;

    if (mtd->parent.type != AOS_Device_Class_MTD)
        return NULL;

    return mtd;
}

aos_mtd_t* aos_mtd_get_by_std_part(const char *partition_name)
{
    char *name;
    int index = 0;

    while (index < g_mtd_info.np) {
        name = g_mtd_info.entries[index].pinfo.part_name_std;
        if (strcmp(partition_name, name) == 0) break;
        index++;
    }

    if (index >= g_mtd_info.np) return NULL;

    return aos_mtd_get_by_device(g_mtd_info.entries[index].pinfo.dev_name);
}

aos_mtd_t* aos_mtd_get_by_vendor_part(const char *partition_name)
{
    char *name;
    int index = 0;

    while (index < g_mtd_info.np) {
        name = g_mtd_info.entries[index].pinfo.part_name;
        if (strcmp(partition_name, name) == 0) break;
        index++;
    }

    if (index >= g_mtd_info.np) return NULL;

    return aos_mtd_get_by_device(g_mtd_info.entries[index].pinfo.dev_name);
}

static int mtd_init(aos_device_t dev)
{
    return 0;
}

static int mtd_open(aos_device_t dev, uint16_t flags)
{
    return 0;
}

static int mtd_close(aos_device_t dev)
{
    return 0;
}

static size_t mtd_read(aos_device_t dev, long pos, void *buffer, uint32_t size)
{
    return (size_t)aos_mtd_read((aos_mtd_t *)dev, pos, (uint8_t *)buffer, size);
}

static size_t mtd_write(aos_device_t dev, long pos, const void *buffer, uint32_t size)
{
    return (size_t)aos_mtd_write((aos_mtd_t *)dev, pos, (const uint8_t *)buffer, size);
}

static int mtd_ioctl(aos_device_t dev, int cmd, void *arg)
{
    int ret = -1;
    aos_mtd_t *mtd = (aos_mtd_t *)dev;
    struct mtd_erase_info *einfo;

    switch (cmd) {
        case IOC_MTD_GET_SIZE:
            *((unsigned int *)arg) = mtd->size;
            ret = 0;
            break;
        case IOC_MTD_GET_OFFSET:
            *((off_t *)arg) = mtd->offset;
            ret = 0;
            break;
        case IOC_MTD_ERASE:
            einfo = (struct mtd_erase_info *)arg;
            ret = aos_mtd_erase(mtd, einfo->offset, einfo->length);
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

int aos_mtd_register(aos_mtd_t *master, const struct mtd_part *parts, int np)
{
    int ret;
    aos_mtd_t *slave;
    static int dev_idx = 0; // defined as static so as to support multiple chips
    int index = 0;
    char name[16];

    master->master = master;
    master->parent.type = AOS_Device_Class_MTD;
    master->parent.init = mtd_init;
    master->parent.open = mtd_open;
    master->parent.close = mtd_close;
    master->parent.read = mtd_read;
    master->parent.write = mtd_write;
    master->parent.control = mtd_ioctl;

    if (np > 0) {
        g_mtd_info.entries = calloc(np, sizeof(struct mtd_info_entry));
        if (!g_mtd_info.entries) return -1;
        g_mtd_info.np = np;

        master->offset = parts->offset;
        master->size = parts->size;

        // generata device name
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), MTD_DEVICE_NAME_FORMAT, dev_idx++);

        // setup mtd information context
        g_mtd_info.entries[index].pinfo.offset = parts->offset;
        g_mtd_info.entries[index].pinfo.size = parts->size;
        strncpy(g_mtd_info.entries[index].pinfo.part_name, parts->name, MTD_PARTITION_NAME_MAX);
        strncpy(g_mtd_info.entries[index].pinfo.part_name_std, parts->name_std, MTD_PARTITION_NAME_MAX);
        strncpy(g_mtd_info.entries[index].pinfo.dev_name, name, MTD_DEVICE_NAME_MAX);
        g_mtd_info.entries[index].mtd = master;

        // register device
        ret = aos_device_register((aos_device_t)master, name, 0);
        if (ret != 0)
            goto _out;

        np --;
        parts ++;
        index++;
    }

    while (np > 0) {
        slave = mtd_part_alloc(master, parts);
        if (!slave)
            break;

        slave->parent.init = mtd_init;
        slave->parent.open = mtd_open;
        slave->parent.close = mtd_close;
        slave->parent.read = mtd_read;
        slave->parent.write = mtd_write;
        slave->parent.control = mtd_ioctl;

        // generate device name
        memset(name, 0, sizeof(name));
        snprintf(name, sizeof(name), MTD_DEVICE_NAME_FORMAT, dev_idx++);

        // setup name map
        g_mtd_info.entries[index].pinfo.offset = parts->offset;
        g_mtd_info.entries[index].pinfo.size = parts->size;
        strncpy(g_mtd_info.entries[index].pinfo.part_name, parts->name, MTD_PARTITION_NAME_MAX);
        strncpy(g_mtd_info.entries[index].pinfo.part_name_std, parts->name_std, MTD_PARTITION_NAME_MAX);
        strncpy(g_mtd_info.entries[index].pinfo.dev_name, name, MTD_DEVICE_NAME_MAX);
        g_mtd_info.entries[index].mtd = slave;

        // register device
        ret = aos_device_register((aos_device_t)slave, name, 0);
        if (ret)
            break;

        parts ++;
        np --;
        index++;
    }

_out:
    return np;
}

aos_mtd_t* aos_mtd_open(const char *name)
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

int aos_mtd_init()
{
    return csi_flash_init();
}

VFS_DRIVER_ENTRY(aos_mtd_init)
