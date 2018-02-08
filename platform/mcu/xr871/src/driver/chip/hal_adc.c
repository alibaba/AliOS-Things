/**
  * @file  hal_adc.c
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

#include "driver/chip/hal_adc.h"
#include "hal_base.h"
#include "pm/pm.h"

typedef enum {
	ADC_STATE_INVALID	= 0,
	ADC_STATE_INIT		= 1, /* Initializing		*/
	ADC_STATE_DEINIT	= 2, /* Deinitializing	*/
	ADC_STATE_READY		= 3,
	ADC_STATE_BUSY		= 4
} ADC_State;

typedef struct {
	uint16_t			chanPinMux;
	ADC_State			state;

	uint32_t			lowPending;
	uint32_t			highPending;
	uint32_t			dataPending;

	ADC_IRQCallback		IRQCallback[ADC_CHANNEL_NUM];
	void			   *arg[ADC_CHANNEL_NUM];
} ADC_Private;

static ADC_Private gADCPrivate;

#define ADC_ASSERT_CHANNEL(chan)	HAL_ASSERT_PARAM((chan) < ADC_CHANNEL_NUM)

#ifdef CONFIG_PM
static ADC_InitParam hal_adc_param;
static struct adc_chan_config{
	uint8_t		is_config;
	uint8_t		select;
	uint8_t		mode;
	uint32_t	lowValue;
	uint32_t	highValue;
} hal_adc_chan_config[ADC_CHANNEL_NUM];
static uint8_t hal_adc_suspending = 0;

static int adc_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	hal_adc_suspending = 1;

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		HAL_ADC_DeInit();
		break;
	default:
		break;
	}

	return 0;
}

static int adc_resume(struct soc_device *dev, enum suspend_state_t state)
{
	static ADC_Channel chan;

	switch (state) {
	case PM_MODE_SLEEP:
		break;
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_ADC_Init(&hal_adc_param);
		for (chan = ADC_CHANNEL_0; chan < ADC_CHANNEL_NUM; chan++) {
			if (hal_adc_chan_config[chan].is_config)
				HAL_ADC_ConfigChannel(chan,
				                      (ADC_Select)hal_adc_chan_config[chan].select,
				                      (ADC_IRQMode)hal_adc_chan_config[chan].mode,
				                      hal_adc_chan_config[chan].lowValue,
				                      hal_adc_chan_config[chan].highValue);
		}
		break;
	default:
		break;
	}

	hal_adc_suspending = 0;

	return 0;
}

static struct soc_device_driver adc_drv = {
	.name = "adc",
	.suspend_noirq = adc_suspend,
	.resume_noirq = adc_resume,
};

static struct soc_device adc_dev = {
	.name = "adc",
	.driver = &adc_drv,
};

#define ADC_DEV (&adc_dev)
#else
#define ADC_DEV NULL
#endif

