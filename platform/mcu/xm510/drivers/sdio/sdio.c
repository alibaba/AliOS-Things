/*
 * File      : sdio.c
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
#include <assert.h>
#include "mmcsd_core.h"
#include "sdio.h"
#include "sd.h"

#ifndef RT_SDIO_STACK_SIZE
#define RT_SDIO_STACK_SIZE 512
#endif
#ifndef RT_SDIO_THREAD_PRIORITY
#define RT_SDIO_THREAD_PRIORITY  0x40
#endif

static dlist_t sdio_cards;
static dlist_t sdio_drivers;

struct sdio_card
{
    struct rt_mmcsd_card *card;
    dlist_t  list;
};

struct sdio_driver
{
    struct rt_sdio_driver *drv;
    dlist_t  list;
};

#define MIN(a, b) (a < b ? a : b)

static const uint8_t speed_value[16] =
{
    0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

static const uint32_t speed_unit[8] =
{
    10000, 100000, 1000000, 10000000, 0, 0, 0, 0
};

static inline int32_t sdio_match_card(struct rt_mmcsd_card           *card,
                                     const struct rt_sdio_device_id *id);


int32_t sdio_io_send_op_cond(struct rt_mmcsd_host *host,
                                uint32_t           ocr,
                                uint32_t          *cmd5_resp)
{
    struct rt_mmcsd_cmd cmd;
    int32_t i, err = 0;

    assert(host != NULL);

    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = SD_IO_SEND_OP_COND;
    cmd.arg = ocr;
    cmd.flags = RESP_SPI_R4 | RESP_R4 | CMD_BCR;

    for (i = 100; i; i--) 
    {
        err = mmcsd_send_cmd(host, &cmd, 0);
        if (err)
            break;

        /* if we're just probing, do a single pass */
        if (ocr == 0)
            break;

        /* otherwise wait until reset completes */
        if (controller_is_spi(host)) 
        {
            /*
             * Both R1_SPI_IDLE and MMC_CARD_BUSY indicate
             * an initialized card under SPI, but some cards
             * (Marvell's) only behave when looking at this
             * one.
             */
            if (cmd.resp[1] & CARD_BUSY)
                break;
        } 
        else 
        {
            if (cmd.resp[0] & CARD_BUSY)
                break;
        }

        err = -1;

        mmcsd_delay_ms(10);
    }

    if (cmd5_resp)
        *cmd5_resp = cmd.resp[controller_is_spi(host) ? 1 : 0];

    return err;
}

int32_t sdio_io_rw_direct(struct rt_mmcsd_card *card,
                             int32_t            rw,
                             uint32_t           fn,
                             uint32_t           reg_addr,
                             uint8_t           *pdata,
                             uint8_t            raw)
{
    struct rt_mmcsd_cmd cmd;
    int32_t err;

    assert(card != NULL);
    assert(fn <= SDIO_MAX_FUNCTIONS);

    if (reg_addr & ~SDIO_ARG_CMD53_REG_MASK)
        return -1;

    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));

    cmd.cmd_code = SD_IO_RW_DIRECT;
    cmd.arg = rw ? SDIO_ARG_CMD52_WRITE : SDIO_ARG_CMD52_READ;
    cmd.arg |= fn << SDIO_ARG_CMD52_FUNC_SHIFT;
    cmd.arg |= raw ? SDIO_ARG_CMD52_RAW_FLAG : 0x00000000;
    cmd.arg |= reg_addr << SDIO_ARG_CMD52_REG_SHIFT;
    cmd.arg |= *pdata;
    cmd.flags = RESP_SPI_R5 | RESP_R5 | CMD_AC;

    err = mmcsd_send_cmd(card->host, &cmd, 0);
    if (err)
        return err;

    if (!controller_is_spi(card->host)) 
    {
        if (cmd.resp[0] & R5_ERROR)
            return -1;
        if (cmd.resp[0] & R5_FUNCTION_NUMBER)
            return -1;
        if (cmd.resp[0] & R5_OUT_OF_RANGE)
            return -1;
    }

    if (!rw || raw) 
    {
        if (controller_is_spi(card->host))
            *pdata = (cmd.resp[0] >> 8) & 0xFF;
        else
            *pdata = cmd.resp[0] & 0xFF;
    }

    return 0;
}

