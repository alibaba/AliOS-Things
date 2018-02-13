/**
  * @file  hal_dmic.c
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

#include "driver/chip/hal_dma.h"
#include "driver/chip/hal_dmic.h"
#include "sys/xr_debug.h"
#include "hal_base.h"
#include "sys/io.h"
#include "pm/pm.h"

#define DMIC_DBG_ON                0

#if (DMIC_DBG_ON == 1)
#define DMIC_DEBUG(fmt, arg...)    HAL_LOG(DMIC_DBG_ON, "[DMIC] "fmt, ##arg)
#else
#define DMIC_DEBUG(fmt, arg...)
#endif
#define DMIC_ERROR(fmt, arg...)    HAL_LOG(1, "[DMIC] "fmt, ##arg)

typedef struct {
        bool               isHwInit;
        bool               isInitiate;
        bool               isSemaphore;
        volatile bool      isRunning;
        DMA_Channel        DMAChan;
        DMIC_HWParam       *hwParam;
        DMIC_DataParam     dataParam;
        uint8_t            *usrBuf;
        uint32_t           length;
        HAL_Semaphore      rxReady;
        volatile uint32_t  halfCounter;
        volatile uint32_t  endCounter;
        uint8_t            *lastReadPointer;
        uint8_t            *dmaPointer;
        HAL_Mutex          devTriggerLock;
        uint32_t           audioPllParam;
        uint32_t           audioPllPatParam;
} DMIC_Private;

typedef enum {
        DMIC_PLL_24M = 0U,
        DMIC_PLL_22M = 1U,
} DMIC_PLLMode;

typedef struct {
        uint32_t hosc;
        uint32_t audio;
        uint32_t pllParam;
        uint32_t pllPatParam;
} HOSC_DMIC_Type;

#define DMIC_MEMCPY                           memcpy
#define DMIC_MALLOC                           malloc
#define DMIC_FREE                             free
#define DMIC_MEMSET                           memset

#define DMIC_OVERRUN_THRESHOLD              3

#ifdef RESERVERD_MEMORY_FOR_DMIC
static uint8_t DMIC_BUF[DMIC_BUF_LENGTH];
#endif
DMIC_Private gDMICPrivate;
static uint32_t DMIC_BUF_LENGTH = 0;

/* default hardware configuration */
static DMIC_HWParam gHwParam = {
        DMIC_CTLR_DMICFDT_5MS,
        DMIC_FIFOCR_MODE1,
        0x40,
        0xB0,
        1,
};

static const HOSC_DMIC_Type dmic_hosc_aud_type[] = {
        {HOSC_CLOCK_26M, DMIC_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC26M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC26M},
        {HOSC_CLOCK_26M, DMIC_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC26M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC26M},
        {HOSC_CLOCK_24M, DMIC_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC24M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC24M},
        {HOSC_CLOCK_24M, DMIC_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC24M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC24M},
        {HOSC_CLOCK_40M, DMIC_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC40M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC40M},
        {HOSC_CLOCK_40M, DMIC_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC40M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC40M},
        {HOSC_CLOCK_52M, DMIC_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC52M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC52M},
        {HOSC_CLOCK_52M, DMIC_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC52M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC52M},
};

static uint32_t DMIC_PLLAUDIO_Update(DMIC_PLLMode pll)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;

        if (pll != DMIC_PLL_24M &&  pll != DMIC_PLL_22M)
                return -1;

        uint32_t hoscClock = HAL_PRCM_GetHFClock();

        int i = 0;
        for (i = 0; i < ARRAY_SIZE(dmic_hosc_aud_type); i++) {
                if ((dmic_hosc_aud_type[i].hosc == hoscClock) && (dmic_hosc_aud_type[i].audio == pll)) {
                        dmicPrivate->audioPllParam = dmic_hosc_aud_type[i].pllParam;
                        dmicPrivate->audioPllPatParam = dmic_hosc_aud_type[i].pllPatParam;
                        break;
                }
        }
        if (i == ARRAY_SIZE(dmic_hosc_aud_type)) {
                DMIC_ERROR("Update audio pll failed....\n");
                return -1;
        }
        return 0;
}

static void HAL_DMIC_Trigger(bool enable);

