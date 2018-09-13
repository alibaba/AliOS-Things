/**
  * @file  hal_dma.h
  * @author  XRADIO IOT WLAN Team
  */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DRIVER_CHIP_HAL_DMA_H_
#define _DRIVER_CHIP_HAL_DMA_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Options of supporting DMA half transfer complete IRQ */
#define HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT   1

/**
 * @brief DMA channel definition
 */
typedef enum {
    DMA_CHANNEL_0       = 0U,
    DMA_CHANNEL_1       = 1U,
    DMA_CHANNEL_2       = 2U,
    DMA_CHANNEL_3       = 3U,
    DMA_CHANNEL_4       = 4U,
    DMA_CHANNEL_5       = 5U,
    DMA_CHANNEL_6       = 6U,
    DMA_CHANNEL_7       = 7U,
    DMA_CHANNEL_NUM     = 8U,
    DMA_CHANNEL_INVALID = DMA_CHANNEL_NUM
} DMA_Channel;

/**
 * @brief DMA channel register block structure
 */
typedef struct
{
    __IO uint32_t CTRL;         /* offset: 0x00, DMA channel configuration register */
    __IO uint32_t SRC_ADDR;     /* offset: 0x04, DMA channel source address register */
    __IO uint32_t DST_ADDR;     /* offset: 0x08, DMA channel destination address register */
    __IO uint32_t BYTE_CNT;     /* offset: 0x0C, DMA channel byte counter register */
         uint32_t RESERVED0[4];
} DMA_CHANNEL_T;

/**
 * @brief DMA register block structure
 */
typedef struct
{
    __IO uint32_t IRQ_EN;                   /* offset: 0x00, DMA IRQ enable register */
    __IO uint32_t IRQ_STATUS;               /* offset: 0x04, DMA IRQ status register */
         uint32_t RESERVED0[62];
    DMA_CHANNEL_T CHANNEL[DMA_CHANNEL_NUM]; /* offset: 0x100, DMA channels register blocks */
} DMA_T;

#define DMA ((DMA_T *)DMA_BASE)  /* address: 0x40001000 */

/*
 * Bit field definition of
 *     - DMA->IRQ_EN
 *     - DMA->IRQ_STATUS
 */
#define DMA_IRQ_TYPE_VMASK  0x3U
typedef enum {
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
    DMA_IRQ_TYPE_HALF       = HAL_BIT(0),
#endif
    DMA_IRQ_TYPE_END        = HAL_BIT(1),
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
    DMA_IRQ_TYPE_BOTH       = HAL_BIT(0) | HAL_BIT(1),
#endif
} DMA_IRQType;

/*
 * Bit field definition of DMA->CHANNEL[x].CTRL
 */
#define DMA_START_BIT           HAL_BIT(31) /* R/W */
#define DMA_BUSY_BIT            HAL_BIT(30) /* R */

#define DMA_WORK_MODE_SHIFT     29  /* R/W */
#define DMA_WORK_MODE_VMASK     0x1U
typedef enum {
    DMA_WORK_MODE_SINGLE        = 0U,	/* one-shot DMA transfer */
    DMA_WORK_MODE_CIRCULAR      = 1U	/* repeat DMA transfer */
} DMA_WorkMode;

#define DMA_WAIT_CYCLE_SHIFT    26  /* R/W */
#define DMA_WAIT_CYCLE_VMASK    0x7U
typedef enum {
    DMA_WAIT_CYCLE_1            = 0U,
    DMA_WAIT_CYCLE_2            = 1U,
    DMA_WAIT_CYCLE_4            = 2U,
    DMA_WAIT_CYCLE_8            = 3U,
    DMA_WAIT_CYCLE_16           = 4U,
    DMA_WAIT_CYCLE_32           = 5U,
    DMA_WAIT_CYCLE_64           = 6U,
    DMA_WAIT_CYCLE_128          = 7U
} DMA_WaitCycle; /* waiting cycles between two burst transaction */