int32_t sdio_io_rw_extended(struct rt_mmcsd_card *card,
                               int32_t            rw,
                               uint32_t           fn,
                               uint32_t           addr,
                               int32_t            op_code,
                               uint8_t           *buf,
                               uint32_t           blocks,
                               uint32_t           blksize)
{
    struct rt_mmcsd_req req;
    struct rt_mmcsd_cmd cmd;
    struct rt_mmcsd_data data;

    assert(card != NULL);
    assert(fn <= SDIO_MAX_FUNCTIONS);
    assert(blocks != 1 || blksize <= 512);
    assert(blocks != 0);
    assert(blksize != 0);

    if (addr & ~SDIO_ARG_CMD53_REG_MASK)
        return -1;

    memset(&req, 0, sizeof(struct rt_mmcsd_req));
    memset(&cmd, 0, sizeof(struct rt_mmcsd_cmd));
    memset(&data, 0, sizeof(struct rt_mmcsd_data));

    req.cmd = &cmd;
    req.data = &data;

    cmd.cmd_code = SD_IO_RW_EXTENDED;
    cmd.arg = rw ? SDIO_ARG_CMD53_WRITE : SDIO_ARG_CMD53_READ;
    cmd.arg |= fn << SDIO_ARG_CMD53_FUNC_SHIFT;
    cmd.arg |= op_code ? SDIO_ARG_CMD53_INCREMENT : 0x00000000;
    cmd.arg |= addr << SDIO_ARG_CMD53_REG_SHIFT;
    if (blocks == 1 && blksize <= 512)
        cmd.arg |= (blksize == 512) ? 0 : blksize;      /* byte mode */
    else
        cmd.arg |= SDIO_ARG_CMD53_BLOCK_MODE | blocks;  /* block mode */
    cmd.flags = RESP_SPI_R5 | RESP_R5 | CMD_ADTC;

    data.blksize = blksize;
    data.blks = blocks;
    data.flags = rw ? DATA_DIR_WRITE : DATA_DIR_READ;
    data.buf = (uint32_t *)buf;

    mmcsd_set_data_timeout(&data, card);

    mmcsd_send_request(card->host, &req);

    if (cmd.err)
        return cmd.err;
    if (data.err)
        return data.err;

    if (!controller_is_spi(card->host)) 
    {
        if (cmd.resp[0] & R5_ERROR)
            return -1;
        if (cmd.resp[0] & R5_FUNCTION_NUMBER)
            return -1;
        if (cmd.resp[0] & R5_OUT_OF_RANGE)
            return -1;
    }

    return 0;
}

static inline uint32_t sdio_max_block_size(struct rt_sdio_function *func)
{
    uint32_t size = MIN(func->card->host->max_seg_size,
                           func->card->host->max_blk_size);
    size = MIN(size, func->max_blk_size);

    return MIN(size, 512u); /* maximum size for byte mode */
}

int32_t sdio_io_rw_extended_block(struct rt_sdio_function *func,
                                            int32_t               rw,
                                            uint32_t              addr,
                                            int32_t               op_code,
                                            uint8_t              *buf,
                                            uint32_t              len)
{
    int32_t  ret;
    uint32_t left_size;
    uint32_t max_blks, blks;
    
    left_size = len;

    /* Do the bulk of the transfer using block mode (if supported). */
    if (func->card->cccr.multi_block && (len > sdio_max_block_size(func)))
    {
        max_blks = MIN(func->card->host->max_blk_count,
                       func->card->host->max_seg_size / func->cur_blk_size);
        max_blks = MIN(max_blks, 511u);

        while (left_size > func->cur_blk_size)
        {
            blks = left_size / func->cur_blk_size;
            if (blks > max_blks)
                blks = max_blks;
            len = blks * func->cur_blk_size;

            ret = sdio_io_rw_extended(func->card, rw, func->num, 
                  addr, op_code, buf, blks, func->cur_blk_size);
            if (ret)
                return ret;

            left_size -= len;
            buf += len;
            if (op_code)
                addr += len;
        }
    }

    while (left_size > 0)
    {
        len = MIN(left_size, sdio_max_block_size(func));

        ret = sdio_io_rw_extended(func->card, rw, func->num, 
                  addr, op_code, buf, 1, len);
        if (ret)
            return ret;

        left_size -= len;
        buf += len;
        if (op_code)
            addr += len;
    }

    return 0;
}

uint8_t sdio_io_readb(struct rt_sdio_function *func, 
                         uint32_t              reg,
                         int32_t              *err)
{
    uint8_t data;
    int32_t ret;

    ret = sdio_io_rw_direct(func->card, 0, func->num, reg, &data, 0);

    if (err)
    {
        *err = ret;
    }

    return data;
}

int32_t sdio_io_writeb(struct rt_sdio_function *func, 
                          uint32_t              reg,
                          uint8_t               data)
{
    return sdio_io_rw_direct(func->card, 1, func->num, reg, &data, 0);
}