static int DMIC_DMA_BUFFER_CHECK_Threshold()
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        if (dmicPrivate->halfCounter >= DMIC_OVERRUN_THRESHOLD ||
                dmicPrivate->endCounter >= DMIC_OVERRUN_THRESHOLD) {
                HAL_DMIC_Trigger(0);
                dmicPrivate->isRunning = 0;
                dmicPrivate->halfCounter = 0;
                dmicPrivate->endCounter = 0;
                dmicPrivate->lastReadPointer = dmicPrivate->usrBuf;
                DMIC_ERROR("Rx : overrun and stop dma rx....\n");
                return -1;
        }
        return 0;
}

/**
  * @internal
  * @brief DMA DMIC receive process half complete callback
  * @param arg: pointer to a HAL_Semaphore structure that contains
  *             semaphore.
  * @retval None
  */
static void DMIC_DMAHalfCallback(void *arg)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        dmicPrivate->halfCounter ++;
        if (DMIC_DMA_BUFFER_CHECK_Threshold() != 0)
                return;
        if (dmicPrivate->isSemaphore == true) {
                dmicPrivate->isSemaphore = false;
                HAL_SemaphoreRelease((HAL_Semaphore *)arg);
        }
}

/**
  * @internal
  * @brief DMA DMIC receive process complete callback
  * @param arg: pointer to a HAL_Semaphore structure
  * @retval None
  */
static void DMIC_DMAEndCallback(void *arg)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        dmicPrivate->endCounter ++;
        if (DMIC_DMA_BUFFER_CHECK_Threshold() != 0)
                return;
        if (dmicPrivate->isSemaphore == true) {
                dmicPrivate->isSemaphore = false;
                HAL_SemaphoreRelease((HAL_Semaphore *)arg);
        }
}

/**
  * @internal
  * @brief Start the DMA Transfer
  * @param chan: the specified DMA Channel.
  * @param srcAddr: The source memory Buffer address.
  * @param dstAddr: The destination memory Buffer address.
  * @param datalen: The length of data to be transferred from source to destination.
  * @retval None
  */
static void DMIC_DMAStart(DMA_Channel chan, uint32_t srcAddr, uint32_t dstAddr, uint32_t datalen)
{
        HAL_DMA_Start(chan, srcAddr, dstAddr, datalen);
}

/**
  * @internal
  * @brief Stop DMA transfer.
  * @param chan: the specified DMA Channel..
  * @retval None
  */
static void DMIC_DMAStop(DMA_Channel chan)
{
        HAL_DMA_Stop(chan);
}

/**
  * @internal
  * @brief Sets the DMA Transfer parameter.
  * @param channel: the specified DMA Channel.
  * @param dir: Data transfer direction
  * @retval none
  */
static void DMIC_DMASet(DMA_Channel channel)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        DMA_ChannelInitParam dmaParam;
        HAL_Memset(&dmaParam, 0, sizeof(dmaParam));
        dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_CIRCULAR,
                        DMA_WAIT_CYCLE_2,
                        DMA_BYTE_CNT_MODE_NORMAL,
                        DMA_DATA_WIDTH_16BIT,
                        DMA_BURST_LEN_1,
                        DMA_ADDR_MODE_INC,
                        DMA_PERIPH_SRAM,
                        DMA_DATA_WIDTH_16BIT,
                        DMA_BURST_LEN_1,
                        DMA_ADDR_MODE_FIXED,
                        DMA_PERIPH_DMIC);

        dmaParam.irqType = DMA_IRQ_TYPE_BOTH;
        dmaParam.endCallback = DMIC_DMAEndCallback;
        dmaParam.halfCallback = DMIC_DMAHalfCallback;
        dmaParam.endArg = &(dmicPrivate->rxReady);
        dmaParam.halfArg = &(dmicPrivate->rxReady);

        HAL_DMA_Init(channel, &dmaParam);
}

/**
  * @internal
  * @brief enbale/disable DMIC controller.
  * @param enable: enable or disable.
  * @retval None
  */
static void HAL_DMIC_Trigger(bool enable)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        if (enable) {
                HAL_SET_BIT(DMIC->FIFO_CTR, DMIC_FIFOCR_FIFO_FLUSH_BIT);

                if (dmicPrivate->dataParam.channels > 1) {
                        HAL_SET_BIT(DMIC->DMIC_EN, DMIC_ECR_DATA0_CHL_EN_BIT|DMIC_ECR_DATA0_CHR_EN_BIT);
                } else {
                        HAL_SET_BIT(DMIC->DMIC_EN, DMIC_ECR_DATA0_CHL_EN_BIT);
                }
                HAL_SET_BIT(DMIC->DMIC_INTC, DMIC_ICR_FIFO_DRQ_EN_BIT);
                DMIC_DMAStart(dmicPrivate->DMAChan, (uint32_t)&(DMIC->DMIC_DATA),
                             (uint32_t)dmicPrivate->usrBuf, dmicPrivate->length);
        } else {
                DMIC_DMAStop(dmicPrivate->DMAChan);
                HAL_CLR_BIT(DMIC->DMIC_INTC, DMIC_ICR_FIFO_DRQ_EN_BIT);
                HAL_CLR_BIT(DMIC->DMIC_EN, DMIC_ECR_DATA0_CHL_EN_BIT|DMIC_ECR_DATA0_CHR_EN_BIT);
        }
}

