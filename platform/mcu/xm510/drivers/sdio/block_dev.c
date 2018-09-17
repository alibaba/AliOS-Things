/*
 * File      : block_dev.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2006, RT-Thread Development Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <k_api.h>
#include <aos/kernel.h>
#include <aos/list.h>
#include "mmcsd_core.h"


static dlist_t blk_devices =  AOS_DLIST_INIT(blk_devices);

struct mmcsd_blk_device
{
	struct rt_mmcsd_card *card;
	dlist_t list;
};


#define SECTOR_SIZE 512

static int32_t mmcsd_num_wr_blocks(struct rt_mmcsd_card *card)
{
    int32_t err;
    uint32_t blocks;

    struct rt_mmcsd_req req;
    struct rt_mmcsd_cmd cmd;
    struct rt_mmcsd_data data;
    uint32_t timeout_us;

    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = APP_CMD;
    cmd.arg = card->rca << 16;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_AC;

    err = mmcsd_send_cmd(card->host, &cmd, 0);
    if (err)
        return -1;
    if (!controller_is_spi(card->host) && !(cmd.resp[0] & R1_APP_CMD))
        return -1;

    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = SD_APP_SEND_NUM_WR_BLKS;
    cmd.arg = 0;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_ADTC;

    memset(&data, 0, sizeof(struct rt_mmcsd_data));

    data.timeout_ns = card->tacc_ns * 100;
    data.timeout_clks = card->tacc_clks * 100;

    timeout_us = data.timeout_ns / 1000;
    timeout_us += data.timeout_clks * 1000 /
        (card->host->io_cfg.clock / 1000);

    if (timeout_us > 100000) 
    {
        data.timeout_ns = 100000000;
        data.timeout_clks = 0;
    }

    data.blksize = 4;
    data.blks = 1;
    data.flags = DATA_DIR_READ;
    data.buf = &blocks;

    memset(&req, 0, sizeof(struct rt_mmcsd_req));

    req.cmd = &cmd;
    req.data = &data;

    mmcsd_send_request(card->host, &req);

    if (cmd.err || data.err)
        return -1;

    return blocks;
}

int rt_mmcsd_req_blk(struct rt_mmcsd_card *card,
                                 uint32_t           sector,
                                 void                 *buf,
                                 size_t             blks,
                                 uint8_t            dir)
{
	//void *aligned_buf;
    
    struct rt_mmcsd_cmd  cmd, stop;
    struct rt_mmcsd_data  data;
    struct rt_mmcsd_req  req;
    struct rt_mmcsd_host *host = card->host;
    uint32_t r_cmd, w_cmd;

    mmcsd_host_lock(host);
    memset(&req, 0, sizeof(struct rt_mmcsd_req));
    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));
    memset(&stop, 0, sizeof(struct rt_mmcsd_cmd));
    memset(&data, 0, sizeof(struct rt_mmcsd_data));
    req.cmd = &cmd;
    req.data = &data;
    
    cmd.arg = sector;
    if (!(card->flags & CARD_FLAG_SDHC)) 
    {
        cmd.arg <<= 9;
    }
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_ADTC;

    data.blksize = SECTOR_SIZE;
    data.blks  = blks;

    if (blks > 1) 
    {
        if (!controller_is_spi(card->host) || !dir)
        {
            req.stop = &stop;
            stop.cmd_code = STOP_TRANSMISSION;
            stop.arg = 0;
            stop.flags = RESP_SPI_R1B | RESP_R1B | CMD_AC;
        }
        r_cmd = READ_MULTIPLE_BLOCK;
        w_cmd = WRITE_MULTIPLE_BLOCK;
    }
    else
    {
        req.stop = NULL;
        r_cmd = READ_SINGLE_BLOCK;
        w_cmd = WRITE_BLOCK;
    }

    if (!dir) 
    {
        cmd.cmd_code = r_cmd;
        data.flags |= DATA_DIR_READ;
    }
    else
    {
        cmd.cmd_code = w_cmd;
        data.flags |= DATA_DIR_WRITE;
    }

    mmcsd_set_data_timeout(&data, card);
    data.buf = buf;
    mmcsd_send_request(host, &req);

    if (!controller_is_spi(card->host) && dir != 0) 
    {
        do 
        {
            int err;

            cmd.cmd_code = SEND_STATUS;
            cmd.arg = card->rca << 16;
            cmd.flags = RESP_R1 | CMD_AC;
            err = mmcsd_send_cmd(card->host, &cmd, 5);
            if (err) 
            {
                printf("error %d requesting status\n", err);
                break;
            }
            /*
             * Some cards mishandle the status bits,
             * so make sure to check both the busy
             * indication and the card state.
             */
         } while (!(cmd.resp[0] & R1_READY_FOR_DATA) ||
            (R1_CURRENT_STATE(cmd.resp[0]) == 7));
    }

    mmcsd_host_unlock(host);

    if (cmd.err || data.err || stop.err) 
    {
        printf("mmcsd request blocks error\n");
        printf("%ld,%ld,%ld, 0x%08lx,0x%08lx\n",
                   cmd.err, data.err, stop.err, data.flags, sector);

        return -1;
    }

    return 0;
}