uint16_t sdio_io_readw(struct rt_sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err)
{
    int32_t ret;
    uint32_t dmabuf;

    if (err)
        *err = 0;

    ret = sdio_io_rw_extended_block(func, 0, addr, 1, (uint8_t *)&dmabuf, 2);
    if (ret) 
    {
        if (err)
            *err = ret;
    }

    return (uint16_t)dmabuf;
}

int32_t sdio_io_writew(struct rt_sdio_function *func,
                          uint16_t              data,
                          uint32_t              addr)
{
    uint32_t dmabuf = data;

    return sdio_io_rw_extended_block(func, 1, addr, 1, (uint8_t *)&dmabuf, 2);
}

uint32_t sdio_io_readl(struct rt_sdio_function *func,
                          uint32_t              addr,
                          int32_t              *err)
{
    int32_t ret;
    uint32_t dmabuf;

    if (err)
        *err = 0;

    ret = sdio_io_rw_extended_block(func, 0, addr, 1, (uint8_t *)&dmabuf, 4);
    if (ret) 
    {
        if (err)
            *err = ret;
    }

    return dmabuf;
}

int32_t sdio_io_writel(struct rt_sdio_function *func,
                          uint32_t              data,
                          uint32_t              addr)
{
    uint32_t dmabuf = data;

    return sdio_io_rw_extended_block(func, 1, addr, 1, (uint8_t *)&dmabuf, 4);
}

int32_t sdio_io_read_multi_fifo_b(struct rt_sdio_function *func, 
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 0, addr, 0, buf, len);
}

int32_t sdio_io_write_multi_fifo_b(struct rt_sdio_function *func, 
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 1, addr, 0, buf, len);
}

int32_t sdio_io_read_multi_incr_b(struct rt_sdio_function *func, 
                                     uint32_t              addr,
                                     uint8_t              *buf,
                                     uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 0, addr, 1, buf, len);
}

int32_t sdio_io_write_multi_incr_b(struct rt_sdio_function *func, 
                                      uint32_t              addr,
                                      uint8_t              *buf,
                                      uint32_t              len)
{
    return sdio_io_rw_extended_block(func, 1, addr, 1, buf, len);
}

static int32_t sdio_read_cccr(struct rt_mmcsd_card *card)
{
    int32_t ret;
    int32_t cccr_version;
    uint8_t data;

    memset(&card->cccr, 0, sizeof(struct rt_sdio_cccr));

    data = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_CCCR_REV, &ret);
    if (ret)
        goto out;

    cccr_version = data & 0x0f;

    if (cccr_version > SDIO_CCCR_REV_3_00) 
    {
        printf("unrecognised CCCR structure version %ld\n", cccr_version);

        return -1;
    }

    card->cccr.sdio_version = (data & 0xf0) >> 4;

    data = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_CARD_CAPS, &ret);
    if (ret)
        goto out;

    if (data & SDIO_CCCR_CAP_SMB)
        card->cccr.multi_block = 1;
    if (data & SDIO_CCCR_CAP_LSC)
        card->cccr.low_speed = 1;
    if (data & SDIO_CCCR_CAP_4BLS)
        card->cccr.low_speed_4 = 1;
    if (data & SDIO_CCCR_CAP_4BLS)
        card->cccr.bus_width = 1;

    if (cccr_version >= SDIO_CCCR_REV_1_10) 
    {
        data = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_POWER_CTRL, &ret);
        if (ret)
            goto out;

        if (data & SDIO_POWER_SMPC)
            card->cccr.power_ctrl = 1;
    }

    if (cccr_version >= SDIO_CCCR_REV_1_20) 
    {
        data = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_SPEED, &ret);
        if (ret)
            goto out;

        if (data & SDIO_SPEED_SHS)
            card->cccr.high_speed = 1;
    }

out:
    return ret;
}

static int32_t cistpl_funce_func0(struct rt_mmcsd_card *card,
                                     const uint8_t     *buf,
                                     uint32_t           size)
{
    if (size < 0x04 || buf[0] != 0)
        return -1;

    /* TPLFE_FN0_BLK_SIZE */
    card->cis.func0_blk_size = buf[1] | (buf[2] << 8);

    /* TPLFE_MAX_TRAN_SPEED */
    card->cis.max_tran_speed = speed_value[(buf[3] >> 3) & 15] *
                speed_unit[buf[3] & 7];

    return 0;
}