/**
  * @brief Receive an amount of data with DMA
  * @param buf: pointer to the Receive data buffer.
  * @param Size: length of buf to be Receive:
  * @retval length of receive data
  */
int32_t HAL_DMIC_Read_DMA(uint8_t *buf, uint32_t size)
{
        if (!buf || size < DMIC_BUF_LENGTH/2)
                return HAL_INVALID;

        DMIC_Private *dmicPrivate = &gDMICPrivate;
        uint8_t *readPointer = NULL;
        uint32_t toRead = DMIC_BUF_LENGTH/2;
        uint32_t readSize = 0;
        uint8_t *writeBuf = buf;
        uint8_t err_flag; /* temp solution to avoid outputing debug message when irq disabled */

        while (size) {
                if (size < toRead)
                        break;
                if (!dmicPrivate->isRunning) {
                        dmicPrivate->lastReadPointer = dmicPrivate->usrBuf;
                        DMIC_DEBUG("Rx: record start...\n");
                        dmicPrivate->isRunning = 1;
                        HAL_DMIC_Trigger(1);
                } else {
                        err_flag = 0;
                        readPointer = dmicPrivate->lastReadPointer;
                        HAL_DisableIRQ();
                        if (dmicPrivate->halfCounter && dmicPrivate->endCounter) {
                                dmicPrivate->halfCounter = 0;
                                dmicPrivate->endCounter = 0;
                                if (dmicPrivate->dmaPointer == dmicPrivate->usrBuf) {
                                        readPointer = dmicPrivate->usrBuf + DMIC_BUF_LENGTH/2;
                                } else {
                                        readPointer = dmicPrivate->usrBuf;
                                }
                                err_flag = 1;
                        } else if (dmicPrivate->halfCounter ) {
                                dmicPrivate->halfCounter --;

                        } else if (dmicPrivate->endCounter) {
                                dmicPrivate->endCounter --;
                        } else {
                                dmicPrivate->isSemaphore = true;
                                HAL_EnableIRQ();
                                HAL_SemaphoreWait(&dmicPrivate->rxReady, HAL_WAIT_FOREVER);
                                HAL_DisableIRQ();
                                if (dmicPrivate->halfCounter)
                                        dmicPrivate->halfCounter--;
                                if (dmicPrivate->endCounter)
                                        dmicPrivate->endCounter--;
                        }
                        DMIC_MEMCPY(writeBuf, readPointer, toRead);
                        HAL_EnableIRQ();
                        if (err_flag) {
                            DMIC_ERROR("overrun...\n");
                        }
                        writeBuf += toRead;
                        size -= toRead;
                        readSize += toRead;
                        dmicPrivate->lastReadPointer = readPointer +  toRead;
                        if (dmicPrivate->lastReadPointer >= dmicPrivate->usrBuf + DMIC_BUF_LENGTH)
                                dmicPrivate->lastReadPointer = dmicPrivate->usrBuf;
                }
        }
        return readSize;
}

/**
  * @internal
  * @brief DMIC peripheral Init
  * @param param: pointer to a DMIC_HWParam structure that contains
  *         some hardware configuration information
  * @retval HAL status
  */
