/**
 * File: spi_si.h
 * Brief: bsp APIs of Sanechips spi
 *
 * Copyright (C) 2017 Sanechips Technology Co., Ltd.
 * Author: Yu Wei <yu.wei3@sanechips.com.cn>
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef _ZX29_SPI_H__
#define _ZX29_SPI_H__

#ifdef __cplusplus
extern "C" {
#endif
/*******************************************************************************
 *                             Include header files                               *
 ******************************************************************************/
#include <stdint.h>
#include <addrspace.h>

/*******************************************************************************
 *                               Macro definitions                               *
 ******************************************************************************/
#if 0
#ifdef CONFIG_HOBBIT_NBIOT
#define SPI0_REG_BASE   0xB0002000
#define SPI1_REG_BASE   0xB0012000
#elif defined CONFIG_PHOBOS_GENERAL
#define SPI0_REG_BASE   0x5000C000
#define SPI1_REG_BASE   0x6000C000
#else
#define SPI0_REG_BASE   0x50002000
#define SPI1_REG_BASE   0x50012000
#endif
#endif

#define SPI0_REG_BASE    SSP0_BASEADDR
#define SPI1_REG_BASE    SSP1_BASEADDR
#define SPI2_REG_BASE    SSP2_BASEADDR

#define LSP_CRM_REG      LSP_CRPM_BASEADDR

#define SPI0_CLK_REG    (LSP_CRM_REG + 0x14)
#define SPI1_CLK_REG    (LSP_CRM_REG + 0x18)
#define SPI2_CLK_REG    (LSP_CRM_REG + 0x48)


//#define SSP_CLK_SRC2    61440000    // 61.4mhz
#define SSP_CLK_SRC1    76800000    // 76.8mhz
#define SSP_CLK_SRC0    19200000    // 19.2mhz

#define SSP_FREQ_SRC_CNT        2

//CLK REGISTER

#define SSP_CLK_POS             0
#define SSP_CLK_MSK             0xffffffff
#define WCLK_DIV_POS            16
#define WCLK_DIV_MSK            0xf
#define WCLK_SEL_POS            12
#define WCLK_SEL_MSK            0x3
#define WCLK_GATE_POS           8
#define WCLK_GATE_MSK           0x3
#define WCLK_SW_GATE_POS        4
#define WCLK_SW_GATE_MSK        0x3
#define WCLK_RST_POS            0
#define WCLK_RST_MSK            0x3

///======================>

#define SSP_MAX_FREQ        52000000
#define SSP_MIN_FREQ        1000000


#define SSP_DMA_ENABLE      1
#define SSP_DMA_DISABLE     0

#define SSP_ENABLE          1
#define SSP_DISABLE         0


#define SSP0_DMA_TXCH    DMA_CHN_SSP0_TX
#define SSP0_DMA_RXCH    DMA_CHN_SSP0_RX

#define SSP1_DMA_TXCH    DMA_CHN_SSP1_TX
#define SSP1_DMA_RXCH    DMA_CHN_SSP1_RX

#define SSP2_DMA_TXCH    DMA_CHN_SSP2_TX
#define SSP2_DMA_RXCH    DMA_CHN_SSP2_RX

#define DEF_SSP_CFG_TX_THRES        3    // 4 bytes
#define DEF_SSP_CFG_RX_THRES        3    // 4 bytes

/********************************************************************************
 * SPI Register address offset
 *******************************************************************************/
#define VER_REG             0x00
#define COM_CTRL_REG        0x04
#define FMT_CTRL_REG        0x08
#define DR_REG              0x0c
#define FIFO_CTRL_REG       0x10
#define FIFO_SR_REG         0x14
#define INTR_EN_REG         0x18
#define INTR_SR_REG         0x1c
#define TIMING_SR_REG       0x20

/********************************************************************************
 * SPI Registers Definition
 ********************************************************************************/

/*
 * SPI Version Register - SPI_VER_REG
 */