static int32_t cistpl_funce_func(struct rt_sdio_function *func,
                                    const uint8_t        *buf,
                                    uint32_t              size)
{
    uint32_t version;
    uint32_t min_size;

    version = func->card->cccr.sdio_version;
    min_size = (version == SDIO_SDIO_REV_1_00) ? 28 : 42;

    if (size < min_size || buf[0] != 1)
        return -1;

    /* TPLFE_MAX_BLK_SIZE */
    func->max_blk_size = buf[12] | (buf[13] << 8);

    /* TPLFE_ENABLE_TIMEOUT_VAL, present in ver 1.1 and above */
    if (version > SDIO_SDIO_REV_1_00)
        func->enable_timeout_val = (buf[28] | (buf[29] << 8)) * 10;
    else
        func->enable_timeout_val = 1000; /* 1000ms */

    return 0;
}

static int32_t sdio_read_cis(struct rt_sdio_function *func)
{
    int32_t ret;
    struct rt_sdio_function_tuple *curr, **prev;
    uint32_t i, cisptr = 0;
    uint8_t data;
    uint8_t tpl_code, tpl_link;

    struct rt_mmcsd_card *card = func->card;
    struct rt_sdio_function *func0 = card->sdio_function[0];

    assert(func0 != NULL);

    for (i = 0; i < 3; i++)
    {
        data = sdio_io_readb(func0, 
            SDIO_REG_FBR_BASE(func->num) + SDIO_REG_FBR_CIS + i, &ret);
        if (ret)
            return ret;
        cisptr |= data << (i * 8);
    }

    prev = &func->tuples;

    do {
        tpl_code = sdio_io_readb(func0, cisptr++, &ret);
        if (ret)
            break;
        tpl_link = sdio_io_readb(func0, cisptr++, &ret);
        if (ret)
            break;

        if ((tpl_code == CISTPL_END) || (tpl_link == 0xff))
            break;

        if (tpl_code == CISTPL_NULL)
            continue;


        curr = aos_malloc(sizeof(struct rt_sdio_function_tuple) + tpl_link);
        if (!curr)
            return -1;
        curr->data = (uint8_t *)curr + sizeof(struct rt_sdio_function_tuple);

        for (i = 0; i < tpl_link; i++) 
        {
            curr->data[i] = sdio_io_readb(func0, cisptr + i, &ret);
            if (ret)
                break;
        }
        if (ret) 
        {
            aos_free(curr);
            break;
        }

        switch (tpl_code)
        {
        case CISTPL_MANFID:
            if (tpl_link < 4)
            {
                printf("bad CISTPL_MANFID length\n");
                break;
            }
            if (func->num != 0)
            {
                func->manufacturer = curr->data[0];
                func->manufacturer |= curr->data[1] << 8;
                func->product = curr->data[2];
                func->product |= curr->data[3] << 8;
            }
            else
            {
                card->cis.manufacturer = curr->data[0];
                card->cis.manufacturer |= curr->data[1] << 8;
                card->cis.product = curr->data[2];
                card->cis.product |= curr->data[3] << 8;
            }
            break;
        case CISTPL_FUNCE:
            if (func->num != 0)
                ret = cistpl_funce_func(func, curr->data, tpl_link);
            else
                ret = cistpl_funce_func0(card, curr->data, tpl_link);

            if (ret)
            {
                printf("bad CISTPL_FUNCE size %u "
                       "type %u\n", tpl_link, curr->data[0]);
            }

            break;
        case CISTPL_VERS_1:
            if (tpl_link < 2)
            {
                printf("CISTPL_VERS_1 too short\n");
            }
            break;
        default: 
            /* this tuple is unknown to the core */
            curr->next = NULL;
            curr->code = tpl_code;
            curr->size = tpl_link;
            *prev = curr;
            prev = &curr->next;
            printf( "function %d, CIS tuple code %#x, length %d\n",
                func->num, tpl_code, tpl_link);
            break;
        }

        cisptr += tpl_link;
    } while (1);

    /*
     * Link in all unknown tuples found in the common CIS so that
     * drivers don't have to go digging in two places.
     */
    if (func->num != 0)
        *prev = func0->tuples;

    return ret;
}


void sdio_free_cis(struct rt_sdio_function *func)
{
    struct rt_sdio_function_tuple *tuple, *tmp;
    struct rt_mmcsd_card *card = func->card;

    tuple = func->tuples;

    while (tuple && ((tuple != card->sdio_function[0]->tuples) || (!func->num))) 
    {
        tmp = tuple;
        tuple = tuple->next;
        aos_free(tmp);
    }

    func->tuples = NULL;
}

static int32_t sdio_read_fbr(struct rt_sdio_function *func)
{
    int32_t ret;
    uint8_t data;
    struct rt_sdio_function *func0 = func->card->sdio_function[0];

    data = sdio_io_readb(func0, 
        SDIO_REG_FBR_BASE(func->num) + SDIO_REG_FBR_STD_FUNC_IF, &ret);
    if (ret)
        goto err;

    data &= 0x0f;

    if (data == 0x0f) 
    {
        data = sdio_io_readb(func0, 
            SDIO_REG_FBR_BASE(func->num) + SDIO_REG_FBR_STD_IF_EXT, &ret);
        if (ret)
            goto err;
    }

    func->func_code = data;

err:
    return ret;
}