static inline HAL_Status DMIC_HwInit(DMIC_HWParam *param)
{
        if (!param)
                return HAL_INVALID;
        /* Set delay time */
        HAL_MODIFY_REG(DMIC->DMIC_CTR, DMIC_CTLR_DMICFDT_MASK,param->delayTime);
        /*Set fifo mode*/
        HAL_MODIFY_REG(DMIC->FIFO_CTR, DMIC_FIFOCR_MODE_MASK|DMIC_FIFOCR_TRG_LEVEL_MASK,
                        param->fifoMode|DMIC_FIFOCR_TRG_LEVEL(param->triggerLevel));
        /* Set volume gain*/
        HAL_MODIFY_REG(DMIC->DATA0_DATA1_VOL_CTR, DMIC_DATA0L_VOL_MASK|DMIC_DATA0R_VOL_MASK,
                        DMIC_DATA0L_VOL(param->volumeGain)|DMIC_DATA0R_VOL(param->volumeGain));
        if (param->hpfEnable) {
                /* Set HPF*/
                HAL_SET_BIT(DMIC->HPF_EN_CTR, DMIC_HPF_DATA0_CHL_EN_BIT|DMIC_HPF_DATA0_CHR_EN_BIT);
        }
        /* Global enable */
        HAL_SET_BIT(DMIC->DMIC_EN, DMIC_ECR_GLOBE_EN);
        return HAL_OK;
}

/**
  * @internal
  * @brief DeInitializes the DMIC peripheral
  * @param param: pointer to a DMIC_HWParam structure that contains
  *         some hardware configuration information.
  * @retval HAL status
  */
static inline HAL_Status DMIC_HwDeInit(DMIC_HWParam *param)
{
        if (!param)
                return HAL_INVALID;
        if (param->hpfEnable) {
                /* close HPF*/
                HAL_CLR_BIT(DMIC->HPF_EN_CTR, DMIC_HPF_DATA0_CHL_EN_BIT|DMIC_HPF_DATA0_CHR_EN_BIT);
        }
        /* Global disable */
        HAL_CLR_BIT(DMIC->DMIC_EN, DMIC_ECR_GLOBE_EN);
        return HAL_OK;
}

/**
  * @brief Open the DMIC module according to the specified parameters
  *         in the DMIC_DataParam.
  * @param param: pointer to a DMIC_DataParam structure that contains
  *         the data format information
  * @retval HAL status
  */
