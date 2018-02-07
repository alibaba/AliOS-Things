/**
  * @file  hal_i2s.c
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
#include "driver/chip/hal_i2s.h"
#include "hal_base.h"
#include "sys/io.h"
#include "pm/pm.h"

#define I2D_DBG_ON                0

#if (I2D_DBG_ON == 1)
#define I2S_DEBUG(fmt, arg...)    HAL_LOG(I2D_DBG_ON, "[I2S] "fmt, ##arg)
#else
#define I2S_DEBUG(fmt, arg...)
#endif
#define I2S_ERROR(fmt, arg...)    HAL_LOG(1, "[I2S] "fmt, ##arg)

typedef struct {
        volatile bool               isHwInit;
        volatile bool               txRunning;
        volatile bool               rxRunning;

        uint8_t                     *txBuf;
        uint8_t                     *rxBuf;
        uint8_t                     *readPointer;
        uint8_t                     *writePointer;
        uint32_t                    rxLength;
        uint32_t                    txLength;

        DMA_Channel                 txDMAChan;
        DMA_Channel                 rxDMAChan;
        I2S_HWParam                 *hwParam;
        I2S_DataParam               pdataParam;
        I2S_DataParam               cdataParam;
        volatile uint32_t           txHalfCallCount;
        volatile uint32_t           rxHalfCallCount;
        volatile uint32_t           txEndCallCount;
        volatile uint32_t           rxEndCallCount;
        uint8_t                     *txDmaPointer;
        uint8_t                     *rxDmaPointer;

        HAL_Semaphore               txReady;
        HAL_Semaphore               rxReady;
        bool                        isTxSemaphore;
        bool                        isRxSemaphore;
        bool                        isTxInitiate;
        bool                        isRxInitiate;

        HAL_Mutex                   devSetLock;
        HAL_Mutex                   devTriggerLock;

        uint32_t                    audioPllParam;
        uint32_t                    audioPllPatParam;
} I2S_Private;

typedef struct {
        uint32_t        hosc;
        uint32_t        audio;
        uint32_t        pllParam;
        uint32_t        pllPatParam;
} HOSC_I2S_Type;

typedef enum {
        I2S_PLL_24M        = 0U,
        I2S_PLL_22M        = 1U,
} I2S_PLLMode;

typedef struct {
	uint8_t       clkDiv;
	I2S_BCLKDIV   bregVal;
	I2S_MCLKDIV   mregVal;
} CLK_DIVRegval;

#define AUDIO_PLL_SRC               (CCM_DAUDIO_MCLK_SRC_1X)
#define AUDIO_PLL_24                (24576000)
#define AUDIO_PLL_22                (22579200)
#define AUDIO_DEVICE_PLL             AUDIO_PLL_22

#define I2S_MEMCPY                   memcpy
#define I2S_MALLOC                   malloc
#define I2S_FREE                     free
#define I2S_MEMSET                   memset

#define UNDERRUN_THRESHOLD           3
#define OVERRUN_THRESHOLD            3

#ifdef RESERVERD_MEMORY_FOR_I2S_TX
static uint8_t I2STX_BUF[I2S_BUF_LENGTH];
#endif
#ifdef RESERVERD_MEMORY_FOR_I2S_RX
static uint8_t I2SRX_BUF[I2S_BUF_LENGTH];
#endif

void HAL_I2S_Trigger(bool enable,I2S_StreamDir dir);
static I2S_Private gI2sPrivate;
static uint32_t I2S_BUF_LENGTH = 0;

/*
 *default hw configuration
 */
static I2S_HWParam gHwParam = {
        0,                  /*0:I2S,1:PCM*/
        DAIFMT_CBS_CFS,
        DAIFMT_I2S,
        DAIFMT_NB_NF,
        32,                 /*16,32,64,128,256*/
        I2S_SHORT_FRAME,
        I2S_TX_MSB_FIRST,
        I2S_RX_MSB_FIRST,
        0x40,
        0xF,
        {AUDIO_DEVICE_PLL,1},
};

static CLK_DIVRegval DivRegval[] = {
        {1,   I2S_BCLKDIV_1,   I2S_MCLKDIV_1},
        {2,   I2S_BCLKDIV_2,   I2S_MCLKDIV_2},
        {4,   I2S_BCLKDIV_4,   I2S_MCLKDIV_4},
        {6,   I2S_BCLKDIV_6,   I2S_MCLKDIV_6},
        {8,   I2S_BCLKDIV_8,   I2S_MCLKDIV_8},
        {12,  I2S_BCLKDIV_12,  I2S_MCLKDIV_12},
        {16,  I2S_BCLKDIV_16,  I2S_MCLKDIV_16},
        {24,  I2S_BCLKDIV_24,  I2S_MCLKDIV_24},
        {32,  I2S_BCLKDIV_32,  I2S_MCLKDIV_32},
        {48,  I2S_BCLKDIV_48,  I2S_MCLKDIV_48},
        {64,  I2S_BCLKDIV_64,  I2S_MCLKDIV_64},
        {96,  I2S_BCLKDIV_96,  I2S_MCLKDIV_96},
        {128, I2S_BCLKDIV_128, I2S_MCLKDIV_128},
        {176, I2S_BCLKDIV_176, I2S_MCLKDIV_176},
        {192, I2S_BCLKDIV_192, I2S_MCLKDIV_192},
        {}
};

static const HOSC_I2S_Type i2s_hosc_aud_type[] = {
        {HOSC_CLOCK_26M, I2S_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC26M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC26M},
        {HOSC_CLOCK_26M, I2S_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC26M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC26M},
        {HOSC_CLOCK_24M, I2S_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC24M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC24M},
        {HOSC_CLOCK_24M, I2S_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC24M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC24M},
        {HOSC_CLOCK_40M, I2S_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC40M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC40M},
        {HOSC_CLOCK_40M, I2S_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC40M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC40M},
        {HOSC_CLOCK_52M, I2S_PLL_24M, PRCM_AUD_PLL24M_PARAM_HOSC52M, PRCM_AUD_PLL24M_PAT_PARAM_HOSC52M},
        {HOSC_CLOCK_52M, I2S_PLL_22M, PRCM_AUD_PLL22M_PARAM_HOSC52M, PRCM_AUD_PLL22M_PAT_PARAM_HOSC52M},
};

/**
  * @internal
  * @brief Set i2s clock frequency
  * @pll I2S clock identifier
  *         This parameter can be one of the following values:
  *            @arg @ref I2S_PLL_22M
  *            @arg @ref I2S_PLL_24M
  * @retval return 0 means success otherwise fail
  */