static int32_t sdio_initialize_function(struct rt_mmcsd_card *card,
                                           uint32_t           func_num)
{
    int32_t ret;
    struct rt_sdio_function *func;

    assert(func_num <= SDIO_MAX_FUNCTIONS);

    func = aos_malloc(sizeof(struct rt_sdio_function));
    if (!func)
    {
        printf("malloc rt_sdio_function failed\n");
        ret = -1;
        goto err;
    }
    memset(func, 0, sizeof(struct rt_sdio_function));

    func->card = card;
    func->num = func_num;

    ret = sdio_read_fbr(func);
    if (ret)
        goto err1;

    ret = sdio_read_cis(func);
    if (ret)
        goto err1;

    card->sdio_function[func_num] = func;

    return 0;

err1:
    sdio_free_cis(func);
    aos_free(func);
    card->sdio_function[func_num] = NULL;
err:
    return ret;
}

static int32_t sdio_set_highspeed(struct rt_mmcsd_card *card)
{
    int32_t ret;
    uint8_t speed;

    if (!(card->host->flags & MMCSD_SUP_HIGHSPEED))
        return 0;

    if (!card->cccr.high_speed)
        return 0;

    speed = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_SPEED, &ret);
    if (ret)
        return ret;

    speed |= SDIO_SPEED_EHS;

    ret = sdio_io_writeb(card->sdio_function[0], SDIO_REG_CCCR_SPEED, speed);
    if (ret)
        return ret;

    card->flags |= CARD_FLAG_HIGHSPEED;

    return 0;
}

static int32_t sdio_set_bus_wide(struct rt_mmcsd_card *card)
{
    int32_t ret;
    uint8_t busif;

    if (!(card->host->flags & MMCSD_BUSWIDTH_4))
        return 0;

    if (card->cccr.low_speed && !card->cccr.bus_width)
        return 0;

    busif = sdio_io_readb(card->sdio_function[0], SDIO_REG_CCCR_BUS_IF, &ret);
    if (ret)
        return ret;

    busif |= SDIO_BUS_WIDTH_4BIT;

    ret = sdio_io_writeb(card->sdio_function[0], SDIO_REG_CCCR_BUS_IF, busif);
    if (ret)
        return ret;

    mmcsd_set_bus_width(card->host, MMCSD_BUS_WIDTH_4);

    return 0;
}

static int32_t sdio_register_card(struct rt_mmcsd_card *card)
{
    struct sdio_card *sc;
    struct sdio_driver *sd;
    dlist_t *l;

    sc = aos_malloc(sizeof(struct sdio_card));
    if (sc == NULL)
    {
        printf("malloc sdio card failed\n");
        return -1;
    }

    sc->card = card;
    dlist_add(&sc->list, &sdio_cards);

    if (dlist_empty(&sdio_drivers))
    {
        goto out;
    }

    for (l = (&sdio_drivers)->next; l != &sdio_drivers; l = l->next)
    {
        sd = (struct sdio_driver *)dlist_entry(l, struct sdio_driver, list);
        if (sdio_match_card(card, sd->drv->id))
        {
            sd->drv->probe(card);
        }
    }

out:
    return 0;
}