HAL_Status HAL_DMIC_Open(DMIC_DataParam *param)
{
        DMIC_DEBUG("Dmic open.\n");

        if (!param)
                return HAL_INVALID;
        DMIC_Private *dmicPrivate = &gDMICPrivate;

        if (dmicPrivate->isInitiate == true) {
                DMIC_ERROR("DMIC opened already,faild...\n");
                return HAL_INVALID;
        }
        dmicPrivate->isInitiate = true;

        DMIC_MEMCPY(&(dmicPrivate->dataParam), param, sizeof(*param));

        DMIC_BUF_LENGTH = param->bufSize;
        dmicPrivate->DMAChan = DMA_CHANNEL_INVALID;
        dmicPrivate->length = DMIC_BUF_LENGTH;
#ifdef RESERVERD_MEMORY_FOR_DMIC
        dmicPrivate->usrBuf = DMIC_BUF;
#else
        dmicPrivate->usrBuf = DMIC_MALLOC(DMIC_BUF_LENGTH);
        if(dmicPrivate->usrBuf)
                DMIC_MEMSET(dmicPrivate->usrBuf,0,DMIC_BUF_LENGTH);
        else {
                DMIC_ERROR("Malloc buf(for DMA),faild...\n");
                return HAL_ERROR;
        }
#endif
        /*request DMA channel*/
        dmicPrivate->DMAChan = HAL_DMA_Request();
        if (dmicPrivate->DMAChan == DMA_CHANNEL_INVALID) {
                DMIC_ERROR("Request DMA channel(for DMIC),faild...\n");
                DMIC_FREE(dmicPrivate->usrBuf);
                return HAL_ERROR;
        }
        HAL_SemaphoreInitBinary(&dmicPrivate->rxReady);

        HAL_MODIFY_REG(DMIC->CH_NUM, DMIC_CH_NUM_MASK,(param->channels - 1));

        /* Enable chl chr(data0) */
        if (param->channels > 1) {
                HAL_SET_BIT(DMIC->DMIC_EN, DMIC_ECR_DATA0_CHL_EN_BIT|DMIC_ECR_DATA0_CHR_EN_BIT);

                HAL_MODIFY_REG(DMIC->CH_MAP, DMIC_CMR_CH0_MAP_MASK(0)| DMIC_CMR_CH0_MAP_MASK(1),
                                DMIC_CMR_CH0_MAP(0)|DMIC_CMR_CH0_MAP(1));
        } else {
                HAL_SET_BIT(DMIC->DMIC_EN, DMIC_ECR_DATA0_CHL_EN_BIT);
                HAL_MODIFY_REG(DMIC->CH_MAP, DMIC_CMR_CH0_MAP_MASK(0),DMIC_CMR_CH0_MAP(0));
        }

        switch (param->sampleRate) {
                case DMIC_SR48KHZ:
                case DMIC_SR24KHZ:
                case DMIC_SR12KHZ:
                case DMIC_SR32KHZ:
                case DMIC_SR16KHZ:
                case DMIC_SR8KHZ:
                        DMIC_PLLAUDIO_Update(DMIC_PLL_24M);
                        HAL_PRCM_SetAudioPLLParam(dmicPrivate->audioPllParam);
                        HAL_PRCM_SetAudioPLLPatternParam(dmicPrivate->audioPllPatParam);
                        break;
                case DMIC_SR44KHZ:
                case DMIC_SR22KHZ:
                case DMIC_SR11KHZ:
                        DMIC_PLLAUDIO_Update(DMIC_PLL_22M);
                        HAL_PRCM_SetAudioPLLParam(dmicPrivate->audioPllParam);
                        HAL_PRCM_SetAudioPLLPatternParam(dmicPrivate->audioPllPatParam);
                        break;
                default:
                        DMIC_ERROR("Invalued Samplerate...\n");

        }

        /* Set DMIC oversample rate*/
        switch (param->sampleRate) {
                case DMIC_SR48KHZ:
                case DMIC_SR24KHZ:
                case DMIC_SR44KHZ:
                case DMIC_SR22KHZ:
                case DMIC_SR32KHZ:
                        HAL_MODIFY_REG(DMIC->DMIC_CTR, DMIC_CTLR_OVERSAMPLE_RATE_MASK,
                                       DMIC_CTLR_OVERSAMPLE_RATE64);
                        break;
                case DMIC_SR16KHZ:
                case DMIC_SR12KHZ:
                case DMIC_SR8KHZ:
                case DMIC_SR11KHZ:
                        HAL_MODIFY_REG(DMIC->DMIC_CTR, DMIC_CTLR_OVERSAMPLE_RATE_MASK,
                                       DMIC_CTLR_OVERSAMPLE_RATE128);
                        break;
        }

        if (param->sampleRate == DMIC_SR44KHZ) {
                param->sampleRate = DMIC_SR48KHZ;
        } else if (param->sampleRate == DMIC_SR22KHZ) {
                param->sampleRate = DMIC_SR24KHZ;
        } else if (param->sampleRate == DMIC_SR11KHZ) {
                param->sampleRate = DMIC_SR12KHZ;
        }

        /* Set sample rate*/
        HAL_MODIFY_REG(DMIC->DMIC_SR, DMIC_SR_MASK,param->sampleRate);

        /* Set sample resolution */
        HAL_MODIFY_REG(DMIC->FIFO_CTR, DMIC_FIFOCR_SAMPLE_RES_MASK,param->resolution);

        /* init DMA*/
        if (dmicPrivate->DMAChan != DMA_CHANNEL_INVALID)
                DMIC_DMASet(dmicPrivate->DMAChan);

        return HAL_OK;
}

/**
  * @brief Close the DMIC module
  * @note The module is closed at the end of transaction to avoid power consumption
  * @retval none
  */
void HAL_DMIC_Close()
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        HAL_DMIC_Trigger(false);
        dmicPrivate->isRunning = false;
        dmicPrivate->isInitiate = false;

        if (dmicPrivate->DMAChan != DMA_CHANNEL_INVALID) {
                HAL_DMA_DeInit(dmicPrivate->DMAChan);
                HAL_DMA_Release(dmicPrivate->DMAChan);
                dmicPrivate->DMAChan = DMA_CHANNEL_INVALID;
        }
        DMIC_MEMSET(&(dmicPrivate->dataParam), 0, sizeof(DMIC_DataParam));
#ifndef RESERVERD_MEMORY_FOR_DMIC
        DMIC_FREE(dmicPrivate->usrBuf);
#endif
        dmicPrivate->usrBuf= NULL;
        dmicPrivate->length = 0;
        dmicPrivate->halfCounter = 0;
        dmicPrivate->endCounter = 0;
}

/**
  * @internal
  * @brief DMIC PINS Init
  * @retval HAL status
  */
static inline HAL_Status DMIC_PINS_Init()
{
        return HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_DMIC, 0), 0);
}

/**
  * @internal
  * @brief DMIC PINS DeInit
  * @retval HAL status
  */
static inline HAL_Status DMIC_PINS_DeInit()
{
        return HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_DMIC, 0), 0);
}