#if 0

static int rt_mmcsd_init(rt_device_t dev)
{
    return 0;
}

static int rt_mmcsd_open(rt_device_t dev, rt_uint16_t oflag)
{
    return 0;
}

static int rt_mmcsd_close(rt_device_t dev)
{
    return 0;
}

static int rt_mmcsd_control(rt_device_t dev, rt_uint8_t cmd, void *args)
{
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    switch (cmd)
    {
    case RT_DEVICE_CTRL_BLK_GETGEOME:
        rt_memcpy(args, &blk_dev->geometry, sizeof(struct rt_device_blk_geometry));
        break;
    default:
        break;
    }
    return 0;
}

static rt_size_t rt_mmcsd_read(rt_device_t dev,
                               rt_off_t    pos,
                               void       *buffer,
                               rt_size_t   size)
{
    int err;
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    struct dfs_partition *part = &blk_dev->part;

    if (dev == NULL)
    {
        rt_set_errno(-DFS_STATUS_EINVAL);

        return 0;
    }

    rt_sem_take(part->lock, RT_WAITING_FOREVER);
    err = rt_mmcsd_req_blk(blk_dev->card, part->offset + pos, buffer, size, 0);
    rt_sem_release(part->lock);

    /* the length of reading must align to SECTOR SIZE */
    if (err) 
    {
        rt_set_errno(-DFS_STATUS_EIO);
        return 0;
    }
    return size;
}

static rt_size_t rt_mmcsd_write(rt_device_t dev,
                                rt_off_t    pos,
                                const void *buffer,
                                rt_size_t   size)
{
    int err;
    struct mmcsd_blk_device *blk_dev = (struct mmcsd_blk_device *)dev->user_data;
    struct dfs_partition *part = &blk_dev->part;

    if (dev == NULL)
    {
        rt_set_errno(-DFS_STATUS_EINVAL);

        return 0;
    }

    rt_sem_take(part->lock, RT_WAITING_FOREVER);
    err = rt_mmcsd_req_blk(blk_dev->card, part->offset + pos, (void *)buffer, size, 1);
    rt_sem_release(part->lock);

    /* the length of reading must align to SECTOR SIZE */
    if (err) 
    {
        rt_set_errno(-DFS_STATUS_EIO);

        return 0;
    }
    return size;
}

#endif

static int32_t mmcsd_set_blksize(struct rt_mmcsd_card *card)
{
    struct rt_mmcsd_cmd cmd;
    int err;

    /* Block-addressed cards ignore MMC_SET_BLOCKLEN. */
    if (card->flags & CARD_FLAG_SDHC)
        return 0;

    mmcsd_host_lock(card->host);
    cmd.cmd_code = SET_BLOCKLEN;
    cmd.arg = 512;
    cmd.flags = RESP_SPI_R1 | RESP_R1 | CMD_AC;
    err = mmcsd_send_cmd(card->host, &cmd, 5);
    mmcsd_host_unlock(card->host);

    if (err) 
    {
        printf("MMCSD: unable to set block size to %ld: %d\n", cmd.arg, err);

        return -1;
    }

    return 0;
}


int32_t rt_mmcsd_blk_probe(struct rt_mmcsd_card *card)
{
	int32_t err = 0;
    struct mmcsd_blk_device *blk_dev = NULL;

	err = mmcsd_set_blksize(card);
	if(err) 
	{   
		return err;
	}   

    blk_dev = aos_malloc(sizeof(struct mmcsd_blk_device));
	if (!blk_dev) 
	{
		printf("mmcsd:malloc memory failed!\n");
		return -1;
	}
    
    /* register mmcsd device */
    blk_dev->card = card;
    dlist_add(&blk_dev->list, &blk_devices);
    return 0;
}

void rt_mmcsd_blk_remove(struct rt_mmcsd_card *card)
{
    dlist_t *l, *n;
    struct mmcsd_blk_device *blk_dev;

    dlist_for_each_safe(l, n, &blk_devices)
    {
        blk_dev = (struct mmcsd_blk_device *)dlist_entry(l, struct mmcsd_blk_device, list);
        if (blk_dev->card == card)
        {
        	/* unmount file system */
           	dlist_del(&blk_dev->list);
            aos_free(blk_dev);
        }
    }
}

/*
 * This function will initialize block device on the mmc/sd.
 *
 * @deprecated since 2.1.0, this function does not need to be invoked
 * in the system initialization.
 */
void rt_mmcsd_blk_init(void)
{
	/* nothing */
}

//TODO 目前只支持一个 mmcsd_card设备
int32_t rt_mmcsd_blk_rescan(struct rt_mmcsd_card *card)
{
	int32_t ret = -1;
    dlist_t *l;
    struct mmcsd_blk_device *blk_dev;

    dlist_for_each(l, &blk_devices)
    {
        blk_dev = (struct mmcsd_blk_device *)dlist_entry(l, struct mmcsd_blk_device, list);
        if (blk_dev->card != NULL)
        {
			memcpy(card, blk_dev->card, sizeof(struct rt_mmcsd_card));
			ret = 0;
			break;
        }
    }
	return ret;
}