static int32_t sdio_init_card(struct rt_mmcsd_host *host, uint32_t ocr)
{
    int32_t err = 0;
    int32_t i, function_num;
    uint32_t  cmd5_resp;
    struct rt_mmcsd_card *card;

    err = sdio_io_send_op_cond(host, ocr, &cmd5_resp);
    if (err)
        goto err;

    if (controller_is_spi(host)) 
    {
        err = mmcsd_spi_use_crc(host, host->spi_use_crc);
        if (err)
            goto err;
    }

    function_num = (cmd5_resp & 0x70000000) >> 28;

    card = aos_malloc(sizeof(struct rt_mmcsd_card));
    if (!card) 
    {
        printf("malloc card failed\n");
        err = -1;
        goto err;
    }
    memset(card, 0, sizeof(struct rt_mmcsd_card));

    card->card_type = CARD_TYPE_SDIO;
    card->sdio_function_num = function_num;
    card->host = host;
    host->card = card;

    card->sdio_function[0] = aos_malloc(sizeof(struct rt_sdio_function));
    if (!card->sdio_function[0])
    {
        printf("malloc sdio_func0 failed\n");
        err = -1;
        goto err1;
    }
    memset(card->sdio_function[0], 0, sizeof(struct rt_sdio_function));
    card->sdio_function[0]->card = card;
    card->sdio_function[0]->num = 0;

    if (!controller_is_spi(host)) 
    {
        err = mmcsd_get_card_addr(host, &card->rca);
        if (err)
            goto err2;

        mmcsd_set_bus_mode(host, MMCSD_BUSMODE_PUSHPULL);
    }

    if (!controller_is_spi(host)) 
    {
        err = mmcsd_select_card(card);
        if (err)
            goto err2;
    }

    err = sdio_read_cccr(card);
    if (err)
        goto err2;

    err = sdio_read_cis(card->sdio_function[0]);
    if (err)
        goto err2;

    err = sdio_set_highspeed(card);
    if (err)
        goto err2;

    if (card->flags & CARD_FLAG_HIGHSPEED) 
    {
        mmcsd_set_clock(host, 50000000);
    } 
    else 
    {
        mmcsd_set_clock(host, card->cis.max_tran_speed);
    }

    err = sdio_set_bus_wide(card);
    if (err)
        goto err2;

    for (i = 1; i < function_num + 1; i++) 
    {
        err = sdio_initialize_function(card, i);
        if (err)
            goto err3;
    }


    /* register sdio card */
    err = sdio_register_card(card);
    if (err)
    {
        goto err3;
    }

    return 0;

err3:
    if (host->card)
    {
        for (i = 1; i < host->card->sdio_function_num + 1; i++)
        {
            if (host->card->sdio_function[i])
            {
                sdio_free_cis(host->card->sdio_function[i]);
                aos_free(host->card->sdio_function[i]);
                host->card->sdio_function[i] = NULL;
                aos_free(host->card);
                host->card = NULL;
            }
        }
    }
err2:
    if (host->card && host->card->sdio_function[0])
    {
        sdio_free_cis(host->card->sdio_function[0]);
        aos_free(host->card->sdio_function[0]);
        host->card->sdio_function[0] = NULL;
    }
err1:
    if (host->card)
    {
        aos_free(host->card);
    }
err:
    printf("error %ld while initialising SDIO card\n", err);
    
    return err;
}

int32_t init_sdio(struct rt_mmcsd_host *host, uint32_t ocr)
{
    int32_t err;
    uint32_t  current_ocr;

    assert(host != NULL);

    if (ocr & 0x7F) 
    {
        printf("Card ocr below the defined voltage rang.\n");
        ocr &= ~0x7F;
    }

    if (ocr & VDD_165_195) 
    {
        printf("Can't support the low voltage SDIO card.\n");
        ocr &= ~VDD_165_195;
    }

    current_ocr = mmcsd_select_voltage(host, ocr);

    if (!current_ocr) 
    {
        err = -1;
        goto err;
    }

    err = sdio_init_card(host, current_ocr);
    if (err)
        goto remove_card;

    return 0;

remove_card:
    aos_free(host->card);
    host->card = NULL;
err:

    printf("init SDIO card failed\n");

    return err;
}

static void sdio_irq_thread(void *param)
{
    int32_t i, ret;
    uint8_t pending;
    struct rt_mmcsd_card *card;
    struct rt_mmcsd_host *host = (struct rt_mmcsd_host *)param;
    assert(host != NULL);
    card = host->card;
    assert(card != NULL);

    while (1) 
    {
        if (krhino_sem_take(host->sdio_irq_sem, RHINO_WAIT_FOREVER) == 0)
        {
            mmcsd_host_lock(host);
            pending = sdio_io_readb(host->card->sdio_function[0], 
                        SDIO_REG_CCCR_INT_PEND, &ret);
            if (ret) 
            {
                mmcsd_dbg("error %d reading SDIO_REG_CCCR_INT_PEND\n", ret);
                goto out;
            }

            for (i = 1; i <= 7; i++) 
            {
                if (pending & (1 << i)) 
                {
                    struct rt_sdio_function *func = card->sdio_function[i];
                    if (!func) 
                    {
                        mmcsd_dbg("pending IRQ for "
                            "non-existant function %d\n", func->num);
                        goto out;
                    } 
                    else if (func->irq_handler) 
                    {
                        func->irq_handler(func);
                    } 
                    else 
                    {
                        mmcsd_dbg("pending IRQ with no register handler\n");
                        goto out;
                    }
                }
            }

        out:
            mmcsd_host_unlock(host);
            if (host->flags & MMCSD_SUP_SDIO_IRQ)
                host->ops->enable_sdio_irq(host, 1);
            continue;
        }
    }
}