#define DMA_DATA_WIDTH_VMASK    0x3U
typedef enum {
    DMA_DATA_WIDTH_8BIT         = 0U,
    DMA_DATA_WIDTH_16BIT        = 1U,
    DMA_DATA_WIDTH_32BIT        = 2U
} DMA_DataWidth;

#define DMA_BURST_LEN_VMASK     0x1U
typedef enum {
    DMA_BURST_LEN_1             = 0U,
    DMA_BURST_LEN_4             = 1U
} DMA_BurstLen;

#define DMA_ADDR_MODE_VMASK     0x1U
typedef enum {
    DMA_ADDR_MODE_INC           = 0U,   /* address is increased when DMA transferring */
    DMA_ADDR_MODE_FIXED         = 1U    /* address is not changed when DMA transferring */
} DMA_AddrMode;

#define DMA_PERIPH_VMASK        0x1F
typedef enum {
    DMA_PERIPH_SRAM             = 0U,
    DMA_PERIPH_SPI0             = 1U,
    DMA_PERIPH_SPI1             = 2U,
    DMA_PERIPH_UART0            = 3U,
    DMA_PERIPH_UART1            = 4U,
    DMA_PERIPH_CE               = 5U,
    DMA_PERIPH_DAUDIO           = 6U,
    DMA_PERIPH_FLASHC           = 7U,
    DMA_PERIPH_DMIC             = 8U
} DMA_Periph;

#define DMA_BYTE_CNT_MODE_SHIFT 15  /* R/W */
#define DMA_BYTE_CNT_MODE_VMASK 0x1U
typedef enum {
    DMA_BYTE_CNT_MODE_NORMAL    = 0U,	/* DMA->CHANNEL[x].BYTE_CNT is the data length of the DAM transfer */
    DMA_BYTE_CNT_MODE_REMAIN    = 1U	/* DMA->CHANNEL[x].BYTE_CNT is the length of the remaining data not transferred */
} DMA_ByteCntMode;

#define DMA_DST_DATA_WIDTH_SHIFT    24  /* R/W */
#define DMA_DST_BURST_LEN_SHIFT     23  /* R/W */
#define DMA_DST_ADDR_MODE_SHIFT     21  /* R/W */
#define DMA_DST_PERIPH_SHIFT        16  /* R/W */
#define DMA_SRC_DATA_WIDTH_SHIFT    8   /* R/W */
#define DMA_SRC_BURST_LEN_SHIFT     7   /* R/W */
#define DMA_SRC_ADDR_MODE_SHIFT     5   /* R/W */
#define DMA_SRC_PERIPH_SHIFT        0

/* DMA->CHANNEL[x].SRC_ADDR */

/* DMA->CHANNEL[x].DST_ADDR */

/* DMA->CHANNEL[x].BYTE_CNT */
#define DMA_BYTE_CNT_VMASK  0x3FFFF

/* Maximum data length of one DMA transfer */
#define DMA_DATA_MAX_LEN    (128 * 1024U)

/******************************************************************************/

/** @brief Type define of DMA IRQ callback function */
typedef void (*DMA_IRQCallback)(void *arg);

/**
 * @brief DMA channel initialization parameters
 */
typedef struct {
    uint32_t            cfg;            /* created by HAL_DMA_MakeInitCfg() */

    DMA_IRQType         irqType;        /* DMA IRQ type to be enabled */
    DMA_IRQCallback     endCallback;    /* DMA transfer complete callback fucntion */
    void               *endArg;         /* argument of DMA transfer complete callback fucntion */
#if HAL_DMA_TRANSFER_HALF_IRQ_SUPPORT
    DMA_IRQCallback     halfCallback;   /* DMA half transfer complete callback fucntion */
    void               *halfArg;        /* argument of DMA half transfer complete callback fucntion */
#endif
} DMA_ChannelInitParam;