__STATIC_INLINE void ADC_SetChanPinMux(ADC_Channel chan)
{
	HAL_SET_BIT(gADCPrivate.chanPinMux, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_ClrChanPinMux(ADC_Channel chan)
{
	HAL_CLR_BIT(gADCPrivate.chanPinMux, HAL_BIT(chan));
}

__STATIC_INLINE uint8_t ADC_GetChanPinMux(ADC_Channel chan)
{
	return !!HAL_GET_BIT(gADCPrivate.chanPinMux, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_SetSampleRate(uint32_t fsDiv, uint32_t tAcq)
{
	ADC->SAMPLE_RATE = (HAL_GET_BIT(fsDiv << ADC_FS_DIV_SHIFT, ADC_FS_DIV_MASK)
						| HAL_GET_BIT(tAcq << ADC_TACQ_SHIFT, ADC_TACQ_MASK));
}

__STATIC_INLINE void ADC_SetFirstDelay(uint32_t firstDelay)
{
	HAL_MODIFY_REG(ADC->CTRL, ADC_FIRST_DELAY_MASK,
				   HAL_GET_BIT(firstDelay << ADC_FIRST_DELAY_SHIFT, ADC_FIRST_DELAY_MASK));
}

__STATIC_INLINE void ADC_SetOPBias(uint32_t opBias)
{
	HAL_MODIFY_REG(ADC->CTRL, ADC_OP_BIAS_MASK,
				   HAL_GET_BIT(opBias << ADC_OP_BIAS_SHIFT, ADC_OP_BIAS_MASK));
}

__STATIC_INLINE void ADC_SetWorkMode(ADC_WorkMode workMode)
{
	HAL_MODIFY_REG(ADC->CTRL, ADC_WORK_MODE_MASK,
				   HAL_GET_BIT(workMode << ADC_WORK_MODE_SHIFT, ADC_WORK_MODE_MASK));
}

__STATIC_INLINE void ADC_EnableCalib(void)
{
	HAL_SET_BIT(ADC->CTRL, ADC_CALIB_EN_BIT);
}

__STATIC_INLINE uint32_t ADC_GetCalibState(void)
{
	return !!HAL_GET_BIT(ADC->CTRL, ADC_CALIB_EN_BIT);
}

__STATIC_INLINE void ADC_EnableADC(void)
{
	HAL_SET_BIT(ADC->CTRL, ADC_EN_BIT);
}

__STATIC_INLINE void ADC_DisableADC(void)
{
	HAL_CLR_BIT(ADC->CTRL, ADC_EN_BIT);
}

__STATIC_INLINE void ADC_EnableVbatDetec(void)
{
	HAL_SET_BIT(ADC->CTRL, ADC_VBAT_EN_BIT);
}

__STATIC_INLINE void ADC_DisableVbatDetec(void)
{
	HAL_CLR_BIT(ADC->CTRL, ADC_VBAT_EN_BIT);
}

__STATIC_INLINE void ADC_EnableLDO(void)
{
	HAL_SET_BIT(ADC->CTRL, ADC_LDO_EN_BIT);
}

__STATIC_INLINE void ADC_DisableLDO(void)
{
	HAL_CLR_BIT(ADC->CTRL, ADC_LDO_EN_BIT);
}

__STATIC_INLINE void ADC_EnableChanCmp(ADC_Channel chan)
{
	HAL_SET_BIT(ADC->CMP_SEL_EN, HAL_BIT(ADC_CMP_EN_SHIFT + chan));
}

__STATIC_INLINE void ADC_DisableChanCmp(ADC_Channel chan)
{
	HAL_CLR_BIT(ADC->CMP_SEL_EN, HAL_BIT(ADC_CMP_EN_SHIFT + chan));
}

__STATIC_INLINE void ADC_DisableAllChanCmp(void)
{
	HAL_CLR_BIT(ADC->CMP_SEL_EN, ADC_CMP_EN_MASK);
}

__STATIC_INLINE void ADC_EnableChanSel(ADC_Channel chan)
{
	HAL_SET_BIT(ADC->CMP_SEL_EN, HAL_BIT(ADC_SEL_EN_SHIFT + chan));
}

__STATIC_INLINE void ADC_DisableChanSel(ADC_Channel chan)
{
	HAL_CLR_BIT(ADC->CMP_SEL_EN, HAL_BIT(ADC_SEL_EN_SHIFT + chan));
}

__STATIC_INLINE void ADC_DisableAllChanSel(void)
{
	HAL_CLR_BIT(ADC->CMP_SEL_EN, ADC_SEL_EN_MASK);
}

__STATIC_INLINE void ADC_EnableChanLowIRQ(ADC_Channel chan)
{
	HAL_SET_BIT(ADC->LOW_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_DisableChanLowIRQ(ADC_Channel chan)
{
	HAL_CLR_BIT(ADC->LOW_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE uint8_t ADC_GetChanLowIRQ(ADC_Channel chan)
{
	return !!HAL_GET_BIT(ADC->LOW_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_EnableChanHighIRQ(ADC_Channel chan)
{
	HAL_SET_BIT(ADC->HIGH_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_DisableChanHighIRQ(ADC_Channel chan)
{
	HAL_CLR_BIT(ADC->HIGH_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE uint8_t ADC_GetChanHighIRQ(ADC_Channel chan)
{
	return !!HAL_GET_BIT(ADC->HIGH_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_EnableChanDataIRQ(ADC_Channel chan)
{
	HAL_SET_BIT(ADC->DATA_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_DisableChanDataIRQ(ADC_Channel chan)
{
	HAL_CLR_BIT(ADC->DATA_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE uint8_t ADC_GetChanDataIRQ(ADC_Channel chan)
{
	return !!HAL_GET_BIT(ADC->DATA_CONFIG, HAL_BIT(chan));
}

__STATIC_INLINE void ADC_DisableAllChanIRQ(void)
{
	HAL_CLR_BIT(ADC->LOW_CONFIG, ADC_LOW_IRQ_MASK);
	HAL_CLR_BIT(ADC->HIGH_CONFIG, ADC_HIGH_IRQ_MASK);
	HAL_CLR_BIT(ADC->DATA_CONFIG, ADC_DATA_IRQ_MASK);
}

__STATIC_INLINE uint32_t ADC_GetLowPending(void)
{
	return HAL_GET_BIT(ADC->LOW_STATUS, ADC_LOW_PENDING_MASK);
}

__STATIC_INLINE void ADC_ClrLowPending(uint32_t lowPending)
{
	ADC->LOW_STATUS = lowPending;
}

__STATIC_INLINE uint32_t ADC_GetHighPending(void)
{
	return HAL_GET_BIT(ADC->HIGH_STATUS, ADC_HIGH_PENDING_MASK);
}

__STATIC_INLINE void ADC_ClrHighPending(uint32_t highPending)
{
	ADC->HIGH_STATUS = highPending;
}

__STATIC_INLINE uint32_t ADC_GetDataPending(void)
{
	return HAL_GET_BIT(ADC->DATA_STATUS, ADC_DATA_PENDING_MASK);
}

__STATIC_INLINE void ADC_ClrDataPending(uint32_t dataPending)
{
	ADC->DATA_STATUS = dataPending;
}

__STATIC_INLINE void ADC_SetHighValue(ADC_Channel chan, uint32_t value)
{
	HAL_MODIFY_REG(ADC->CMP_DATA[chan], ADC_HIGH_DATA_MASK,
				   HAL_GET_BIT(value << ADC_HIGH_DATA_SHIFT, ADC_HIGH_DATA_MASK));
}

__STATIC_INLINE void ADC_SetLowValue(ADC_Channel chan, uint32_t value)
{
	HAL_MODIFY_REG(ADC->CMP_DATA[chan], ADC_LOW_DATA_MASK,
				   HAL_GET_BIT(value << ADC_LOW_DATA_SHIFT, ADC_LOW_DATA_MASK));
}

__STATIC_INLINE uint32_t ADC_GetValue(ADC_Channel chan)
{
	return HAL_GET_BIT(ADC->DATA[chan], ADC_DATA_MASK);
}

void GPADC_IRQHandler(void)
{
	uint32_t i;

	gADCPrivate.lowPending	= ADC_GetLowPending();
	gADCPrivate.highPending	= ADC_GetHighPending();
	gADCPrivate.dataPending	= ADC_GetDataPending();

	ADC_ClrLowPending(gADCPrivate.lowPending);
	ADC_ClrHighPending(gADCPrivate.highPending);
	ADC_ClrDataPending(gADCPrivate.dataPending);

	for (i = ADC_CHANNEL_0; i < ADC_CHANNEL_NUM; i++) {
		if (((HAL_GET_BIT(gADCPrivate.dataPending, HAL_BIT(i)) && ADC_GetChanDataIRQ(i))
			|| (HAL_GET_BIT(gADCPrivate.lowPending, HAL_BIT(i)) && ADC_GetChanLowIRQ(i))
			|| (HAL_GET_BIT(gADCPrivate.highPending, HAL_BIT(i)) && ADC_GetChanHighIRQ(i)))
			&& (gADCPrivate.IRQCallback[i])) {
			gADCPrivate.IRQCallback[i](gADCPrivate.arg[i]);
		}
	}
}

/**
 * @brief Initialize the ADC according to the specified parameters
 * @param[in] initParam Pointer to ADC_InitParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_Init(ADC_InitParam *initParam)
{
	unsigned long flags;
	ADC_Private *priv;
	uint32_t hoscClk;
	uint32_t fsDiv;
	uint32_t tAcq;

	if ((initParam->freq < 1000) | (initParam->freq > 1000000)) {
		HAL_ERR("invalid parameter, freq: %d\n", initParam->freq);
		return HAL_ERROR;
	}

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if (priv->state == ADC_STATE_INVALID)
		priv->state = ADC_STATE_INIT;
	else
		priv = NULL;
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_BUSY;
	}

#ifdef CONFIG_PM
	if (!hal_adc_suspending) {
		pm_register_ops(ADC_DEV);
		HAL_Memcpy(&hal_adc_param, initParam, sizeof(ADC_InitParam));
		HAL_Memset(hal_adc_chan_config, 0, ADC_CHANNEL_NUM * sizeof(struct adc_chan_config));
	}
#endif

	priv->chanPinMux		= 0;
	priv->lowPending		= 0;
	priv->highPending		= 0;
	priv->dataPending		= 0;
#ifdef CONFIG_PM
	if (!hal_adc_suspending) {
		HAL_Memset(priv->IRQCallback, 0, ADC_CHANNEL_NUM * sizeof(ADC_IRQCallback));
		HAL_Memset(priv->arg, 0, ADC_CHANNEL_NUM * sizeof(void *));
	}
#else
	HAL_Memset(priv->IRQCallback, 0, ADC_CHANNEL_NUM * sizeof(ADC_IRQCallback));
	HAL_Memset(priv->arg, 0, ADC_CHANNEL_NUM * sizeof(void *));
#endif

	/* enable ADC clock and release reset */
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_GPADC);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_GPADC);
	HAL_CCM_GPADC_SetMClock(CCM_APB_PERIPH_CLK_SRC_LFCLK, CCM_PERIPH_CLK_DIV_N_1, CCM_PERIPH_CLK_DIV_M_1);
	HAL_CCM_GPADC_EnableMClock();

	hoscClk = HAL_GetHFClock();
	fsDiv = hoscClk / initParam->freq - 1;
	if (initParam->freq <= 300000)
		tAcq = hoscClk / 500000 - 1;
	else if (initParam->freq <= 600000)
		tAcq = hoscClk / 1000000 - 1;
	else
		tAcq = hoscClk / 2000000 - 1;
	ADC_SetSampleRate(fsDiv, tAcq);

	ADC_SetFirstDelay(initParam->delay);
	ADC_SetWorkMode(ADC_CONTI_CONV);
	ADC_EnableLDO();

	ADC_DisableAllChanSel();
	ADC_DisableAllChanCmp();
	ADC_DisableAllChanIRQ();

	ADC_EnableCalib();
	while (ADC_GetCalibState())
		;

	/* enable NVIC IRQ */
	HAL_NVIC_SetPriority(GPADC_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_EnableIRQ(GPADC_IRQn);

	flags = HAL_EnterCriticalSection();
	priv->state = ADC_STATE_READY;
	HAL_ExitCriticalSection(flags);

	return HAL_OK;
}

/**
 * @brief DeInitialize the ADC
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_DeInit(void)
{
	unsigned long flags;
	ADC_Private *priv;
	ADC_Channel chan;

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if (priv->state == ADC_STATE_READY)
		priv->state = ADC_STATE_DEINIT;
	else
		priv = NULL;
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

#ifdef CONFIG_PM
	if (!hal_adc_suspending) {
		pm_unregister_ops(ADC_DEV);
	}
#endif

	HAL_NVIC_DisableIRQ(GPADC_IRQn);

	ADC_DisableLDO();

	/* disable ADC clock and force reset */
	HAL_CCM_GPADC_DisableMClock();
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_GPADC);
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_GPADC);

	if (gADCPrivate.chanPinMux) {
		for (chan = ADC_CHANNEL_0; chan < ADC_CHANNEL_NUM; chan++) {
			if (ADC_GetChanPinMux(chan) && (chan != ADC_CHANNEL_8)) {
				HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, chan), 0);
				ADC_ClrChanPinMux(chan);
				if (!gADCPrivate.chanPinMux)
					break;
			}
		}
	}

	flags = HAL_EnterCriticalSection();
	priv->state = ADC_STATE_INVALID;
	HAL_ExitCriticalSection(flags);

	return HAL_OK;
}

/**
 * @brief The specified ADC channel convert once in polling mode
 * @param[in] chan The specified ADC channel
 * @param[in] data Pointer to the output data
 * @param[in] msec Timeout value in millisecond of conversion
 *                 HAL_WAIT_FOREVER for no timeout
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_Conv_Polling(ADC_Channel chan, uint32_t *data, uint32_t msec)
{
	unsigned long 	flags;
	ADC_Private	   *priv;
	uint32_t		stopTime;
	uint8_t			isTimeout;

	ADC_ASSERT_CHANNEL(chan);

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if (priv->state == ADC_STATE_READY)
		priv->state = ADC_STATE_BUSY;
	else
		priv = NULL;
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

	if ((!ADC_GetChanPinMux(chan)) && (chan != ADC_CHANNEL_8)) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, chan), 0);
		ADC_SetChanPinMux(chan);
	}

	ADC_DisableAllChanSel();
	ADC_DisableAllChanCmp();
	ADC_DisableAllChanIRQ();

	if (chan == ADC_CHANNEL_8)
		ADC_EnableVbatDetec();

	ADC_EnableChanSel(chan);

	if (msec == HAL_WAIT_FOREVER)
		stopTime = 0xFFFFFFFF;
	else
		stopTime = HAL_TicksToMSecs(HAL_Ticks()) + msec;

	if (stopTime < msec) {
		HAL_ERR("stopTime overflow.\n");
		return HAL_ERROR;
	}

	isTimeout = 1;
	ADC_EnableADC();
	while (HAL_TicksToMSecs(HAL_Ticks()) <= stopTime) {
		if (HAL_GET_BIT(ADC_GetDataPending(), HAL_BIT(chan))) {
			*data = ADC_GetValue(chan);
			ADC_ClrDataPending(ADC_GetDataPending());
			isTimeout = 0;
			break;
		}
	}
	ADC_DisableADC();
	ADC_DisableChanSel(chan);

	if (chan == ADC_CHANNEL_8)
		ADC_DisableVbatDetec();

	if (ADC_GetChanPinMux(chan) && (chan != ADC_CHANNEL_8)) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, chan), 0);
		ADC_ClrChanPinMux(chan);
	}

	flags = HAL_EnterCriticalSection();
	priv->state = ADC_STATE_READY;
	HAL_ExitCriticalSection(flags);

	if (isTimeout) {
		HAL_WRN("ADC timeout.\n");
		return HAL_TIMEOUT;
	} else {
		return HAL_OK;
	}
}

/**
 * @brief Enable interrupt callback function for the specified ADC channel
 * @param[in] chan The specified ADC channel
 * @param[in] cb The interrupt callback function
 * @param[in] arg Argument of the interrupt callback function
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_EnableIRQCallback(ADC_Channel chan, ADC_IRQCallback cb, void *arg)
{
	unsigned long	flags;
	ADC_Private	   *priv;

	ADC_ASSERT_CHANNEL(chan);

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if ((priv->state == ADC_STATE_READY) || (priv->state == ADC_STATE_BUSY)) {
		priv->arg[chan] = arg;
		priv->IRQCallback[chan] = cb;
	} else {
		priv = NULL;
	}
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Disable interrupt callback function for the specified ADC channel
 * @param[in] chan The specified ADC channel
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_DisableIRQCallback(ADC_Channel chan)
{
	unsigned long	flags;
	ADC_Private	   *priv;

	ADC_ASSERT_CHANNEL(chan);

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if ((priv->state == ADC_STATE_READY) || (priv->state == ADC_STATE_BUSY)) {
		priv->IRQCallback[chan] = NULL;
		priv->arg[chan] = NULL;
	} else {
		priv = NULL;
	}
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Configure the specified ADC channel for conversion in interrupt mode
 * @param[in] chan The specified ADC channel
 * @param[in] select ADC channel selected state
 * @param[in] mode ADC interrupt mode
 * @param[in] lowValue lower limit value in interrupt mode of ADC_IRQ_LOW,
 *            ADC_IRQ_LOW_DATA, ADC_IRQ_LOW_HIGH or ADC_IRQ_LOW_HIGH_DATA
 * @param[in] highValue Upper limit value in interrupt mode of ADC_IRQ_HIGH,
 *            ADC_IRQ_HIGH_DATA, ADC_IRQ_LOW_HIGH or ADC_IRQ_LOW_HIGH_DATA
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_ConfigChannel(ADC_Channel chan, ADC_Select select, ADC_IRQMode mode, uint32_t lowValue, uint32_t highValue)
{
	unsigned long flags;

	ADC_ASSERT_CHANNEL(chan);

	if (((mode == ADC_IRQ_LOW_HIGH_DATA) || (mode == ADC_IRQ_LOW_HIGH)) && (lowValue > highValue)) {
		HAL_ERR("lowValue greater than highValue.\n");
		return HAL_ERROR;
	}

#ifdef CONFIG_PM
	hal_adc_chan_config[chan].is_config = 1;
	hal_adc_chan_config[chan].select = select;
	hal_adc_chan_config[chan].mode = mode;
	hal_adc_chan_config[chan].lowValue = lowValue;
	hal_adc_chan_config[chan].highValue = highValue;
#endif

	flags = HAL_EnterCriticalSection();
	if ((gADCPrivate.state == ADC_STATE_READY) || (gADCPrivate.state == ADC_STATE_BUSY)) {
		if (gADCPrivate.state == ADC_STATE_BUSY)
			ADC_DisableADC();
		if (select == ADC_SELECT_DISABLE) {
			ADC_DisableChanSel(chan);
			if (chan == ADC_CHANNEL_8)
				ADC_DisableVbatDetec();
			ADC_DisableChanDataIRQ(chan);
			ADC_DisableChanCmp(chan);
			ADC_DisableChanLowIRQ(chan);
			ADC_DisableChanHighIRQ(chan);
			if (ADC_GetChanPinMux(chan) && (chan != ADC_CHANNEL_8)) {
				HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, chan), 0);
				ADC_ClrChanPinMux(chan);
			}
		} else {
			if ((!ADC_GetChanPinMux(chan)) && (chan != ADC_CHANNEL_8)) {
				HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_ADC, chan), 0);
				ADC_SetChanPinMux(chan);
			}
			if (chan == ADC_CHANNEL_8)
				ADC_EnableVbatDetec();
			ADC_EnableChanSel(chan);
			switch (mode) {
			case ADC_IRQ_NONE:
				ADC_DisableChanDataIRQ(chan);
				ADC_DisableChanCmp(chan);
				ADC_DisableChanLowIRQ(chan);
				ADC_DisableChanHighIRQ(chan);
				break;
			case ADC_IRQ_DATA:
				ADC_EnableChanDataIRQ(chan);
				ADC_DisableChanCmp(chan);
				ADC_DisableChanLowIRQ(chan);
				ADC_DisableChanHighIRQ(chan);
				break;
			case ADC_IRQ_LOW:
				ADC_DisableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_EnableChanLowIRQ(chan);
				ADC_SetLowValue(chan, lowValue);
				ADC_DisableChanHighIRQ(chan);
				break;
			case ADC_IRQ_HIGH:
				ADC_DisableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_DisableChanLowIRQ(chan);
				ADC_EnableChanHighIRQ(chan);
				ADC_SetHighValue(chan, highValue);
				break;
			case ADC_IRQ_LOW_DATA:
				ADC_EnableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_EnableChanLowIRQ(chan);
				ADC_SetLowValue(chan, lowValue);
				ADC_DisableChanHighIRQ(chan);
				break;
			case ADC_IRQ_HIGH_DATA:
				ADC_EnableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_DisableChanLowIRQ(chan);
				ADC_EnableChanHighIRQ(chan);
				ADC_SetHighValue(chan, highValue);
				break;
			case ADC_IRQ_LOW_HIGH:
				ADC_DisableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_EnableChanLowIRQ(chan);
				ADC_SetLowValue(chan, lowValue);
				ADC_EnableChanHighIRQ(chan);
				ADC_SetHighValue(chan, highValue);
				break;
			case ADC_IRQ_LOW_HIGH_DATA:
				ADC_EnableChanDataIRQ(chan);
				ADC_EnableChanCmp(chan);
				ADC_EnableChanLowIRQ(chan);
				ADC_SetLowValue(chan, lowValue);
				ADC_EnableChanHighIRQ(chan);
				ADC_SetHighValue(chan, highValue);
				break;
			}
		}
		if (gADCPrivate.state == ADC_STATE_BUSY)
			ADC_EnableADC();
		HAL_ExitCriticalSection(flags);
		return HAL_OK;
	} else {
		HAL_ExitCriticalSection(flags);
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}
}

/**
 * @brief Start the ADC conversion in interrupt mode
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_Start_Conv_IT(void)
{
	unsigned long	flags;
	ADC_Private	   *priv;

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if (priv->state == ADC_STATE_READY) {
		ADC_EnableADC();
		priv->state = ADC_STATE_BUSY;
	} else {
		priv = NULL;
	}
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Stop the ADC conversion in interrupt mode
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_ADC_Stop_Conv_IT(void)
{
	unsigned long	flags;
	ADC_Private	   *priv;

	flags = HAL_EnterCriticalSection();
	priv = &gADCPrivate;
	if (priv->state == ADC_STATE_BUSY) {
		ADC_DisableADC();
		priv->state = ADC_STATE_READY;
	} else {
		priv = NULL;
	}
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("ADC state: %d\n", gADCPrivate.state);
		return HAL_ERROR;
	}

	return HAL_OK;
}

/**
 * @brief Get interrupt mode of the specified ADC channel
 * @param[in] chan The specified ADC channel
 * @retval HAL_Status, HAL_OK on success
 */
ADC_IRQState HAL_ADC_GetIRQState(ADC_Channel chan)
{
	ADC_ASSERT_CHANNEL(chan);

	if (HAL_GET_BIT(gADCPrivate.dataPending, HAL_BIT(chan)) && ADC_GetChanDataIRQ(chan)) {
		if (HAL_GET_BIT(gADCPrivate.lowPending, HAL_BIT(chan)) && ADC_GetChanLowIRQ(chan))
			return ADC_LOW_DATA_IRQ;
		if (HAL_GET_BIT(gADCPrivate.highPending, HAL_BIT(chan)) && ADC_GetChanHighIRQ(chan))
			return ADC_HIGH_DATA_IRQ;
		else
			return ADC_DATA_IRQ;
	} else {
		if (HAL_GET_BIT(gADCPrivate.lowPending, HAL_BIT(chan)) && ADC_GetChanLowIRQ(chan))
			return ADC_LOW_IRQ;
		if (HAL_GET_BIT(gADCPrivate.highPending, HAL_BIT(chan)) && ADC_GetChanHighIRQ(chan))
			return ADC_HIGH_IRQ;
		else
			return ADC_NO_IRQ;
	}
}

/**
 * @brief Get digital value of the specified ADC channel
 * @param[in] chan The specified ADC channel
 * @return Digital value converted by the specified ADC channel
 */
uint32_t HAL_ADC_GetValue(ADC_Channel chan)
{
	ADC_ASSERT_CHANNEL(chan);

	return ADC_GetValue(chan);
}