static int32_t sdio_irq_thread_create(struct rt_mmcsd_card *card)
{
    struct rt_mmcsd_host *host = card->host;

    /* init semaphore and create sdio irq processing thread */
    if (!host->sdio_irq_num)
    {
        host->sdio_irq_num++;
		krhino_sem_dyn_create(&host->sdio_irq_sem, "sdio_irq", 0);
        assert(host->sdio_irq_sem != NULL);
		
    	krhino_task_dyn_create(&host->sdio_irq_thread, "sdio_irq", host, RT_SDIO_THREAD_PRIORITY, 0, RT_SDIO_STACK_SIZE, sdio_irq_thread, 1);
        assert(host->sdio_irq_thread != NULL);

    }

    return 0;
}

static int32_t sdio_irq_thread_delete(struct rt_mmcsd_card *card)
{
    struct rt_mmcsd_host *host = card->host;

    assert(host->sdio_irq_num > 0);

    host->sdio_irq_num--;
    if (!host->sdio_irq_num) 
    {
        if (host->flags & MMCSD_SUP_SDIO_IRQ)
            host->ops->enable_sdio_irq(host, 0);
        rt_sem_delete(host->sdio_irq_sem);
        host->sdio_irq_sem = NULL;
        rt_thread_delete(host->sdio_irq_thread);
        host->sdio_irq_thread = NULL;
    }

    return 0;
}

int32_t sdio_attach_irq(struct rt_sdio_function *func,
                           rt_sdio_irq_handler_t   *handler)
{
    int32_t ret;
    uint8_t reg;
    struct rt_sdio_function *func0;

    assert(func != NULL);
    assert(func->card != NULL);

    func0 = func->card->sdio_function[0];

    mmcsd_dbg("SDIO: enabling IRQ for function %d\n", func->num);

    if (func->irq_handler) 
    {
        mmcsd_dbg("SDIO: IRQ for already in use.\n");

        return -1;
    }

    reg = sdio_io_readb(func0, SDIO_REG_CCCR_INT_EN, &ret);
    if (ret)
        return ret;

    reg |= 1 << func->num;

    reg |= 1; /* Master interrupt enable */

    ret = sdio_io_writeb(func0, SDIO_REG_CCCR_INT_EN, reg);
    if (ret)
        return ret;

    func->irq_handler = handler;

    ret = sdio_irq_thread_create(func->card);
    if (ret)
        func->irq_handler = NULL;

    return ret;
}

int32_t sdio_detach_irq(struct rt_sdio_function *func)
{
    int32_t ret;
    uint8_t reg;
    struct rt_sdio_function *func0;

    assert(func != NULL);
    assert(func->card != NULL);

    func0 = func->card->sdio_function[0];

    mmcsd_dbg("SDIO: disabling IRQ for function %d\n", func->num);

    if (func->irq_handler) 
    {
        func->irq_handler = NULL;
        sdio_irq_thread_delete(func->card);
    }

    reg = sdio_io_readb(func0, SDIO_REG_CCCR_INT_EN, &ret);
    if (ret)
        return ret;

    reg &= ~(1 << func->num);

    /* Disable master interrupt with the last function interrupt */
    if (!(reg & 0xFE))
        reg = 0;

    ret = sdio_io_writeb(func0, SDIO_REG_CCCR_INT_EN, reg);
    if (ret)
        return ret;

    return 0;
}

void sdio_irq_wakeup(struct rt_mmcsd_host *host)
{
    if (host->flags & MMCSD_SUP_SDIO_IRQ)
        host->ops->enable_sdio_irq(host, 0);
    krhino_sem_give(host->sdio_irq_sem);
}

int32_t sdio_enable_func(struct rt_sdio_function *func)
{
    int32_t ret;
    uint8_t reg;
    uint32_t timeout;
    struct rt_sdio_function *func0;

    assert(func != NULL);
    assert(func->card != NULL);

    func0 = func->card->sdio_function[0];

    mmcsd_dbg("SDIO: enabling function %d\n", func->num);

    reg = sdio_io_readb(func0, SDIO_REG_CCCR_IO_EN, &ret);
    if (ret)
        goto err;

    reg |= 1 << func->num;

    ret = sdio_io_writeb(func0, SDIO_REG_CCCR_IO_EN, reg);
    if (ret)
        goto err;

    timeout = krhino_sys_time_get() + func->enable_timeout_val;

    while (1) 
    {
        reg = sdio_io_readb(func0, SDIO_REG_CCCR_IO_RDY, &ret);
        if (ret)
            goto err;
        if (reg & (1 << func->num))
            break;
        ret = -1;
        if (krhino_sys_time_get() > timeout)
            goto err;
    }

    mmcsd_dbg("SDIO: enabled function successfull\n");

    return 0;

err:
    mmcsd_dbg("SDIO: failed to enable function %d\n", func->num);
    return ret;
}