uint32_t I2S_PLLAUDIO_Update(I2S_PLLMode pll)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;

        if (pll != I2S_PLL_24M &&  pll != I2S_PLL_22M)
                return -1;

        uint32_t hoscClock = HAL_PRCM_GetHFClock();

        int i = 0;
        for (i = 0; i < ARRAY_SIZE(i2s_hosc_aud_type); i++) {
                if ((i2s_hosc_aud_type[i].hosc == hoscClock) && (i2s_hosc_aud_type[i].audio == pll)) {
                        i2sPrivate->audioPllParam = i2s_hosc_aud_type[i].pllParam;
                        i2sPrivate->audioPllPatParam = i2s_hosc_aud_type[i].pllPatParam;
                        break;
                }
        }
        if (i == ARRAY_SIZE(i2s_hosc_aud_type)) {
                I2S_ERROR("Update audio pll failed....\n");
                return -1;
        }
        return 0;
}

/**
  * @internal
  * @brief Enable/disable I2S tx
  * @retval None
  */
static void I2S_DisableTx()
{
        HAL_CLR_BIT(I2S->DA_CTL, I2S_TX_EN_BIT);
}

static void I2S_EnableTx()
{
        HAL_SET_BIT(I2S->DA_CTL, I2S_TX_EN_BIT);
}

/**
  * @internal
  * @brief Enable/disable I2S rx
  * @retval None
  */
static void I2S_DisableRx()
{
        HAL_CLR_BIT(I2S->DA_CTL, I2S_RX_EN_BIT);
}

static void I2S_EnableRx()
{
        HAL_SET_BIT(I2S->DA_CTL, I2S_RX_EN_BIT);
}

/**
  * @internal
  * @brief set the i2s module clock frequency for a given peripheral clk
  * @param isEnable: flag for i2s mclk output
  * @param clkSource: Peripheral clock freq
  * @param pll: the freq of mclk
  * @retval HAL status
  */
static HAL_Status I2S_SET_Mclk(uint32_t isEnable, uint32_t clkSource, uint32_t pll)
{
        if (isEnable == 0) {
                HAL_CLR_BIT(I2S->DA_CLKD, I2S_MCLK_OUT_EN_BIT);
                return HAL_OK;
        }
        uint32_t mclkDiv = pll / clkSource;
        CLK_DIVRegval *divRegval = DivRegval;

        do {
                if (divRegval->clkDiv == mclkDiv) {
                        HAL_MODIFY_REG(I2S->DA_CLKD, I2S_MCLKDIV_MASK, divRegval->mregVal);
                        break;
                }
                divRegval++;
        } while (divRegval->mregVal < I2S_MCLKDIV_192);
        HAL_SET_BIT(I2S->DA_CLKD, I2S_MCLK_OUT_EN_BIT);
        return HAL_OK;
}

/**
  * @internal
  * @brief set the Sample Resolution
  * @param param: pointer to a I2S_DataParam structure that contains
  *        data format information
  * @retval HAL status
  */
static HAL_Status I2S_SET_SampleResolution(I2S_DataParam *param)
{
        if (!param)
                return HAL_INVALID;
        I2S_SampleResolution res = param->resolution;

        if (I2S_SR8BIT <= res &&  res <= I2S_SR32BIT)
                HAL_MODIFY_REG(I2S->DA_FMT0, I2S_SR_MASK, res);
        else {
                I2S_ERROR("Invalid sample resolution (%d) failed\n",res);
                return HAL_ERROR;
        }
        return HAL_OK;
}

/**
  * @internal
  * @brief set the i2s bclk lrck freq
  * @param param: pointer to a I2S_DataParam structure that contains
  *        data format information
  * @param hwParam: pointer to a I2S_HWParam structure that contains
  *        the configuration for clk/mode/format.
  * @retval HAL status
  */
static HAL_Status I2S_SET_ClkDiv(I2S_DataParam *param,  I2S_HWParam *hwParam)
{
        int32_t ret = HAL_OK;
        if (!param || !hwParam)
                return HAL_INVALID;
        uint32_t rate = 0;
        I2S_SampleRate SR = param->sampleRate;
        uint32_t Period = hwParam->lrckPeriod;
        uint16_t bclkDiv = 0;
        uint32_t audioPll = AUDIO_PLL_24;

        switch (SR) {
                case I2S_SR8K:/*  8000Hz  */
                        rate = 8000;
                        break;
                case I2S_SR12K:/*  12000Hz */
                        rate = 12000;
                        break;
                case I2S_SR16K:/*  16000Hz */
                        rate = 16000;
                        break;
                case I2S_SR24K:/*  24000Hz */
                        rate = 24000;
                        break;
                case I2S_SR32K:/*  32000Hz */
                        rate = 32000;
                        break;
                case I2S_SR48K:/*  48000Hz */
                        rate = 48000;
                        break;
                case I2S_SR11K:
                        rate = 11025;
                        break;
                case I2S_SR22K:
                        rate = 22050;
                        break;
                case I2S_SR44K:
                        rate = 44100;
                        break;
                default:
                        I2S_ERROR("Invalid sample rate(%x) failed...\n",rate);
                        return HAL_INVALID;
        }

        I2S_DEBUG("SAMPLE RATE:%d...\n",rate);
        if ((rate % 1000) != 0)
                audioPll = AUDIO_PLL_22;

        I2S_Private *i2sPrivate = &gI2sPrivate;

        /*set sysclk*/
        if (audioPll == AUDIO_PLL_24) {
                I2S_PLLAUDIO_Update(I2S_PLL_24M);
                HAL_PRCM_SetAudioPLLParam(i2sPrivate->audioPllParam);
                HAL_PRCM_SetAudioPLLPatternParam(i2sPrivate->audioPllPatParam);
        } else {
                I2S_PLLAUDIO_Update(I2S_PLL_22M);
                HAL_PRCM_SetAudioPLLParam(i2sPrivate->audioPllParam);
                HAL_PRCM_SetAudioPLLPatternParam(i2sPrivate->audioPllPatParam);
        }

        /*config bclk pll div*/
        if (!hwParam->i2sFormat)
                bclkDiv = audioPll/(2*Period*rate);
        else
                bclkDiv = audioPll/(Period*rate);

        CLK_DIVRegval *divRegval = DivRegval;
        do {
                if (divRegval->clkDiv == bclkDiv) {
                        HAL_MODIFY_REG(I2S->DA_CLKD, I2S_BCLKDIV_MASK, divRegval->bregVal);
                        break;
                }
                divRegval++;
        } while (divRegval->bregVal < I2S_BCLKDIV_192);

        return ret;
}

/**
  * @internal
  * @brief set the i2s transfer format
  * @param param: pointer to a I2S_HWParam structure that contains
  *        the configuration for clk/mode/format.
  * @retval HAL status
  */