#ifdef CONFIG_PM
static int dmic_suspend(struct soc_device *dev, enum suspend_state_t state)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;

        switch (state) {
        case PM_MODE_SLEEP:
        case PM_MODE_STANDBY:
        case PM_MODE_HIBERNATION:
        case PM_MODE_POWEROFF:
                DMIC_HwDeInit(dmicPrivate->hwParam);
                DMIC_PINS_DeInit();
                HAL_CCM_DMIC_DisableMClock();
                HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_DMIC);
                HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_DMIC);
                HAL_PRCM_DisableAudioPLL();
                HAL_PRCM_DisableAudioPLLPattern();
                break;
        default:
                break;
        }
        return 0;
}

static int dmic_resume(struct soc_device *dev, enum suspend_state_t state)
{
        DMIC_Private *dmicPrivate = &gDMICPrivate;
        switch (state) {
        case PM_MODE_SLEEP:
        case PM_MODE_STANDBY:
        case PM_MODE_HIBERNATION:
                DMIC_PLLAUDIO_Update(DMIC_PLL_24M);
                HAL_PRCM_SetAudioPLLParam(dmicPrivate->audioPllParam);
                HAL_PRCM_SetAudioPLLPatternParam(dmicPrivate->audioPllPatParam);
                HAL_PRCM_EnableAudioPLL();
                HAL_PRCM_EnableAudioPLLPattern();
                HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_DMIC);
                HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_DMIC);

                /*init and enable clk*/
                HAL_CCM_DMIC_EnableMClock();
                /*init gpio*/
                DMIC_PINS_Init();
                DMIC_HwInit(dmicPrivate->hwParam);
                break;
        default:
                break;
        }

        return 0;
}

static struct soc_device_driver dmic_drv = {
        .name    = "DMIC",
        .suspend = dmic_suspend,
        .resume  = dmic_resume,
};

static struct soc_device dmic_dev = {
        .name   = "DMIC",
        .driver = &dmic_drv,
};

#define DMIC_DEV (&dmic_dev)
#else
#define DMIC_DEV NULL
#endif

/**
  * @brief Initializes the DMIC according to the specified parameters
  *         in the DMIC_Param.
  * @param  param: pointer to a DMIC_Param structure that contains
  *         the hw configuration information for DMIC controller
  * @retval HAL status
  */
HAL_Status HAL_DMIC_Init(DMIC_Param *param)
{
        int32_t ret = 0;
        DMIC_DEBUG("Dmic init.\n");
        if (!param)
                return HAL_INVALID;

        DMIC_Private *dmicPrivate = &gDMICPrivate;

        if (dmicPrivate->isHwInit)
                return HAL_OK;
        DMIC_MEMSET(dmicPrivate,0,sizeof(struct DMIC_Private *));
        dmicPrivate->isHwInit = true;

        if (!param->hwParam)
                dmicPrivate->hwParam = &gHwParam;
        else
                dmicPrivate->hwParam = param->hwParam;

        /*init and enable clk*/
        DMIC_PLLAUDIO_Update(DMIC_PLL_24M);
        HAL_PRCM_SetAudioPLLParam(dmicPrivate->audioPllParam);
        HAL_PRCM_EnableAudioPLL();
        HAL_PRCM_SetAudioPLLPatternParam(dmicPrivate->audioPllPatParam);
        HAL_PRCM_EnableAudioPLLPattern();
        HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_DMIC);
        HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_DMIC);
        /*init and enable clk*/
        HAL_CCM_DMIC_EnableMClock();

        /*init gpio*/
        DMIC_PINS_Init();
#ifdef CONFIG_PM
        pm_register_ops(DMIC_DEV);
#endif
        ret = DMIC_HwInit(dmicPrivate->hwParam);
        return ret;
}

/**
  * @brief DeInitializes the DMIC module
  * @retval
  */
void HAL_DMIC_DeInit()
{
        DMIC_DEBUG("Dmic deinit.\n");

        DMIC_Private *dmicPrivate = &gDMICPrivate;

        DMIC_HwDeInit(dmicPrivate->hwParam);
        DMIC_PINS_DeInit();

        HAL_CCM_DMIC_DisableMClock();
        HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_DMIC);
        HAL_PRCM_DisableAudioPLLPattern();
        HAL_PRCM_DisableAudioPLL();

        DMIC_MEMSET(dmicPrivate,0,sizeof(struct DMIC_Private *));
}
