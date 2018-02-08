/**
  * @file  hal_dmic.h
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

#ifndef _DRIVER_CHIP_HAL_DMIC_H_
#define _DRIVER_CHIP_HAL_DMIC_H_

#include <stdbool.h>
#include "driver/chip/hal_def.h"
#include "driver/chip/hal_gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	__IO uint32_t DMIC_EN;               /* DMIC enable control register      Address offset: 0x0000  */
	__IO uint32_t DMIC_SR;               /* DMIC sample rate register         Adrress offset: 0x0004  */
	__IO uint32_t DMIC_CTR;              /* DMIC control register           Address offset: 0x0008  */
	__IO uint32_t RESERVED0;
	__IO uint32_t DMIC_DATA;             /* DMIC data register              Address offset: 0x0010  */
	__IO uint32_t DMIC_INTC;             /* DMIC interrupt control register       Address offset: 0x0014  */
	__IO uint32_t DMIC_INTS;             /* DMIC interrupt status register        Address offset: 0x0018  */
	__IO uint32_t FIFO_CTR;              /* DMIC FIFO control register          Address offset: 0x001C  */
	__I uint32_t  FIFO_STA;              /* DMIC FIFO status register         Address offset: 0x0020  */
	__IO uint32_t CH_NUM;                /* DMIC channel numbers register       Address offset: 0x0024  */
	__IO uint32_t CH_MAP;                /* DMIC channel mapping register       Address offset: 0x0028  */
	__IO uint32_t DMIC_CNT;              /* DMIC counter register           Address offset: 0x002C  */
	__IO uint32_t DATA0_DATA1_VOL_CTR;   /* DATA0 and DATA1 volume control register   Address offset: 0x0030  */
	__IO uint32_t DATA2_DATA3_VOL_CTR;   /* High pass filter enable control register  Address offset: 0x0034  */
	__IO uint32_t HPF_EN_CTR;            /**/
	__IO uint32_t HPF_COEF_REG;          /* High pass filter coef register        Address offset: 0x003C  */
	__IO uint32_t HPF_GAIN_REG;          /* High pass filter gain register        Address offset: 0x0040  */
} DMIC_T;

#define DMIC                                  ((DMIC_T *)DMIC_BASE)

/*
 * Bits definition for DMIC_ECR register(0x0000)
 */
#define DMIC_ECR_DATA0_CHL_EN_BIT                         HAL_BIT(0)
#define DMIC_ECR_DATA0_CHR_EN_BIT                         HAL_BIT(1)
#define DMIC_ECR_DATA1_CHL_EN_BIT                         HAL_BIT(2)
#define DMIC_ECR_DATA1_CHR_EN_BIT                         HAL_BIT(3)
#define DMIC_ECR_DATA2_CHL_EN_BIT                         HAL_BIT(4)
#define DMIC_ECR_DATA2_CHR_EN_BIT                         HAL_BIT(5)
#define DMIC_ECR_DATA3_CHL_EN_BIT                         HAL_BIT(6)
#define DMIC_ECR_DATA3_CHR_EN_BIT                         HAL_BIT(7)
#define DMIC_ECR_GLOBE_EN                                 HAL_BIT(8)

/*
 * Bits definition for DMIC_SRR register (0x0004)
 */
#define DMIC_SR_SHIFT                                     (0)
#define DMIC_SR_MASK                                      (0x7U << DMIC_SR_SHIFT)

/**
  * @brief Dmic sample rate definition
  */