static HAL_Status I2S_SET_Format(I2S_HWParam *param)
{
        int32_t ret = HAL_OK;
        if (!param)
                return HAL_INVALID;

        /* config clk mode*/
        switch (param->clkMode) {
                case DAIFMT_CBM_CFM:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_BCLK_OUT_MASK|I2S_LRCK_OUT_MASK|I2S_SDO0_EN_BIT,
                                        I2S_BCLK_INPUT|I2S_LRCK_INPUT|I2S_SDO0_EN_BIT);

                        break;
                case DAIFMT_CBS_CFS:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_BCLK_OUT_MASK|I2S_LRCK_OUT_MASK|I2S_SDO0_EN_BIT,
                                        I2S_BCLK_OUTPUT|I2S_LRCK_OUTPUT|I2S_SDO0_EN_BIT);
                        break;
                default:
                        ret = HAL_INVALID;
                        I2S_ERROR("Invalid DAI format,failed (%d)...\n",param->clkMode);
                        break;
        }

        /* config transfer format */
        switch (param->transferFormat) {
                case DAIFMT_I2S:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_MODE_SEL_MASK, I2S_LEFT_MODE);
                        HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_OFFSET_MASK, I2S_TX_ONEBCLK_OFFSET);
                        HAL_MODIFY_REG(I2S->DA_RXCHSEL, I2S_RXN_OFFSET_MASK, I2S_RX_ONEBCLK_OFFSET);
                        break;
                case DAIFMT_RIGHT_J:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_MODE_SEL_MASK, I2S_RIGHT_MODE);
                        break;
                case DAIFMT_LEFT_J:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_MODE_SEL_MASK, I2S_LEFT_MODE);
                        HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_OFFSET_MASK, I2S_TX_NO_OFFSET);
                        HAL_MODIFY_REG(I2S->DA_RXCHSEL, I2S_RXN_OFFSET_MASK, I2S_RX_NO_OFFSET);
                        break;
                case DAIFMT_DSP_A:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_MODE_SEL_MASK, I2S_PCM_MODE);
                        HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_OFFSET_MASK, I2S_TX_ONEBCLK_OFFSET);
                        HAL_MODIFY_REG(I2S->DA_RXCHSEL, I2S_RXN_OFFSET_MASK, I2S_RX_ONEBCLK_OFFSET);
                        break;
                case DAIFMT_DSP_B:
                        HAL_MODIFY_REG(I2S->DA_CTL, I2S_MODE_SEL_MASK, I2S_PCM_MODE);
                        HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_OFFSET_MASK, I2S_TX_NO_OFFSET);
                        HAL_MODIFY_REG(I2S->DA_RXCHSEL, I2S_RXN_OFFSET_MASK, I2S_RX_NO_OFFSET);
                        break;
                default:
                        ret = HAL_INVALID;
                        I2S_ERROR("Invalid transfer format,failed (%d)...\n",param->transferFormat);
                        break;
        }

        /* config signal interval */
        switch (param->signalInterval) {
                case DAIFMT_NB_NF:
                        HAL_CLR_BIT(I2S->DA_FMT0, I2S_LRCK_POLARITY_MASK);
                        HAL_CLR_BIT(I2S->DA_FMT0, I2S_BCLK_POLARITY_MASK);
                        break;
                case DAIFMT_NB_IF:
                        HAL_SET_BIT(I2S->DA_FMT0, I2S_LRCK_POLARITY_MASK);
                        HAL_CLR_BIT(I2S->DA_FMT0, I2S_BCLK_POLARITY_MASK);
                        break;
                case DAIFMT_IB_NF:
                        HAL_CLR_BIT(I2S->DA_FMT0, I2S_LRCK_POLARITY_MASK);
                        HAL_SET_BIT(I2S->DA_FMT0, I2S_BCLK_POLARITY_MASK);
                        break;
                case DAIFMT_IB_IF:
                        HAL_SET_BIT(I2S->DA_FMT0, I2S_LRCK_POLARITY_MASK);
                        HAL_SET_BIT(I2S->DA_FMT0, I2S_BCLK_POLARITY_MASK);
                        break;
                default:
                        ret = HAL_INVALID;
                        I2S_ERROR("Invalid signal Interval,failed (%d)...\n",param->signalInterval);
                        break;
        }
        return ret;
}

/**
  * @internal
  * @brief set the number channels of i2s transfer
  * @param param: pointer to a I2S_DataParam structure that contains
  *         data format information.
  * @retval HAL status
  */
static HAL_Status I2S_SET_Channels(I2S_DataParam *param)
{
        uint8_t channel = 0;
        if (!param)
                return HAL_INVALID;

        if (param->direction == PLAYBACK) {/*play*/
                if (param->channels < I2S_TX_SLOT_NUM1 || param->channels > I2S_TX_SLOT_NUM8) {
                        I2S_ERROR("Invalid usr tx channels num,failed (%d)...\n",param->channels);
                        return HAL_INVALID;
                }
                HAL_MODIFY_REG(I2S->DA_CHCFG, I2S_TX_SLOT_NUM_MASK, ((param->channels - 1) << I2S_TX_SLOT_NUM_SHIFT));
                HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_CHANNEL_SEL_MASK,
                                                 ((param->channels -1) << I2S_TXN_CHANNEL_SEL_SHIFT));
                HAL_MODIFY_REG(I2S->DA_TX0CHSEL, I2S_TXN_CHANNEL_SLOT_ENABLE_MASK,
                                                 I2S_TXN_CHANNEL_SLOTS_ENABLE(param->channels));
                for (channel = 0; channel < param->channels; channel++) {
                        HAL_MODIFY_REG(I2S->DA_TX0CHMAP, I2S_TXN_CHX_MAP_MASK(channel),I2S_TXN_CHX_MAP(channel));
                }

        } else {/*record*/
                if (param->channels < I2S_RX_SLOT_NUM1 || param->channels > I2S_RX_SLOT_NUM8){
                        I2S_ERROR("Invalid usr rx channels num,failed (%d)...\n",param->channels);
                        return HAL_INVALID;
                }

                HAL_MODIFY_REG(I2S->DA_CHCFG, I2S_RX_SLOT_NUM_MASK, ((param->channels - 1) << I2S_RX_SLOT_NUM_SHIFT));
                HAL_MODIFY_REG(I2S->DA_RXCHSEL, I2S_RXN_CHANNEL_SEL_MASK,
                                                ((param->channels - 1) << I2S_RXN_CHANNEL_SEL_SHIFT));
                for (channel = 0; channel < param->channels; channel++) {
                        HAL_MODIFY_REG(I2S->DA_RXCHMAP, I2S_RXN_CHX_MAP_MASK(channel), I2S_RXN_CHX_MAP(channel));
                }
        }
        return HAL_OK;
}

