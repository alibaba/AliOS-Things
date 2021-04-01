/*
 * Copyright (C) 2015-2021 Alibaba Group Holding Limited
 */
#ifndef __HAL_XDMACFG_BEST2001_H__
#define __HAL_XDMACFG_BEST2001_H__

#define DSPXDMA_PERIPH_NUM                      2
#define DSPXDMA_CHAN_NUM                        16
#define DSPXDMA_CHAN_START                      (0)

static const uint32_t bes2001_dspxdma_fifo_addr[DSPXDMA_PERIPH_NUM] = {
    FLASH_CTRL_BASE + 0x008, // FLASH RX
    FLASH_CTRL_BASE + 0x008, // FLASH TX
};

static const enum HAL_XDMA_PERIPH_T bes2001_dspxdma_fifo_periph[DSPXDMA_PERIPH_NUM] = {
    HAL_DSPXDMA_MEM,
};

#define dspxdma_fifo_addr   bes2001_dspxdma_fifo_addr
#define dspxdma_fifo_periph bes2001_dspxdma_fifo_periph

#endif