#define VER_REG_POS                     0
#define VER_REG_MSK                     0xffffffff
#define VER_REG_X_POS                   24
#define VER_REG_X_MSK                   0xff
#define VER_REG_Y_POS                   16
#define VER_REG_Y_MSK                   0xff
/*
 * SPI Common Control Register - SPI_COM_CTRL
 */
#define COM_CTRL_POS                    0
#define COM_CTRL_MSK                    0xffffffff
#define COM_CTRL_SSPEBACK_POS           4
#define COM_CTRL_SSPEBACK_MSK           0x1
#define COM_CTRL_MS_POS                 2       /* master or slave */
#define COM_CTRL_MS_MSK                 0x1
#define COM_CTRL_SSPE_POS               1       /* spi enable bit*/
#define COM_CTRL_SSPE_MSK               0x1
#define COM_CTRL_LBM_POS                0
#define COM_CTRL_LBM_MSK                0x1
/*
 * SPI Format Control Register - SPI_FMT_CTRL
 */
#define FMT_CTRL_POS                    0
#define FMT_CTRL_MSK                    0xffffffff
#define FMT_CTRL_DSS_POS                4               /*datawidth*/
#define FMT_CTRL_DSS_MSK                0x1f
//efine FMT_CTRL_PHA_POS                3
//efine FMT_CTRL_PHA_MSK                0x1
#define FMT_CTRL_POL_POS                2               /* mode 0*/
#define FMT_CTRL_POL_MSK                0x3        //include pha..
#define FMT_CTRL_FRF_POS                0
#define FMT_CTRL_FRF_MSK                0x3

#define DR_REG_POS                      0
#define DR_REG_MSK                      0xffffffff
/*
 * SPI FIFO Control Register - SPI_FIFO_CTRL
 */
#define FIFO_CTRL_POS                   0
#define FIFO_CTRL_MSK                   0xffffffff
#define FIFO_CTRL_TX_THRES_POS          8           /*tx fifo threshold */
#define FIFO_CTRL_TX_THRES_MSK          0xf
#define FIFO_CTRL_RX_THRES_POS          4           /*rx fifo threshold */
#define FIFO_CTRL_RX_THRES_MSK          0xf
#define FIFO_CTRL_TX_DMA_EN_POS         3
#define FIFO_CTRL_TX_DMA_EN_MSK         0x1
#define FIFO_CTRL_RX_DMA_EN_POS         2
#define FIFO_CTRL_RX_DMA_EN_MSK         0x1
/*
 * SPI FIFO Status Register - SPI_FIFO_SR
 */
#define FIFO_SR_POS                     0
#define FIFO_SR_MSK                     0xffffffff
#define FIFO_SR_TX_CNTR_POS             10          /* send fifo count num */
#define FIFO_SR_TX_CNTR_MSK             0x1f
#define FIFO_SR_RX_CNTR_POS             5           /* recieve fifo count num */
#define FIFO_SR_RX_CNTR_MSK             0x1f
#define FIFO_SR_BUSY_POS                4           /* fifo status busy bit */
#define FIFO_SR_BUSY_MSK                0x1
#define FIFO_SR_TX_EMPTY_POS            3
#define FIFO_SR_TX_EMPTY_MSK            0x1
#define FIFO_SR_RX_FULL_POS             2
#define FIFO_SR_RX_FULL_MSK             0x1
#define FIFO_SR_TX_BEYONDL_POS          1
#define FIFO_SR_TX_BEYONDL_MSK          0x1
#define FIFO_SR_RX_BEYONDL_POS          0
#define FIFO_SR_RX_BEYONDL_MSK          0x1
/*
 * SPI Interrupt Enable Register - SPI_INTR_EN
 */