typedef enum {
	DMIC_SR48KHZ = (0X0U << DMIC_SR_SHIFT),
	DMIC_SR44KHZ = (0X8U << DMIC_SR_SHIFT),/*44100*/
	DMIC_SR24KHZ = (0X2U << DMIC_SR_SHIFT),
	DMIC_SR22KHZ = (0X9U << DMIC_SR_SHIFT),/*22050*/
	DMIC_SR12KHZ = (0X4U << DMIC_SR_SHIFT),
	DMIC_SR11KHZ = (0XaU << DMIC_SR_SHIFT),/*11025*/
	DMIC_SR32KHZ = (0X1U << DMIC_SR_SHIFT),
	DMIC_SR16KHZ = (0X3U << DMIC_SR_SHIFT),
	DMIC_SR8KHZ  = (0X5U << DMIC_SR_SHIFT),
} DMIC_SampleRate;

/*
 * Bits definition for DMIC_CTLR register (0X0008)
 */
#define DMIC_CTLR_OVERSAMPLE_RATE_SHIFT                   (0)
#define DMIC_CTLR_OVERSAMPLE_RATE_MASK                    (0x1U << DMIC_CTLR_OVERSAMPLE_RATE_SHIFT)
typedef enum {
	 DMIC_CTLR_OVERSAMPLE_RATE64  = (0X1U << DMIC_CTLR_OVERSAMPLE_RATE_SHIFT),
	 DMIC_CTLR_OVERSAMPLE_RATE128 = (0X0U << DMIC_CTLR_OVERSAMPLE_RATE_SHIFT),
} DMIC_OverSampleRate;

#define DMIC_CTLR_DATA0_SWAP_BIT                           HAL_BIT(4)       /* bit4, data0 left data and right data sweep enable */
#define DMIC_CTLR_DATA1_SWAP_BIT                           HAL_BIT(5)       /* bit5, data1 left data and right data sweep enable */
#define DMIC_CTLR_DATA2_SWAP_BIT                           HAL_BIT(6)       /* bit6, data2 left data and right data sweep enable */
#define DMIC_CTLR_DATA3_SWAP_BIT                           HAL_BIT(7)       /* bit7, data3 left data and right data sweep enable */
#define DMIC_CTLR_DMICDFEN_BIT                             HAL_BIT(8)       /* bit8, dmic fifo delay function for writing data after globe_en */

#define DMIC_CTLR_DMICFDT_SHIFT                            (9)
#define DMIC_CTLR_DMICFDT_MASK                             (0x3U << DMIC_CTLR_OVERSAMPLE_RATE_SHIFT)
typedef enum {
	 DMIC_CTLR_DMICFDT_5MS   = (0X0U << DMIC_CTLR_DMICFDT_SHIFT),
	 DMIC_CTLR_DMICFDT_10MS  = (0X1U << DMIC_CTLR_DMICFDT_SHIFT),
	 DMIC_CTLR_DMICFDT_200MS = (0X2U << DMIC_CTLR_DMICFDT_SHIFT),
	 DMIC_CTLR_DMICFDT_30MS  = (0X3U << DMIC_CTLR_DMICFDT_SHIFT),
} DMIC_CtrlDmicFDT;

/*
 * Bits definition for DMIC_ICR register  (0x14)
 */
#define DMIC_ICR_DATA_IRQ_EN_BIT                           HAL_BIT(0)       /* bit0, dmic fifo data available irq enable */
#define DMIC_ICR_OVERRUN_IRQ_EN_BIT                        HAL_BIT(1)       /* bit1, dmic fifo fifo overrun irq enable */
#define DMIC_ICR_FIFO_DRQ_EN_BIT                           HAL_BIT(2)       /* bit2, dmic fifo data available drq enable */

/*
 * Bits definition for DMIC_ISR register (0X18)
 */
#define DMIC_ISR_DATA_IRQ_PEND_BIT                         HAL_BIT(0)       /* bit0, dmic fifo data available pending interrupt */
#define DMIC_ISR_OVERRUN_IRQ_PEND_BIT                      HAL_BIT(1)       /* bit1, dmic fifo fifo overrun pending interrupt */

/*
 * Bits definition for DMIC_FIFOCR register(0X1C)
 */