static int I2S_DMA_BUFFER_CHECK_Threshold(uint8_t dir)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        if (dir == 0) {
                if (i2sPrivate->txHalfCallCount >= UNDERRUN_THRESHOLD ||
                        i2sPrivate->txEndCallCount >= UNDERRUN_THRESHOLD) {
                        I2S_ERROR("Tx : underrun and stop dma tx....\n");
                        HAL_I2S_Trigger(false,PLAYBACK);/*stop*/
                        i2sPrivate->txRunning =false;
                        i2sPrivate->writePointer = NULL;
                        i2sPrivate->txHalfCallCount = 0;
                        i2sPrivate->txEndCallCount = 0;
                        i2sPrivate->txDmaPointer = NULL;
                        return -1;
                }
        } else {
                if (i2sPrivate->rxHalfCallCount >= OVERRUN_THRESHOLD ||
                        i2sPrivate->rxEndCallCount >= OVERRUN_THRESHOLD) {
                        I2S_ERROR("Rx : overrun and stop dma rx....\n");
                        HAL_I2S_Trigger(false,RECORD);/*stop*/
                        i2sPrivate->rxRunning =false;
                        i2sPrivate->rxHalfCallCount = 0;
                        i2sPrivate->rxEndCallCount = 0;
                        i2sPrivate->readPointer = NULL;
                        i2sPrivate->rxDmaPointer = NULL;
                        return -1;
                }
        }
        return 0;
}

/**
  * @internal
  * @brief DMA I2S transmit/receive process half complete callback
  * @param arg: pointer to a HAL_Semaphore structure that contains
  *             sem to synchronous data.
  * @retval None
  */
static void I2S_DMAHalfCallback(void *arg)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        if (arg == &(i2sPrivate->txReady)) {
                i2sPrivate->txHalfCallCount ++;
                if (I2S_DMA_BUFFER_CHECK_Threshold(0) != 0)
                        return;
                i2sPrivate->txDmaPointer = i2sPrivate->txBuf + I2S_BUF_LENGTH/2;
                if (i2sPrivate->isTxSemaphore) {
                        i2sPrivate->isTxSemaphore = false;
                        HAL_SemaphoreRelease((HAL_Semaphore *)arg);
                }

        } else {
                i2sPrivate->rxHalfCallCount ++;
                if (I2S_DMA_BUFFER_CHECK_Threshold(1) != 0)
                        return;
                i2sPrivate->rxDmaPointer = i2sPrivate->rxBuf + I2S_BUF_LENGTH/2;
                if (i2sPrivate->isRxSemaphore) {
                        i2sPrivate->isRxSemaphore = false;
                        HAL_SemaphoreRelease((HAL_Semaphore *)arg);
                }
        }
}

/**
  * @internal
  * @brief DMA I2S transmit/receive process complete callback
  * @param arg: pointer to a HAL_Semaphore structure that contains
  *             sem to synchronous data.
  * @retval None
  */
static void I2S_DMAEndCallback(void *arg)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        if (arg == &(i2sPrivate->txReady)) {
                i2sPrivate->txEndCallCount ++;
                if (I2S_DMA_BUFFER_CHECK_Threshold(0) != 0)
                        return;
                i2sPrivate->txDmaPointer = i2sPrivate->txBuf;
                if (i2sPrivate->isTxSemaphore) {
                        i2sPrivate->isTxSemaphore = false;
                        HAL_SemaphoreRelease((HAL_Semaphore *)arg);
                }
        } else {
                i2sPrivate->rxEndCallCount ++;
                if (I2S_DMA_BUFFER_CHECK_Threshold(1) != 0)
                        return;
                i2sPrivate->rxDmaPointer = i2sPrivate->rxBuf + I2S_BUF_LENGTH/2;
                if (i2sPrivate->isRxSemaphore) {
                        i2sPrivate->isRxSemaphore = false;
                        HAL_SemaphoreRelease((HAL_Semaphore *)arg);
                }
        }
}

/**
  * @internal
  * @brief Start the DMA Transfer.
  * @param chan: the specified DMA Channel.
  * @param srcAddr: The source memory Buffer address
  * @param dstAddr: The destination memory Buffer address
  * @param datalen: The length of data to be transferred from source to destination
  * @retval none
  */
static void I2S_DMAStart(DMA_Channel chan, uint32_t srcAddr, uint32_t dstAddr, uint32_t datalen)
{
        HAL_DMA_Start(chan, srcAddr, dstAddr, datalen);
}

/**
  * @internal
  * @brief stop the DMA Transfer.
  * @param chan: the specified DMA Channel.
  * @retval none
  */
static void I2S_DMAStop(DMA_Channel chan)
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
static void I2S_DMASet(DMA_Channel channel,I2S_StreamDir dir)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        DMA_ChannelInitParam dmaParam;
        HAL_Memset(&dmaParam, 0, sizeof(dmaParam));
        if (dir == PLAYBACK) {

                dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_CIRCULAR,
                                DMA_WAIT_CYCLE_2,
                                DMA_BYTE_CNT_MODE_REMAIN,
                                DMA_DATA_WIDTH_16BIT,
                                DMA_BURST_LEN_1,
                                DMA_ADDR_MODE_FIXED,
                                DMA_PERIPH_DAUDIO,
                                DMA_DATA_WIDTH_16BIT,
                                DMA_BURST_LEN_1,
                                DMA_ADDR_MODE_INC,
                                DMA_PERIPH_SRAM);

                dmaParam.endArg = &(i2sPrivate->txReady);
                dmaParam.halfArg = &(i2sPrivate->txReady);
        } else {

                dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_CIRCULAR,
                                DMA_WAIT_CYCLE_2,
                                DMA_BYTE_CNT_MODE_REMAIN,
                                DMA_DATA_WIDTH_16BIT,
                                DMA_BURST_LEN_1,
                                DMA_ADDR_MODE_INC,
                                DMA_PERIPH_SRAM,
                                DMA_DATA_WIDTH_16BIT,
                                DMA_BURST_LEN_1,
                                DMA_ADDR_MODE_FIXED,
                                DMA_PERIPH_DAUDIO);

                dmaParam.endArg = &(i2sPrivate->rxReady);
                dmaParam.halfArg = &(i2sPrivate->rxReady);
        }
        dmaParam.irqType = DMA_IRQ_TYPE_BOTH;
        dmaParam.endCallback = I2S_DMAEndCallback;
        dmaParam.halfCallback = I2S_DMAHalfCallback;
        HAL_DMA_Init(channel, &dmaParam);
}

/**
  * @internal
  * @brief Enable or disable the specified i2s tx module.
  * @param enable: specifies enable or disable.
  * @retval None
  */
