/***************************************************************************
 *
 * Copyright 2015-2019 BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * No part of this work may be used or reproduced in any form or by any
 * means, or stored in a database or retrieval system, without prior written
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/
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