#define DMIC_FIFOCR_TRG_LEVEL_SHIFT                        (0)
#define DMIC_FIFOCR_TRG_LEVEL_MASK                         (0xFFU << DMIC_FIFOCR_TRG_LEVEL_SHIFT)
#define DMIC_FIFOCR_TRG_LEVEL(n)                           ((n) << DMIC_FIFOCR_TRG_LEVEL_SHIFT)

#define DMIC_FIFOCR_SAMPLE_RES_SHIFT                       (8)
#define DMIC_FIFOCR_SAMPLE_RES_MASK 	                   (0x1U << DMIC_FIFOCR_SAMPLE_RES_SHIFT)

/**
  * @brief sampling accuracy
  */
typedef enum {
	 DMIC_RES16BIT = (0X0U << DMIC_FIFOCR_SAMPLE_RES_SHIFT),
	 DMIC_RES24BIT = (0X1U << DMIC_FIFOCR_SAMPLE_RES_SHIFT),
} DMIC_FifoSampleRes;

#define DMIC_FIFOCR_MODE_SHIFT                             (9)
#define DMIC_FIFOCR_MODE_MASK                              (0x1U << DMIC_FIFOCR_MODE_SHIFT)
typedef enum {
	 DMIC_FIFOCR_MODE0 = (0X0U << DMIC_FIFOCR_MODE_SHIFT),
	 DMIC_FIFOCR_MODE1 = (0X1U << DMIC_FIFOCR_MODE_SHIFT),
} DMIC_FifoMode;

#define DMIC_FIFOCR_FIFO_FLUSH_BIT                          HAL_BIT(31)      /* bit31, dmic fifo flush */

/*
 * Bits definition for DMIC_FIFOSR register (0x20)
 */
#define DMIC_FIFOSR_DATA_CNT_SHIFT                         (0)
#define DMIC_FIFOSR_DATA_CNT_MASK                          (0xFFU << DMIC_FIFOSR_DATA_CNT_SHIFT)       /* bit0:bit7, FIFO available sample world counter */

/*
 * Bits definition for DMIC_CNR register
 */