static void tx_enable(bool enable)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        /*clear tx tifo*/
        HAL_SET_BIT(I2S->DA_FCTL, I2S_TXFIFO_RESET_BIT);

        if (enable) {
                if (i2sPrivate->txDMAChan != DMA_CHANNEL_INVALID)
                        HAL_SET_BIT(I2S->DA_INT, I2S_TXFIFO_DMA_ITEN_BIT);
        } else {
                if (i2sPrivate->txDMAChan != DMA_CHANNEL_INVALID)
                        HAL_CLR_BIT(I2S->DA_INT, I2S_TXFIFO_DMA_ITEN_BIT);
        }
}

/**
  * @internal
  * @brief Enable or disable the specified i2s rx module.
  * @param enable: specifies enable or disable.
  * @retval None
  */
static void rx_enable(bool enable)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        /*clear rx tifo*/
        HAL_SET_BIT(I2S->DA_FCTL, I2S_RXFIFO_RESET_BIT);

        if (enable) {
                if (i2sPrivate->rxDMAChan != DMA_CHANNEL_INVALID)
                        HAL_SET_BIT(I2S->DA_INT, I2S_RXFIFO_DMA_ITEN_BIT);
        } else {

                if (i2sPrivate->rxDMAChan != DMA_CHANNEL_INVALID)
                        HAL_CLR_BIT(I2S->DA_INT, I2S_RXFIFO_DMA_ITEN_BIT);
        }
}

/**
  * @internal
  * @brief Enable or disable the specified i2s module with DMA module.
  * @param enable: specifies enable or disable.
  * @param dir: the direction of stream.
  * @retval None
  */
void HAL_I2S_Trigger(bool enable,I2S_StreamDir dir)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        HAL_MutexLock(&i2sPrivate->devTriggerLock, OS_WAIT_FOREVER);
        if (enable) {
                if (dir == PLAYBACK) {
                        /*trigger tx*/
                        tx_enable(enable);

                        /* start dma*/
                        if (i2sPrivate->txDMAChan != DMA_CHANNEL_INVALID) {
                                I2S_DMAStart(i2sPrivate->txDMAChan, (uint32_t)i2sPrivate->txBuf,
                                                (uint32_t)&(I2S->DA_TXFIFO), i2sPrivate->txLength);
                        }
                        i2sPrivate->txRunning = true;
                } else {
                        rx_enable(enable);
                        if (i2sPrivate->rxDMAChan != DMA_CHANNEL_INVALID)
                                I2S_DMAStart(i2sPrivate->rxDMAChan, (uint32_t)&(I2S->DA_RXFIFO),
                                                (uint32_t)i2sPrivate->rxBuf, i2sPrivate->rxLength);
                        i2sPrivate->rxRunning = true;
                }
        } else {
                if (dir == PLAYBACK) {
                        tx_enable(enable);
                        if (i2sPrivate->txDMAChan != DMA_CHANNEL_INVALID)
                                I2S_DMAStop(i2sPrivate->txDMAChan);
                        i2sPrivate->txRunning = false;
                } else {
                        rx_enable(enable);
                        if (i2sPrivate->rxDMAChan != DMA_CHANNEL_INVALID)
                                I2S_DMAStop(i2sPrivate->rxDMAChan);
                        i2sPrivate->rxRunning = false;
                }
        }
        #if 0
        int i =0 ;
        printf("\n");
        for (i = 0; i < 0x68; i = i+4) {
                printf("0x%x: 0x%08x",i,(*(uint32_t *)(0x40042c00+i)));
                printf("\n");
        }
        #endif
        HAL_MutexUnlock(&i2sPrivate->devTriggerLock);
}

/**
  * @brief Transmit an amount of data with DMA module
  * @param buf: pointer to the Transmit data buffer.
  * @param Size: number of data sample to be sent:
  * @note if the data size less than half of dmabuf, return HAL_INVALID. when the size
  *       several times greater than half of dmabuf, only transfer its(half length of
  *       dmabuf)integer multiple
  * @retval length of data transmited
  */
int32_t HAL_I2S_Write_DMA(uint8_t *buf, uint32_t size)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        if (!buf || size <= 0)
                return HAL_INVALID;
        uint8_t *pdata = buf;
        uint8_t *lastWritePointer = NULL;
        uint32_t toWrite = 0,writeSize = I2S_BUF_LENGTH/2;
        uint8_t err_flag; /* temp solution to avoid outputing debug message when irq disabled */

        if (size < writeSize) {
                //  I2S_INFO("Tx : size is too small....\n");
                return HAL_INVALID;
        }
        while (size > 0) {
                if (size < writeSize)
                        break;
                if (i2sPrivate->txRunning == false) {
                        if (!i2sPrivate->writePointer)
                                i2sPrivate->writePointer = i2sPrivate->txBuf;

                        lastWritePointer = i2sPrivate->writePointer;

                        I2S_MEMCPY(lastWritePointer, pdata, writeSize);
                        pdata += writeSize;
                        lastWritePointer += writeSize;
                        toWrite += writeSize;
                        size -= writeSize;
                        if (lastWritePointer >= i2sPrivate->txBuf + I2S_BUF_LENGTH)
                                lastWritePointer = i2sPrivate->txBuf;
                        i2sPrivate->writePointer = lastWritePointer;
                        if (i2sPrivate->writePointer == i2sPrivate->txBuf) {
                                I2S_DEBUG("Tx: play start...\n");
                                HAL_I2S_Trigger(true,PLAYBACK);/*play*/
                                i2sPrivate->txRunning =true;
                        }
                } else {
                        err_flag = 0;
                        /*disable irq*/
                        HAL_DisableIRQ();

                        lastWritePointer = i2sPrivate->writePointer;
                        if (i2sPrivate->txHalfCallCount && i2sPrivate->txEndCallCount) {
                                err_flag = 1;
                                i2sPrivate->txHalfCallCount = 0;
                                i2sPrivate->txEndCallCount = 0;

                                if (i2sPrivate->txDmaPointer == i2sPrivate->txBuf) {
                                        lastWritePointer = i2sPrivate->txBuf + I2S_BUF_LENGTH/2;
                                } else {
                                        lastWritePointer = i2sPrivate->txBuf;
                                }
                        } else if (i2sPrivate->txHalfCallCount) {
                                i2sPrivate->txHalfCallCount --;
                        } else if (i2sPrivate->txEndCallCount) {
                                i2sPrivate->txEndCallCount --;
                        } else {
                                /**enable irq**/
                                i2sPrivate->isTxSemaphore = true;
                                HAL_EnableIRQ();
                                HAL_SemaphoreWait(&(i2sPrivate->txReady), HAL_WAIT_FOREVER);
                                /**disable irq**/
                                HAL_DisableIRQ();
                                if (i2sPrivate->txHalfCallCount)
                                        i2sPrivate->txHalfCallCount --;
                                if (i2sPrivate->txEndCallCount)
                                        i2sPrivate->txEndCallCount --;
                        }

                        I2S_MEMCPY(lastWritePointer, pdata, writeSize);
                        pdata += writeSize;
                        lastWritePointer += writeSize;
                        toWrite += writeSize;
                        size -= writeSize;
                        if (lastWritePointer >= i2sPrivate->txBuf + I2S_BUF_LENGTH)
                                lastWritePointer = i2sPrivate->txBuf;
                        i2sPrivate->writePointer = lastWritePointer;

                        /**enable irq**/
                        HAL_EnableIRQ();
                        if (err_flag) {
                            I2S_ERROR("TxCount:(H:%d,F:%d)\n",i2sPrivate->txHalfCallCount,
                                                              i2sPrivate->txEndCallCount);
                            I2S_ERROR("Tx : underrun....\n");
                        }
                }
        }
        return toWrite;
}