#define INTR_EN_POS                     0
#define INTR_EN_MSK                     0xffffffff
#define INTR_EN_MST_EOT_IE_POS          6
#define INTR_EN_MST_EOT_IE_MSK          0x1
#define INTR_EN_TX_THRES_IE_POS         5
#define INTR_EN_TX_THRES_IE_MSK         0x1
#define INTR_EN_RX_THRES_IE_POS         4
#define INTR_EN_RX_THRES_IE_MSK         0x1
#define INTR_EN_TX_EMPTY_IE_POS         3
#define INTR_EN_TX_EMPTY_IE_MSK         0x1
#define INTR_EN_RX_FULL_IE_POS          2
#define INTR_EN_RX_FULL_IE_MSK          0x1
#define INTR_EN_TX_UNDERRUN_IE_POS      1
#define INTR_EN_TX_UNDERRUN_IE_MSK      0x1
#define INTR_EN_RX_OVERRUN_IE_POS       0
#define INTR_EN_RX_OVERRUN_IE_MSK       0x1
/*
 * SPI Interrupt Status Register OR Interrupt Clear Register - SPI_INTR_SR_SCLR
 */

#define INTR_SR_POS                     0
#define INTR_SR_MSK                     0xffffffff
#define INTR_SR_MST_EOT_INTR_POS        6
#define INTR_SR_MST_EOT_INTR_MSK        0x1
#define INTR_SR_TX_THRES_INTR_POS       5
#define INTR_SR_TX_THRES_INTR_MSK       0x1
#define INTR_SR_RX_THRES_INTR_POS       4
#define INTR_SR_RX_THRES_INTR_MSK       0x1
#define INTR_SR_TX_EMPTY_INTR_POS       3
#define INTR_SR_TX_EMPTY_INTR_MSK       0x1
#define INTR_SR_RX_FULL_INTR_POS        2
#define INTR_SR_RX_FULL_INTR_MSK        0x1
#define INTR_SR_TX_UNDERRUN_INTR_POS    1
#define INTR_SR_TX_UNDERRUN_INTR_MSK    0x1
#define INTR_SR_RX_OVERRUN_INTR_POS     0
#define INTR_SR_RX_OVERRUN_INTR_MSK     0x1

#define TIMING_SR_POS                   0
#define TIMING_SR_MSK                   0xffffffff
#define TIMING_T_CS_DESEL_POS           0
#define TIMING_T_CS_DESEL_MSK           0xf



#define  _POS(field)    (field##_POS)
#define  _MSK(field)    (field##_MSK)

#define SET_FIELD(field,val)        ((val&_MSK(field))<<_POS(field))

#define GET_REG_VALUE(reg)          (*(volatile uint32_t *)(reg))

#define GET_REG_FIELD(reg,field)    ((GET_REG_VALUE(reg)>>_POS(field))&_MSK(field))



#define READ_REG32(reg)                 *((volatile uint32_t*)(reg))/*(*(volatile UINT32 *)(reg))*/

#define WRITE_REG32(reg,val)            *((volatile uint32_t*)(reg))=val/*((*(volatile UINT32*)(reg)) = (val))*/

#define SET_REG_FIELD(reg,field,val)    \
    do { \
        *(volatile uint32_t*)(reg) =((GET_REG_VALUE(reg)&(~( _MSK(field)<<_POS(field))))|((val&_MSK(field))<<_POS(field)));\
    } while (0)

/**(volatile uint32_t*)(reg) =((GET_REG_VALUE(reg)&(~( _MSK(field)<<_POS(field))))|((val&_MSK(field))<<_POS(field)));*/\

#define SET_REG_VALUE(reg,val)    \
    do { \
        *(volatile uint32_t*)(reg) = (val);            \
    } while (0)

typedef enum {
    SPI_DATASIZE4 = 3,
    SPI_DATASIZE5,
    SPI_DATASIZE6,
    SPI_DATASIZE7,
    SPI_DATASIZE8,
    SPI_DATASIZE9,
    SPI_DATASIZE10,
    SPI_DATASIZE11,
    SPI_DATASIZE12,
    SPI_DATASIZE13,
    SPI_DATASIZE14,
    SPI_DATASIZE15,
    SPI_DATASIZE16,
    SPI_DATASIZE32 = 31,
} spi_datawidth_e;

//#define SPI_DMA_ENABLE 1

#ifdef __cplusplus
}
#endif

#endif /* __HOBBIT_SPI_H__ */