#define DMIC_CH_NUM_SHIFT                                  (0)
#define DMIC_CH_NUM_MASK                                   (0X7U << DMIC_CH_NUM_SHIFT)
typedef enum {
	DMIC_CH_NUM1 = (0X0U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM2 = (0X1U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM3 = (0X2U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM4 = (0X3U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM5 = (0X4U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM6 = (0X5U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM7 = (0X6U << DMIC_CH_NUM_SHIFT),
	DMIC_CH_NUM8 = (0X7U << DMIC_CH_NUM_SHIFT),
} DMIC_ChannelNum;

/*
 * Bits definition for DMIC_CMR register(0x28)
 */
#define DMIC_CMR_CH0_MAP_SHIFT                             (0)
#define DMIC_CMR_CH0_MAP_MASK(m)                           (0X7U << 4*m)
#define DMIC_CMR_CH0_MAP(m)                                (m << 4*m)

/*
 * Bits definition for DMIC_VCR0 register (0X0030)
 */
#define DMIC_DATA0R_VOL_SHIFT                              (0)
#define DMIC_DATA0R_VOL_MASK                               (0xFFU << DMIC_DATA0R_VOL_SHIFT)
#define DMIC_DATA0R_VOL(n)                                 (n << DMIC_DATA0R_VOL_SHIFT)

#define DMIC_DATA0L_VOL_SHIFT                              (8)
#define DMIC_DATA0L_VOL_MASK                               (0xFFU << DMIC_DATA0L_VOL_SHIFT)
#define DMIC_DATA0L_VOL(n)                                 (n << DMIC_DATA0L_VOL_SHIFT)

#define DMIC_DATA1R_VOL_SHIFT                              (16)
#define DMIC_DATA1R_VOL_MASK                               (0xFFU << DMIC_DATA1R_VOL_SHIFT)

#define DMIC_DATA1L_VOL_SHIFT                              (24)
#define DMIC_DATA1L_VOL_MASK                               (0xFFU << DMIC_DATA1L_VOL_SHIFT)

/*
 * Bits definition for DMIC_VCR1 register (0X0034)
 */
#define DMIC_DATA2R_VOL_SHIFT                               (0)
#define DMIC_DATA2R_VOL_MASK                                (0xFFU << DMIC_DATA2R_VOL_SHIFT)

#define DMIC_DATA2L_VOL_SHIFT                               (8)
#define DMIC_DATA2L_VOL_MASK                                (0xFFU << DMIC_DATA2L_VOL_SHIFT)

#define DMIC_DATA3R_VOL_SHIFT                               (16)
#define DMIC_DATA3R_VOL_MASK                                (0xFFU << DMIC_DATA3R_VOL_SHIFT)

#define DMIC_DATA3L_VOL_SHIFT                               (24)
#define DMIC_DATA3L_VOL_MASK                                (0xFFU << DMIC_DATA3L_VOL_SHIFT)

/*
 * Bits definition for DMIC_HPFECR register (0X0038)
 */
#define DMIC_HPF_DATA0_CHL_EN_BIT                           HAL_BIT(0)       /* bit0, data0 left channel enable */
#define DMIC_HPF_DATA0_CHR_EN_BIT                           HAL_BIT(1)       /* bit1, data0 right channel 1 enable */
#define DMIC_HPF_DATA1_CHL_EN_BIT                           HAL_BIT(2)       /* bit2, data1 left channel enable */
#define DMIC_HPF_DATA1_CHR_EN_BIT                           HAL_BIT(3)       /* bit3, data1 right channel 1 enable */
#define DMIC_HPF_DATA2_CHL_EN_BIT                           HAL_BIT(4)       /* bit4, data2 left channel enable */
#define DMIC_HPF_DATA2_CHR_EN_BIT                           HAL_BIT(5)       /* bit5, data2 right channel 1 enable */
#define DMIC_HPF_DATA3_CHL_EN_BIT                           HAL_BIT(6)       /* bit6, data3 left channel enable */
#define DMIC_HPF_DATA3_CHR_EN_BIT                           HAL_BIT(7)       /* bit7, data3 right channel 1 enable */

/**
  * @brief Dmic low level hardware init structure definition
  */
typedef struct {
	DMIC_CtrlDmicFDT  delayTime;    /*!< Specifies dmic fdt delay time.    */
	DMIC_FifoMode     fifoMode;     /*!< Specifies dmic fifo mode.    */
	uint32_t          triggerLevel; /*!< Specifies the threshold for dmic fifo triggers.    */
	uint32_t          volumeGain;   /*!< Specifies the dmic hardware gain.    */
	bool              hpfEnable;    /*!< Specifies whether to enable hpf function.    */
} DMIC_HWParam;

/**
  * @brief Dmic data format init structure definition
  */
typedef struct {
	DMIC_SampleRate    sampleRate;  /*!< Specifies the sampling rate of the stream.    */
	DMIC_FifoSampleRes resolution;  /*!< Specifies the sampling accuracy of the stream.    */
	uint32_t           channels;    /*!< Specifies the number of the stream.    */
	uint32_t           bufSize;     /*!< Specifies the buffer size of received data.    */
} DMIC_DataParam;

/**
  * @brief Dmic module init structure definition
  */
typedef struct {
	DMIC_HWParam  *hwParam; /*!< Dmic hardware init structure.    */
} DMIC_Param;

HAL_Status HAL_DMIC_Init(DMIC_Param *param);
void HAL_DMIC_DeInit();
HAL_Status HAL_DMIC_Open(DMIC_DataParam *param);
void HAL_DMIC_Close();
HAL_Status HAL_DMIC_Suspend();
HAL_Status HAL_DMIC_Resume();
int32_t HAL_DMIC_Read_DMA(uint8_t *buf, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_DMIC_H_ */