/**
  * @brief receive an amount of data with DMA module
  * @param buf: pointer to the receive data buffer.
  * @param Size: number of data sample to be receive:
  * @note if the data size less than half of dmabuf, return HAL_INVALID. when the size
  *       several times greater than half of dmabuf, only transfer its(half length of
  *       dmabuf)integer multiple
  * @retval length of data received
  */
int32_t HAL_I2S_Read_DMA(uint8_t *buf, uint32_t size)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        if (!buf || size <= 0)
                return HAL_INVALID;
        uint8_t *pdata = buf;
        uint8_t *lastReadPointer = NULL;
        uint32_t readSize = I2S_BUF_LENGTH/2;
        uint32_t toRead = 0;
        uint8_t err_flag; /* temp solution to avoid outputing debug message when irq disabled */

        if ((size / readSize) < 1) {
                I2S_ERROR("Rx buf size too small...\n");
                return HAL_INVALID;
        }

        while (size > 0) {

                if (size/readSize < 1)
                        break;
                if (i2sPrivate->rxRunning == false) {

                        if (!i2sPrivate->readPointer)
                                i2sPrivate->readPointer = i2sPrivate->rxBuf;
                        I2S_DEBUG("Rx: record start...\n");
                        HAL_I2S_Trigger(true,RECORD);

                } else {
                        err_flag = 0;
                        /*disable irq*/
                        HAL_DisableIRQ();
                        lastReadPointer = i2sPrivate->readPointer;
                        if (i2sPrivate->rxHalfCallCount && i2sPrivate->rxEndCallCount) {
                                err_flag = 1;
                                i2sPrivate->rxHalfCallCount = 0;
                                i2sPrivate->rxEndCallCount = 0;

                                if (i2sPrivate->rxDmaPointer == i2sPrivate->rxBuf) {
                                        lastReadPointer = i2sPrivate->txBuf + I2S_BUF_LENGTH/2;
                                } else {
                                        lastReadPointer = i2sPrivate->txBuf;
                                }
                        } else if (i2sPrivate->rxHalfCallCount) {
                                i2sPrivate->rxHalfCallCount --;
                        } else if (i2sPrivate->rxEndCallCount) {
                                i2sPrivate->rxEndCallCount --;
                        } else {
                                /**enable irq**/
                                i2sPrivate->isRxSemaphore = true;
                                HAL_EnableIRQ();
                                HAL_SemaphoreWait(&(i2sPrivate->rxReady), HAL_WAIT_FOREVER);
                                /**disable irq**/
                                HAL_DisableIRQ();
                                if (i2sPrivate->rxHalfCallCount)
                                        i2sPrivate->rxHalfCallCount --;
                                if (i2sPrivate->rxEndCallCount)
                                        i2sPrivate->rxEndCallCount --;
                        }
                        I2S_MEMCPY(pdata, lastReadPointer, readSize);
                        pdata += readSize;
                        lastReadPointer += readSize;
                        if (lastReadPointer >= i2sPrivate->rxBuf + I2S_BUF_LENGTH)
                                lastReadPointer = i2sPrivate->rxBuf;
                        i2sPrivate->readPointer = lastReadPointer;
                        /**enable irq**/
                        HAL_EnableIRQ();
                        if (err_flag) {
                            I2S_ERROR("RxCount:(H:%d,F:%d)\n",i2sPrivate->rxHalfCallCount,
                                                              i2sPrivate->rxEndCallCount);
                            I2S_ERROR("Rx : overrun....\n");
                        }
                        size -= readSize;
                        toRead += readSize;
                }
        }
        return toRead;
}

/**
  * @brief Open the I2S module according to the specified parameters
  *         in the I2S_DataParam.
  * @param param: pointer to a I2S_DataParam structure that contains
  *         data format information
  * @retval HAL status
  */
