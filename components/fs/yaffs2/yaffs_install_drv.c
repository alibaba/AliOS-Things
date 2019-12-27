/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "yportenv.h"
#include "yaffs_guts.h"
#include "yaffs_install_drv.h"

static int nand_WriteChunk(struct yaffs_dev *dev, int nand_chunk,
				   const u8 *data, int data_len,
				   const u8 *oob, int oob_len)
{
    /* add code according to your nand driver */
    return YAFFS_OK;
}

static int nand_ReadChunk(struct yaffs_dev *dev, int nand_chunk,
				   u8 *data, int data_len,
				   u8 *oob, int oob_len,
				   enum yaffs_ecc_result *ecc_result)
{
    /* add code according to your nand driver */
    return YAFFS_OK;
}

static int nand_EraseBlock(struct yaffs_dev *dev, int block_no)
{
    /* add code according to your nand driver */	
    return YAFFS_OK;
}

static int nand_MarkBad(struct yaffs_dev *dev, int block_no)
{
    /* add code according to your nand driver */
    return YAFFS_OK;
}

static int nand_CheckBad(struct yaffs_dev *dev, int block_no)
{
    /* add code according to your nand driver */
    return YAFFS_OK;
}

static int nand_Initialise(struct yaffs_dev *dev)
{
    /* add code according to your nand driver */
    return YAFFS_OK;
}

struct yaffs_dev *yaffs_install_drv(const char *name)
{
    struct yaffs_dev *dev = NULL;
    struct yaffs_param *param;
    struct yaffs_driver *drv;

    if(name == NULL) {
        return NULL;
    }

    dev = malloc(sizeof(*dev));

    if(dev == NULL)
    {
        return NULL;
    }

    memset(dev, 0, sizeof(*dev));

    dev->param.name = name;

    drv = &dev->drv;

    drv->drv_write_chunk_fn = nand_WriteChunk;
    drv->drv_read_chunk_fn = nand_ReadChunk;
    drv->drv_erase_fn = nand_EraseBlock;
    drv->drv_mark_bad_fn = nand_MarkBad;
    drv->drv_check_bad_fn = nand_CheckBad;
    drv->drv_initialise_fn = nand_Initialise;

    param = &dev->param;

    param->total_bytes_per_chunk = 2048;
    param->chunks_per_block = 64;
    param->start_block = 0;
    param->end_block = 8192;
    param->is_yaffs2 = 1;
    param->use_nand_ecc=1;

    param->n_reserved_blocks = 5;
    param->wide_tnodes_disabled=0;
    param->refresh_period = 1000;
    param->n_caches = 10; // Use caches

    param->enable_xattr = 1;

    yaffs_add_device(dev);

    return dev;
}