int32_t sdio_disable_func(struct rt_sdio_function *func)
{
    int32_t ret;
    uint8_t reg;
    struct rt_sdio_function *func0;

    assert(func != NULL);
    assert(func->card != NULL);

    func0 = func->card->sdio_function[0];

    mmcsd_dbg("SDIO: disabling function %d\n", func->num);

    reg =  sdio_io_readb(func0, SDIO_REG_CCCR_IO_EN, &ret);
    if (ret)
        goto err;

    reg &= ~(1 << func->num);

    ret = sdio_io_writeb(func0, SDIO_REG_CCCR_IO_EN, reg);
    if (ret)
        goto err;

    mmcsd_dbg("SDIO: disabled function successfull\n");

    return 0;

err:
    mmcsd_dbg("SDIO: failed to disable function %d\n", func->num);
    return -1;
}

void sdio_set_drvdata(struct rt_sdio_function *func, void *data)
{
    func->priv = data;
}

void* sdio_get_drvdata(struct rt_sdio_function *func)
{
    return func->priv;
}

int32_t sdio_set_block_size(struct rt_sdio_function *func,
                               uint32_t              blksize)
{
    int32_t ret;
    struct rt_sdio_function *func0 = func->card->sdio_function[0];

    if (blksize > func->card->host->max_blk_size)
        return -1;

    if (blksize == 0) 
    {
        blksize = MIN(func->max_blk_size, func->card->host->max_blk_size);
        blksize = MIN(blksize, 512u);
    }

    ret = sdio_io_writeb(func0, SDIO_REG_FBR_BASE(func->num) + SDIO_REG_FBR_BLKSIZE, 
                 blksize & 0xff);
    if (ret)
        return ret;
    ret = sdio_io_writeb(func0, SDIO_REG_FBR_BASE(func->num) + SDIO_REG_FBR_BLKSIZE + 1, 
                 (blksize >> 8) & 0xff);
    if (ret)
        return ret;
    func->cur_blk_size = blksize;

    return 0;
}

static inline int32_t sdio_match_card(struct rt_mmcsd_card           *card,
                                     const struct rt_sdio_device_id *id)
{
    uint8_t num = 1;
    
    if ((id->manufacturer != SDIO_ANY_MAN_ID) && 
        (id->manufacturer != card->cis.manufacturer))
        return 0;
    
    while (num <= card->sdio_function_num)
    {
        if ((id->product != SDIO_ANY_PROD_ID) && 
            (id->product == card->sdio_function[num]->product))
            return 1;
        num++;
    }

    return 0;
}


static struct rt_mmcsd_card *sdio_match_driver(struct rt_sdio_device_id *id)
{
    dlist_t *l;
    struct sdio_card *sc;
    struct rt_mmcsd_card *card;

    for (l = (&sdio_cards)->next; l != &sdio_cards; l = l->next)
    {
        sc = (struct sdio_card *)dlist_entry(l, struct sdio_card, list);
        card = sc->card;

        if (sdio_match_card(card, id))
        {
            return card;
        }
    }

    return NULL;
}

int32_t sdio_register_driver(struct rt_sdio_driver *driver)
{
    struct sdio_driver *sd;
    struct rt_mmcsd_card *card;

    sd = aos_malloc(sizeof(struct sdio_driver));
    if (sd == NULL)
    {
        printf("malloc sdio driver failed\n");

        return -1;
    }

    sd->drv = driver;
    dlist_add(&sd->list, &sdio_drivers);

    if (!dlist_empty(&sdio_cards))
    {
        card = sdio_match_driver(driver->id);
        if (card != NULL)
        {
            return driver->probe(card);
        }
    }

    return -1;
}

int32_t sdio_unregister_driver(struct rt_sdio_driver *driver)
{
    dlist_t *l;
    struct sdio_driver *sd = NULL;
    struct rt_mmcsd_card *card;

    for (l = (&sdio_drivers)->next; l != &sdio_drivers; l = l->next)
    {
        sd = (struct sdio_driver *)dlist_entry(l, struct sdio_driver, list);
        if (sd->drv != driver)
        {
            sd = NULL;
        }
    }

    if (sd == NULL)
    {
        printf("SDIO driver %s not register\n", driver->name);
        return -1;
    }

    if (!dlist_empty(&sdio_cards))
    {
        card = sdio_match_driver(driver->id);
        if (card != NULL)
        {
            driver->remove(card);
            dlist_del(&sd->list);
            aos_free(sd);
        }
    }

    return 0;
}

void rt_sdio_init(void)
{
    dlist_init(&sdio_cards);
    dlist_init(&sdio_drivers);
}