HAL_Status HAL_I2S_Open(I2S_DataParam *param)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;

        I2S_DataParam *dataParam = param;

        if (param->direction == PLAYBACK) {
                if (i2sPrivate->isTxInitiate == true) {
                        I2S_ERROR("Tx device opened already.open faied...\n");
                        return HAL_ERROR;
                }
                i2sPrivate->isTxInitiate = true;

                I2S_MEMCPY(&(i2sPrivate->pdataParam), param, sizeof(*param));
        } else {
                if (i2sPrivate->isRxInitiate == true) {
                        I2S_ERROR("Rx device opened already.open faied...\n");
                        return HAL_ERROR;
                }
                i2sPrivate->isRxInitiate = true;

                I2S_MEMCPY(&(i2sPrivate->cdataParam), param, sizeof(*param));
        }
        I2S_BUF_LENGTH = dataParam->bufSize;

        if (param->direction == PLAYBACK) {
                i2sPrivate->txDMAChan = DMA_CHANNEL_INVALID;
                i2sPrivate->txLength = I2S_BUF_LENGTH;
                i2sPrivate->txHalfCallCount = 0;
                i2sPrivate->txEndCallCount = 0;
#ifdef RESERVERD_MEMORY_FOR_I2S_TX
                i2sPrivate->txBuf = I2STX_BUF;
#else
                i2sPrivate->txBuf = I2S_MALLOC(I2S_BUF_LENGTH);
                if(i2sPrivate->txBuf)
                        I2S_MEMSET(i2sPrivate->txBuf,0,I2S_BUF_LENGTH);
                else {
                        I2S_ERROR("Malloc tx buf(for DMA),faild...\n");
                        return HAL_ERROR;
                }
#endif
                /*request DMA channel*/
                i2sPrivate->txDMAChan = HAL_DMA_Request();
                if (i2sPrivate->txDMAChan == DMA_CHANNEL_INVALID) {
                        I2S_ERROR("Obtain I2S tx DMA channel,faild...\n");
#ifndef RESERVERD_MEMORY_FOR_I2S_TX
                        I2S_FREE(i2sPrivate->txBuf);
#endif
                        return HAL_ERROR;
                } else
                I2S_DMASet(i2sPrivate->txDMAChan, PLAYBACK);
                HAL_SemaphoreInitBinary(&i2sPrivate->txReady);
        } else {
                i2sPrivate->rxDMAChan = DMA_CHANNEL_INVALID;
                i2sPrivate->rxLength = I2S_BUF_LENGTH;
                i2sPrivate->rxHalfCallCount = 0;
                i2sPrivate->rxEndCallCount = 0;
#ifdef RESERVERD_MEMORY_FOR_I2S_RX
                i2sPrivate->rxBuf = I2SRX_BUF;
#else
                i2sPrivate->rxBuf = I2S_MALLOC(I2S_BUF_LENGTH);
                if(i2sPrivate->rxBuf)
                        I2S_MEMSET(i2sPrivate->rxBuf,0,I2S_BUF_LENGTH);
                else {
                        I2S_ERROR("Malloc rx buf(for DMA),faild...\n");
                        return HAL_ERROR;
                }
#endif
                i2sPrivate->rxDMAChan = HAL_DMA_Request();
                if (i2sPrivate->rxDMAChan == DMA_CHANNEL_INVALID) {
                        I2S_ERROR("Obtain I2S rx DMA channel,faild...\n");
#ifndef RESERVERD_MEMORY_FOR_I2S_TX
                        I2S_FREE(i2sPrivate->rxBuf);
#endif
                        return HAL_ERROR;
                } else
                        I2S_DMASet(i2sPrivate->rxDMAChan, RECORD);
                HAL_SemaphoreInitBinary(&i2sPrivate->rxReady);
        }

        HAL_MutexLock(&i2sPrivate->devSetLock, OS_WAIT_FOREVER);
        /*set bclk*/
        I2S_SET_ClkDiv(dataParam, i2sPrivate->hwParam);

        /*set sample resolution*/
        I2S_SET_SampleResolution(dataParam);

        /*set channel*/
        I2S_SET_Channels(dataParam);

        if (param->direction == PLAYBACK) {
                I2S_EnableTx();
        } else {
                I2S_EnableRx();
        }
        HAL_MutexUnlock(&i2sPrivate->devSetLock);

        return HAL_OK;
}

/**
  * @brief Close the I2S module
  * @note The module is closed at the end of transaction to avoid power consumption
  * @retval none
  */
HAL_Status HAL_I2S_Close(uint32_t dir)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;

        if (dir == PLAYBACK) {
                HAL_I2S_Trigger(false,PLAYBACK);
                I2S_DisableTx();
                i2sPrivate->txRunning = false;
                i2sPrivate->isTxInitiate = false;
                if (i2sPrivate->txDMAChan != DMA_CHANNEL_INVALID) {
                        HAL_DMA_DeInit(i2sPrivate->txDMAChan);
                        HAL_DMA_Release(i2sPrivate->txDMAChan);
                        i2sPrivate->txDMAChan = DMA_CHANNEL_INVALID;

                }
                I2S_MEMSET(&(i2sPrivate->pdataParam), 0, sizeof(I2S_DataParam));
#ifndef RESERVERD_MEMORY_FOR_I2S_TX
                I2S_FREE(i2sPrivate->txBuf);
#endif
                HAL_SemaphoreDeinit(&i2sPrivate->txReady);
                i2sPrivate->txBuf = NULL;
                i2sPrivate->txLength = 0;
                i2sPrivate->writePointer = NULL;
                i2sPrivate->txHalfCallCount = 0;
                i2sPrivate->txEndCallCount = 0;
        } else {
                HAL_I2S_Trigger(false,RECORD);
                I2S_DisableRx();
                i2sPrivate->isRxInitiate = false;
                i2sPrivate->rxRunning = false;
                if (i2sPrivate->rxDMAChan != DMA_CHANNEL_INVALID) {
                        HAL_DMA_DeInit(i2sPrivate->rxDMAChan);
                        HAL_DMA_Release(i2sPrivate->rxDMAChan);
                        i2sPrivate->rxDMAChan = DMA_CHANNEL_INVALID;

                }
                I2S_MEMSET(&(i2sPrivate->cdataParam), 0, sizeof(I2S_DataParam));
#ifndef RESERVERD_MEMORY_FOR_I2S_TX
                I2S_FREE(i2sPrivate->rxBuf);
#endif
                HAL_SemaphoreDeinit(&i2sPrivate->rxReady);
                i2sPrivate->rxBuf = NULL;
                i2sPrivate->rxLength = 0;
                i2sPrivate->readPointer = NULL;

                i2sPrivate->rxHalfCallCount = 0;
                i2sPrivate->rxEndCallCount = 0;
        }
        return HAL_OK;
}

/**
  * @internal
  * @brief I2S PINS Init
  * @retval HAL status
  */
static inline HAL_Status I2S_PINS_Init()
{
        return HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_I2S, 0), 0);
}

/**
  * @internal
  * @brief I2S PINS DeInit
  * @retval HAL status
  */
static inline HAL_Status I2S_PINS_Deinit()
{
        return HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_I2S, 0), 0);
}

/**
  * @internal
  * @brief I2S hardware Init
  * @param param: pointer to a I2S_HWParam structure that contains
  *         the configuration for clk/mode/format.
  * @retval HAL status
  */
static inline HAL_Status I2S_HwInit(I2S_HWParam *param)
{
        if (!param)
                return HAL_INVALID;

        /*config device clk source*/
        if (param->codecClk.isDevclk != 0) {
                I2S_SET_Mclk(true,param->codecClk.clkSource, AUDIO_DEVICE_PLL);
        }

        /* set lrck period /frame mode */
        HAL_MODIFY_REG(I2S->DA_FMT0, I2S_LRCK_PERIOD_MASK|I2S_LRCK_WIDTH_MASK|
                                     I2S_SW_SEC_MASK,
                                      I2S_LRCK_PERIOD(param->lrckPeriod)|
                                      param->frameMode|I2S_SLOT_WIDTH_BIT32);

        /* set first transfer bit */
        HAL_MODIFY_REG(I2S->DA_FMT1, I2S_TX_MLS_MASK|I2S_RX_MLS_MASK|
                        I2S_PCM_TXMODE_MASK|I2S_PCM_RXMODE_MASK|I2S_SEXT_MASK,
                        param->txMsbFirst|param->rxMsbFirst|
                        I2S_TX_LINEAR_PCM|I2S_RX_LINEAR_PCM|I2S_ZERO_SLOT);
        /* global enable */
        HAL_SET_BIT(I2S->DA_CTL, I2S_GLOBE_EN_BIT);

        HAL_MODIFY_REG(I2S->DA_FCTL, I2S_RXFIFO_LEVEL_MASK|I2S_TXFIFO_LEVEL_MASK|
                                     I2S_RX_FIFO_MODE_SHIFT|I2S_TX_FIFO_MODE_MASK,
                                     I2S_RXFIFO_TRIGGER_LEVEL(param->rxFifoLevel)|
                                     I2S_TXFIFO_TRIGGER_LEVEL(param->txFifoLevel)|
                                     I2S_RX_FIFO_MODE3|I2S_TX_FIFO_MODE1);
        return I2S_SET_Format(param);
}

