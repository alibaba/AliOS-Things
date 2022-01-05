/*
 * Copyright (c) 2021 Bestechnic (Shanghai) Co., Ltd. All rights reserved.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
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

