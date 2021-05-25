/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */

#include <aos/mtdnor.h>
#include <drv/spiflash.h>

static int _nor_erase(aos_mtd_t *mtd, off_t addr, size_t len)
{
    (void) mtd;

    return csi_spiflash_erase(NULL, addr, len);
}

static int _nor_read(aos_mtd_t *mtd, off_t from, struct mtd_io_desc *desc)
{
    int ret;

    (void) mtd;

    ret = csi_spiflash_read(NULL, from, desc->datbuf, desc->datlen);
    if (ret > 0) {
        desc->datretlen = ret;
    }

    return ret;
}

static int _nor_write(aos_mtd_t *mtd, off_t to, struct mtd_io_desc *desc)
{
    int ret;

    (void) mtd;

    ret = csi_spiflash_program(NULL, to, desc->datbuf, desc->datlen);
    if (ret > 0) {
        desc->datretlen = ret;
    }

    return ret; 
}

static const struct mtd_ops _ops =
{
    _nor_erase,
    _nor_read,
    _nor_write,
    NULL,
    NULL
};

int aos_mtd_nor_init(aos_mtd_t *nor, int blksize, int sector_size)
{
    nor->sector_size = sector_size > 0 ? sector_size : 1;
    nor->block_size = blksize;
    nor->ops = &_ops;
    nor->type = MTD_TYPE_NOR;
    nor->oob_size = 0;

    return 0;
}