/**
  * @internal
  * @brief I2S hardware DeInit
  * @param param: pointer to a I2S_HWParam structure
  * @retval HAL status
  */
static inline HAL_Status I2S_HwDeInit(I2S_HWParam *param)
{
        if (!param)
                return HAL_INVALID;
        /* global disable */
        HAL_CLR_BIT(I2S->DA_CTL, I2S_GLOBE_EN_BIT);
        I2S_SET_Mclk(false, 0, 0);
        return HAL_OK;
}

#ifdef CONFIG_PM
static int i2s_suspend(struct soc_device *dev, enum suspend_state_t state)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        switch (state) {
                case PM_MODE_SLEEP:
                case PM_MODE_STANDBY:
                case PM_MODE_HIBERNATION:
                case PM_MODE_POWEROFF:
                        HAL_MutexLock(&i2sPrivate->devSetLock, OS_WAIT_FOREVER);
                        I2S_HwDeInit(i2sPrivate->hwParam);
                        HAL_MutexUnlock(&i2sPrivate->devSetLock);

                        I2S_PINS_Deinit();
                        HAL_CCM_DAUDIO_DisableMClock();
                        HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_DAUDIO);
                        HAL_PRCM_DisableAudioPLL();
                        HAL_PRCM_DisableAudioPLLPattern();
                        break;
                default:
                        break;
        }
        return 0;
}

static int i2s_resume(struct soc_device *dev, enum suspend_state_t state)
{
        I2S_Private *i2sPrivate = &gI2sPrivate;

        switch (state) {
                case PM_MODE_SLEEP:
                case PM_MODE_STANDBY:
                case PM_MODE_HIBERNATION:
                        I2S_PINS_Init();
                        /*init and enable clk*/
                        I2S_PLLAUDIO_Update(I2S_PLL_22M);
                        HAL_PRCM_SetAudioPLLParam(i2sPrivate->audioPllParam);
                        HAL_PRCM_SetAudioPLLPatternParam(i2sPrivate->audioPllPatParam);
                        HAL_PRCM_EnableAudioPLL();
                        HAL_PRCM_EnableAudioPLLPattern();

                        HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_DAUDIO);
                        HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_DAUDIO);
                        HAL_CCM_DAUDIO_SetMClock(AUDIO_PLL_SRC);
                        HAL_CCM_DAUDIO_EnableMClock();

                        HAL_MutexLock(&i2sPrivate->devSetLock, OS_WAIT_FOREVER);
                        I2S_HwInit(i2sPrivate->hwParam);
                        HAL_MutexUnlock(&i2sPrivate->devSetLock);
                        break;
                default:
                        break;
        }
        return 0;
}

static struct soc_device_driver i2s_drv = {
        .name = "I2S",
        .suspend = i2s_suspend,
        .resume = i2s_resume,
};

static struct soc_device i2s_dev = {
        .name = "I2S",
        .driver = &i2s_drv,
};

#define I2S_DEV (&i2s_dev)
#else
#define I2S_DEV NULL
#endif

/**
  * @brief Initializes the I2S module according to the specified parameters
  *         in the I2S_Param.
  * @param param: pointer to a I2S_Param structure that contains
  *         the configuration information for I2S
  * @retval HAL status
  */
HAL_Status HAL_I2S_Init(I2S_Param *param)

{
        int32_t ret = 0;
        if (!param)
                return HAL_INVALID;
        I2S_Private *i2sPrivate = &gI2sPrivate;

        if (i2sPrivate->isHwInit == true)
                return HAL_OK;
        I2S_MEMSET(i2sPrivate,0,sizeof(*i2sPrivate));
        i2sPrivate->isHwInit = true;

        if (param->hwParam == NULL)
                i2sPrivate->hwParam = &gHwParam;
        else
                i2sPrivate->hwParam = param->hwParam;

        HAL_MutexInit(&i2sPrivate->devSetLock);
        HAL_MutexInit(&i2sPrivate->devTriggerLock);

        I2S_PINS_Init();

        /*init and enable clk*/
        I2S_PLLAUDIO_Update(I2S_PLL_22M);
        HAL_PRCM_SetAudioPLLParam(i2sPrivate->audioPllParam);
        HAL_PRCM_EnableAudioPLL();
        HAL_PRCM_SetAudioPLLPatternParam(i2sPrivate->audioPllPatParam);
        HAL_PRCM_EnableAudioPLLPattern();
        HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_DAUDIO);
        HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_DAUDIO);
        HAL_CCM_DAUDIO_SetMClock(AUDIO_PLL_SRC);
        HAL_CCM_DAUDIO_EnableMClock();

        HAL_MutexLock(&i2sPrivate->devSetLock, OS_WAIT_FOREVER);
        ret = I2S_HwInit(i2sPrivate->hwParam);

#ifdef CONFIG_PM
        pm_register_ops(I2S_DEV);
#endif
        HAL_MutexUnlock(&i2sPrivate->devSetLock);
        return ret;
}

#if 0
void HAL_I2S_REG_DEBUG()
{
        int i = 0;
        for (i = 0; i < 0x58; i = i+4)
                printf("REG:0X%x,VAL:0X%x\n",i,(*((__IO uint32_t *)(0x40042c00+i))));
}
#endif

/**
  * @brief DeInitializes the I2S module
  *
  * @retval none
  */
void HAL_I2S_DeInit()
{
        I2S_Private *i2sPrivate = &gI2sPrivate;
        HAL_MutexLock(&i2sPrivate->devSetLock, OS_WAIT_FOREVER);
        i2sPrivate->isHwInit = false;
        I2S_HwDeInit(i2sPrivate->hwParam);
        HAL_MutexUnlock(&i2sPrivate->devSetLock);

        HAL_MutexDeinit(&i2sPrivate->devSetLock);
        HAL_MutexDeinit(&i2sPrivate->devTriggerLock);
        I2S_PINS_Deinit();

        HAL_CCM_DAUDIO_DisableMClock();
        HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_DAUDIO);
        HAL_PRCM_DisableAudioPLL();
        HAL_PRCM_DisableAudioPLLPattern();

        I2S_MEMSET(i2sPrivate,0,sizeof(struct I2S_Private *));
}