/**
 * @brief Make configuration value for DMA_ChannelInitParam::cfg
 * @param[in] workMode DMA transfer working mode
 * @param[in] waitCycle Waiting cycles between two DMA burst transaction
 * @param[in] byteCntMode DMA byte count mode
 *     @arg DMA_BYTE_CNT_MODE_NORMAL In normal mode, DMA->CHANNEL[x].BYTE_CNT is
 *                                   not changed in the whole DMA transfer, and
 *                                   its value is the data length of the DAM
 *                                   transfer.
 *     @arg DMA_BYTE_CNT_MODE_REMAIN In remaining mode, DMA->CHANNEL[x].BYTE_CNT
 *                                   is decreased when DMA transferring, and its
 *                                   value is the length of the remaining data
 *                                   not transferred.
 * @param[in] dstDataWidth Destination DMA transaction data width
 * @param[in] dstBurstLen Destination DMA transaction burst length
 * @param[in] dstAddrMode Destination DMA address mode
 * @param[in] dstPeriph Destination DMA peripheral type
 * @param[in] srcDataWidth Source DMA transaction data width
 * @param[in] srcBurstLen Source DMA transaction burst length
 * @param[in] srcAddrMode Source DMA address mode
 * @param[in] srcPeriph Source DMA peripheral type
 * @return configuration value for DMA_ChannelInitParam::cfg
 *
 * @note
 *     - A DMA transaction is not interruptible
 *     - A DMA transaction data length = data width * burst length
 */
__STATIC_INLINE uint32_t HAL_DMA_MakeChannelInitCfg(DMA_WorkMode workMode,
                                                    DMA_WaitCycle waitCycle,
                                                    DMA_ByteCntMode byteCntMode,
                                                    DMA_DataWidth dstDataWidth,
                                                    DMA_BurstLen dstBurstLen,
                                                    DMA_AddrMode dstAddrMode,
                                                    DMA_Periph dstPeriph,
                                                    DMA_DataWidth srcDataWidth,
                                                    DMA_BurstLen srcBurstLen,
                                                    DMA_AddrMode srcAddrMode,
                                                    DMA_Periph srcPeriph)
{
    return ((((uint32_t)workMode     & DMA_WORK_MODE_VMASK)     << DMA_WORK_MODE_SHIFT)      |
            (((uint32_t)waitCycle    & DMA_WAIT_CYCLE_VMASK)    << DMA_WAIT_CYCLE_SHIFT)     |
            (((uint32_t)byteCntMode  & DMA_BYTE_CNT_MODE_VMASK) << DMA_BYTE_CNT_MODE_SHIFT)  |
            (((uint32_t)dstDataWidth & DMA_DATA_WIDTH_VMASK)    << DMA_DST_DATA_WIDTH_SHIFT) |
            (((uint32_t)dstBurstLen  & DMA_BURST_LEN_VMASK)     << DMA_DST_BURST_LEN_SHIFT)  |
            (((uint32_t)dstAddrMode  & DMA_ADDR_MODE_VMASK)     << DMA_DST_ADDR_MODE_SHIFT)  |
            (((uint32_t)dstPeriph    & DMA_PERIPH_VMASK)        << DMA_DST_PERIPH_SHIFT)     |
            (((uint32_t)srcDataWidth & DMA_DATA_WIDTH_VMASK)    << DMA_SRC_DATA_WIDTH_SHIFT) |
            (((uint32_t)srcBurstLen  & DMA_BURST_LEN_VMASK)     << DMA_SRC_BURST_LEN_SHIFT)  |
            (((uint32_t)srcAddrMode  & DMA_ADDR_MODE_VMASK)     << DMA_SRC_ADDR_MODE_SHIFT)  |
            (((uint32_t)srcPeriph    & DMA_PERIPH_VMASK)        << DMA_SRC_PERIPH_SHIFT));
}

DMA_Channel HAL_DMA_Request(void);
void HAL_DMA_Release(DMA_Channel chan);

void HAL_DMA_Init(DMA_Channel chan, const DMA_ChannelInitParam *param);
void HAL_DMA_DeInit(DMA_Channel chan);

void HAL_DMA_Start(DMA_Channel chan, uint32_t srcAddr, uint32_t dstAddr, uint32_t datalen);
void HAL_DMA_Stop(DMA_Channel chan);

int HAL_DMA_IsBusy(DMA_Channel chan);
uint32_t HAL_DMA_GetByteCount(DMA_Channel chan);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_DMA_H_ */
