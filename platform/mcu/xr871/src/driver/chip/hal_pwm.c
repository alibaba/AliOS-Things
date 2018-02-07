/**
  * @file  hal_pwm.c
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

#include "kernel/os/os.h"
#include "hal_base.h"
#include "driver/chip/hal_pwm.h"

#define DBG_PWM	0
#define PWM_DBG(fmt, arg...)	\
	HAL_LOG(HAL_DBG_ON && DBG_PWM, "[HAL PWM] "fmt, ##arg)


#define MAXCNTRVAL 65535
#define RISECH(ch)	(HAL_BIT(ch) * HAL_BIT(ch))
#define FALLCH(ch)	(HAL_BIT(ch) * HAL_BIT(ch) * 2)
#define PWM_IRQ_ALL_BITS	((1 << (PWM_CH_NUM << 1)) - 1)

static uint32_t Cap_priv = 0;
static uint8_t IoInitCount = 0;
static PWM_IrqParam PWM_IrqPrivate[8];

/**
  * @internal
  * @brief eanble pwm module.
  * @note This function is used to enable the pwm module source clock.
  * @param none.
  * @retval none.
  */
static void PWM_ModuleEnable()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_PWM);
	HAL_CCM_BusForcePeriphReset(CCM_BUS_PERIPH_BIT_PWM);
	HAL_CCM_BusReleasePeriphReset(CCM_BUS_PERIPH_BIT_PWM);
	HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_PWM);
}

/**
  * @internal
  * @brief disable pwm module.
  * @note This function is used to disable the pwm module source clock.
  * @param none.
  * @retval none.
  */
static void PWM_ModuleDisable()
{
	HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_PWM);
}

static void PWM_EnableModuleIRQ()
{
		HAL_NVIC_SetPriority(PWM_ECT_IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
		HAL_NVIC_EnableIRQ(PWM_ECT_IRQn);
}

static void PWM_DisableModuleIRQ()
{
		HAL_NVIC_DisableIRQ(PWM_ECT_IRQn);
}

static PWM_GROUP_ID PWM_ChToGroup(PWM_CH_ID ch_id)
{
	if (ch_id >= PWM_CH_NUM)
		return PWM_GROUP_NULL;

	return ch_id / 2;
}

static PWM_CH_ID PWM_GroupToCh0(PWM_GROUP_ID group_id)
{
	if (group_id >= PWM_GROUP_NUM)
		return PWM_GROUP_NULL;

	return group_id * 2;
}

static PWM_CH_ID PWM_GroupToCh1(PWM_GROUP_ID group_id)
{
	if (group_id >= PWM_GROUP_NUM)
		return PWM_GROUP_NULL;

	return group_id * 2 + 1;
}

static uint32_t PWM_ReadGroupClkFreq(PWM_GROUP_ID group_id)
{
	if (group_id >= PWM_GROUP_NUM)
		return 0;

	uint32_t src_clk_freq = 0;
	__IO uint32_t *reg = &PWM->PCCR[group_id];
	uint32_t clk = *reg & PWM_SRC_CLK_SELECT;
	uint32_t div = *reg & PWM_SRC_CLK_DIV_V;

	if (clk == 0) {
		src_clk_freq = HAL_GetHFClock();
		PWM_DBG("%s, %d HRCLK\n", __func__, __LINE__);
	} else {
		src_clk_freq =  HAL_GetAPBClock();
		PWM_DBG("%s, %d APBCLK\n", __func__, __LINE__);
	}

	src_clk_freq /= (1 << div);

	return src_clk_freq;
}


static int PWM_ChClkDiv(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	if (ch_id >= PWM_CH_NUM)
		return 0;

	int ch_clk_freq = 1;
	uint32_t minFreq = 0;
	uint8_t ch_div = 0;
	uint32_t temp1 = 0, temp2 = 0;
	__IO uint32_t *reg = NULL;
	uint32_t src_clk_freq = PWM_ReadGroupClkFreq(PWM_ChToGroup(ch_id));

	PWM_DBG("SRC_CLK freq = %d\n", src_clk_freq);

	if ((src_clk_freq % MAXCNTRVAL) > 0)
		temp1 = 1;
	if (((src_clk_freq + temp1) % 256) > 0)
		temp2 = 1;

	minFreq = (src_clk_freq / MAXCNTRVAL + temp1)/ 256 + temp2;

	if (param->hz > src_clk_freq || param->hz < minFreq)
		ch_clk_freq = 0;

	if (param->hz > (src_clk_freq / MAXCNTRVAL + temp1))
		ch_div = 0;
	else {
		ch_div =  (src_clk_freq / MAXCNTRVAL + temp1) % param->hz;
		if (ch_div)
			ch_div =  (src_clk_freq / MAXCNTRVAL + temp1) / param->hz;
		else
			ch_div =  (src_clk_freq / MAXCNTRVAL + temp1) / param->hz - 1;
	}


	PWM_DBG("ch div = %d\n", ch_div);

	reg = &PWM->CH_REG[ch_id].PCR;

	if (ch_div > 255)
		return 0;

	uint32_t temp = *reg;
	temp &= ~PWM_PCR_PRESCAL;
	temp |= ch_div;
	*reg = temp;						//Source clock Frequency Division

	if (ch_clk_freq != 0)
		ch_clk_freq = src_clk_freq / (ch_div + 1);

	return ch_clk_freq;
}


static void PWM_ChSetPolarity(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	if (ch_id >= PWM_CH_NUM)
		return;

	__IO uint32_t *reg = NULL;

	reg = &PWM->CH_REG[ch_id].PCR;

	if (param->polarity == PWM_LOWLEVE)
		HAL_CLR_BIT(*reg, PWM_PCR_ACT_STA);
	else if (param->polarity == PWM_HIGHLEVE)
		HAL_SET_BIT(*reg, PWM_PCR_ACT_STA);
}

static void PWM_ChSetMode(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	if (ch_id >= PWM_CH_NUM)
		return;

	__IO uint32_t *reg = NULL;
	reg = &PWM->CH_REG[ch_id].PCR;

	if (param->mode == PWM_PLUSE_MODE)
		HAL_SET_BIT(*reg, PWM_PCR_MODE);
	else if (param->mode == PWM_CYCLE_MODE)
		HAL_CLR_BIT(*reg, PWM_PCR_MODE);
}

static int PWM_OutPeriodRady(PWM_CH_ID ch_id)
{
	if (ch_id >= PWM_CH_NUM)
		return -1;
	__IO uint32_t *reg = NULL;
	reg = &PWM->CH_REG[ch_id].PCR;

	return HAL_GET_BIT(*reg, PWM_PCR_PERIODRDY);

}

static void PWM_OutSetCycle(PWM_CH_ID ch_id, uint16_t value)
{

	if (ch_id >= PWM_CH_NUM)
		return;

	__IO uint32_t *reg = &PWM->CH_REG[ch_id].PPR;;

	uint32_t temp = *reg;
	temp &= ~PWM_PPR_ENTIER_CYCLE;
	temp |= (value << 16);
	*reg = temp;
}

static int PWM_OutModeInit(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	int ch_cycle_value = 0;

	ch_cycle_value = PWM_ChClkDiv(ch_id, param);

	PWM_DBG("ch freq = %d\n", ch_cycle_value);

	if (ch_cycle_value == 0)
		return -1;

	if (param->hz > (ch_cycle_value / 2))
		return -1;

	ch_cycle_value /= param->hz;

	PWM_DBG("ch_cycle_value = %d\n", ch_cycle_value);

	PWM_ChSetPolarity(ch_id, param);
	PWM_ChSetMode(ch_id, param);

	while (PWM_OutPeriodRady(ch_id) == 1)
		OS_MSleep(10);

	PWM_OutSetCycle(ch_id, ch_cycle_value - 1);

	return ch_cycle_value;
}

static int PWM_InputInit(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	int clk_freq =  PWM_ChClkDiv(ch_id, param);

	if (clk_freq == 0) {
		HAL_ERR("Hz out of range\n");
		return -1;
	}

	PWM_ChSetPolarity(ch_id, param);
	PWM_ChSetMode(ch_id, param);

	return 0;
}

static int PWM_EnableClock(PWM_CH_ID ch_id, uint8_t en)
{
	PWM_GROUP_ID group = PWM_ChToGroup(ch_id);

	__IO uint32_t *reg = NULL;

	if (group >= PWM_GROUP_NUM)
		return -1;

	reg = &PWM->PCCR[group];

	if (en == 1)
		HAL_SET_BIT(*reg, PWM_CH_CLK_GATING);
	else
		HAL_CLR_BIT(*reg, PWM_CH_CLK_GATING);

	return 0;
}

static void PWM_Init(PWM_CH_ID ch_id)
{
	int is_init = IoInitCount & (1 << ch_id);

	if (!is_init) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch_id), 0);
		IoInitCount |= 1 << ch_id;
	}
}

static void PWM_DeInit(PWM_CH_ID ch_id)
{


	int is_init = IoInitCount & (1 << ch_id);
	PWM_CH_ID brother;

	if (ch_id % 2)
		brother = ch_id - 1;
	else
		brother = ch_id + 1;

	int brother_is_init = IoInitCount & (1 << brother);

	if(is_init) {
		HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_PWM, ch_id), 0);

		if (!brother_is_init)
			PWM_EnableClock(ch_id, 0);

		IoInitCount &= ~(1 << ch_id);

		if (IoInitCount == 0) {
			PWM_DBG("pwm module disable\n");
			PWM_ModuleDisable();
		}
	}
}

__STATIC_INLINE void PWM_EnableOutputIRQ(PWM_CH_ID ch_id, uint8_t en)
{
	if (en)
		HAL_SET_BIT(PWM->PIER, HAL_BIT(ch_id));
	else
		HAL_CLR_BIT(PWM->PIER, HAL_BIT(ch_id));
}

__STATIC_INLINE void PWM_EnableInuptRiseEdgeIRQ(PWM_CH_ID ch_id, uint8_t en)
{
	if (en)
		HAL_SET_BIT(PWM->CIER,  RISECH(ch_id));
	else
		HAL_CLR_BIT(PWM->CIER,  RISECH(ch_id));
}

__STATIC_INLINE void PWM_EnableInuputFallEdgeIRQ(PWM_CH_ID ch_id, uint8_t en)
{
	if (en)
		HAL_SET_BIT(PWM->CIER, FALLCH(ch_id));
	else
		HAL_CLR_BIT(PWM->CIER, FALLCH(ch_id));
}

__STATIC_INLINE void PWM_EnableInputBothEdgeIRQ(PWM_CH_ID ch_id, uint8_t en)
{
	if (en)
		HAL_SET_BIT(PWM->CIER, (RISECH(ch_id) | FALLCH(ch_id)));
	else
		HAL_CLR_BIT(PWM->CIER, (RISECH(ch_id) | FALLCH(ch_id)));
}

static uint32_t PWM_CycleValue(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg;

	reg = &PWM->CH_REG[ch_id].PPR;

	return (*reg & PWM_PPR_ENTIER_CYCLE) >> 16;
}

static int PWM_CycleIsReady(PWM_CH_ID ch_id)
{
	__IO uint32_t* reg;

	reg = &PWM->CH_REG[ch_id].PCR;

	return HAL_GET_BIT(*reg, PWM_PCR_PERIODRDY);
}

static void PWM_SetActCycle(PWM_CH_ID ch_id, uint16_t cycle)
{
	__IO uint32_t *reg;
	reg = &PWM->CH_REG[ch_id].PPR;

	uint32_t p = *reg;
	p &= ~PWM_PPR_ACT_CYCLE;
	p |= cycle;
	*reg = p;
}

static void PWM_EnableDeadZone(PWM_GROUP_ID group_id, uint8_t en)
{
	__IO uint32_t *reg = NULL;

	reg = &PWM->PDZCR[group_id];

	if (en)
		HAL_SET_BIT(*reg, PWM_CH_DZ_EN);
	else
		HAL_CLR_BIT(*reg, PWM_CH_DZ_EN);
}

static int PWM_FallEdgeLock(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CCR;;

	return *reg & PWM_CCR_CFLF;
}

static void PWM_ClearFallEdgeLock(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CCR;

	*reg |= PWM_CCR_CFLF;
}

static int PWM_RiseEdgeLock(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CCR;

	return *reg & PWM_CCR_CRLF;
}

static void PWM_ClearRiseEdgeLock(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CCR;
	if(reg == NULL)
		return;
	*reg |= PWM_CCR_CRLF;
}

static uint16_t PWM_CRLRValue(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CRLR;

	return ((uint16_t)(*reg & PWM_CRLR));
}

static uint16_t PWM_CFLRValue(PWM_CH_ID ch_id)
{
	__IO uint32_t *reg = &PWM->CH_REG[ch_id].CFLR;

	return ((uint16_t)(*reg & PWM_CFLR));
}

static void PWM_OutIRQHandle()
{
	uint32_t i;
	uint32_t status;
	uint32_t is_irq;
	PWM_IrqParam *irq;

	status =  PWM->PISR & PWM->PIER & PWM_IRQ_ALL_BITS; /* get pending bits */
	PWM->PISR = status; /* clear IRQ bits */
	irq = PWM_IrqPrivate;

	for (i = PWM_GROUP0_CH0; i < PWM_CH_NUM && status != 0; i++) {
		is_irq = status & HAL_BIT(0);
		if (is_irq && irq[i].callback) {
			PWM_IrqEvent event = PWM_IRQ_OUTPUT;
			irq[i].callback(irq[i].arg, event);
		}
		status >>= 1;
	}
}

static void PWM_InputIRQHandle()
{
	uint32_t i;
	uint32_t status;
	uint32_t rise_edge;
	uint32_t fall_edge;
	PWM_IrqParam *irq;
	status =  PWM->CISR & PWM->CIER & PWM_IRQ_ALL_BITS; /* get pending bits */
	PWM->CISR = status; /* clear IRQ bits */
	irq = PWM_IrqPrivate;

	for (i = PWM_GROUP0_CH0; i < PWM_CH_NUM && status != 0; i++) {
		rise_edge = status & HAL_BIT(0);
		if (rise_edge && irq[i].callback) {
			PWM_IrqEvent event = PWM_IRQ_RISEEDGE;
			irq[i].callback(irq[i].arg, event);
		}
		fall_edge = status & HAL_BIT(1);
		if (fall_edge && irq[i].callback) {
			PWM_IrqEvent event = PWM_IRQ_FALLEDGE;
			irq[i].callback(irq[i].arg, event);
		}
		status >>= 2;
	}
}

void PWM_ECT_IRQHandler()
{
	if (PWM->CIER > 0)
		PWM_InputIRQHandle();

	if (PWM->PIER > 0)
		PWM_OutIRQHandle();
}

/**
  * @brief Configure the pwm group source clock.
  * @note This function is used to configure the group source clock ,
  *           each group contains two channels.
  *           For example : The group_0's clock is effect channel0 and channel 1.
  * @param group_id: The pwm group id.
  * @param param:
  *        @arg param->clk:The source clk of this group.
  *        @arg param->div:The division for source clk.
  * @retval HAL_Status:  The status of driver
  */
HAL_Status HAL_PWM_GroupClkCfg(PWM_GROUP_ID group_id, PWM_ClkParam *param)
{
	if (group_id >= PWM_GROUP_NUM)
		return HAL_ERROR;

	if (IoInitCount == 0) {
		PWM_ModuleEnable();
	}

	__IO uint32_t *reg = &PWM->PCCR[group_id];

	if (param->clk == PWM_CLK_HOSC) {
		*reg &= ~PWM_SRC_CLK_SELECT;
	} else if (param->clk == PWM_CLK_APB1){
		HAL_SET_BIT(*reg, PWM_SRC_CLK_SELECT);
		HAL_CLR_BIT(*reg, HAL_BIT(8));
	} else
		return HAL_ERROR;

	if (param->div > PWM_SRC_CLK_DIV_256 || param->div < 0)
		return HAL_ERROR;

	uint32_t temp = *reg;
	temp &= ~PWM_SRC_CLK_DIV_V;
	temp |= param->div;
	*reg = temp;

	return HAL_OK;
}

/**
  * @brief Init the pwm channel.
  * @note This function is used to configure the channel pin(IO mode), output frequency ,
  *           polarity and rum mode.
  * @param ch_id: The pwm channel id.
  * @param param:
  *        @arg param->mode:The channels run mode.
  *        @arg param->polarity:The channels polarity.
  *        @arg param->hz:The channels output frequency.
  * @retval max_duty_ratio:  The channel max duty cycle value. if the mode is
  *        capture, the return value is 0. error return -1;
  */
int HAL_PWM_ChInit(PWM_CH_ID ch_id, PWM_ChInitParam *param)
{
	int duty_ratio = -1;
	if (ch_id >= PWM_CH_NUM)
		return -1;

	PWM_Init(ch_id);

	switch(param->mode) {
		case PWM_CYCLE_MODE:
		case PWM_PLUSE_MODE:
			duty_ratio = PWM_OutModeInit(ch_id, param);
			break;
		case PWM_CAPTURE_MODE:
			duty_ratio = PWM_InputInit(ch_id, param);
			break;
		default:
			return -1;
	}

	return duty_ratio;
}

/**
  * @brief Deinit the pwm channel.
  * @note This function is used to deinit the channel pin, reduce power consumption.
  * @param ch_id: The pwm channel id.
  * @retval HAL_Status:  The status of driver.
  */
HAL_Status HAL_PWM_ChDeinit(PWM_CH_ID ch_id)
{
	if (ch_id >= PWM_CH_NUM)
		return HAL_ERROR;

	PWM_DeInit(ch_id);

	return HAL_OK;
}

/**
  * @brief Init the complementary mode.
  * @note This function is used to configure the groups pin(IO mode), output frequency ,
  *           polarity.
  *           This mode is two channel output complementart waveform, the channel run in
  *           cycle mode.
  * @param group_id: The pwm group id.
  * @param param:
  *        @arg param->polarity:The channels polarity.
  *        @arg param->hz:The channels output frequency.
  * @retval HAL_Status:  The channel max duty cycle value.
  */
int HAL_PWM_ComplementaryInit(PWM_GROUP_ID group_id, PWM_CompInitParam *param)
{
	if (group_id >= PWM_GROUP_NUM)
		return -1;

	int cycle_value = -1;
	PWM_CH_ID ch_low = group_id * 2;
	PWM_CH_ID ch_high = ch_low + 1;
	PWM_ChInitParam ch_param;

	PWM_Init(ch_low);
	PWM_Init(ch_high);

	ch_param.hz = param->hz;
	ch_param.polarity = param->polarity;
	ch_param.mode = PWM_CYCLE_MODE;

	cycle_value = PWM_OutModeInit(ch_low, &ch_param);
	if (cycle_value == -1)
		return -1;

	if (ch_param.polarity == PWM_LOWLEVE)
		ch_param.polarity = PWM_HIGHLEVE;
	else if (ch_param.polarity == PWM_HIGHLEVE)
		ch_param.polarity = PWM_LOWLEVE;
	else
		return -1;

	PWM_OutModeInit(ch_high, &ch_param);

	return cycle_value;
}

/**
  * @brief Deinit the pwm complementary.
  * @note This function is used to deinit the groupl pin, reduce power consumption.
  * @param group_id: The pwm group id.
  * @retval HAL_Status:  The status of driver.
  */
HAL_Status HAL_PWM_ComplementaryDeInit(PWM_GROUP_ID group_id)
{
	if (group_id >= PWM_GROUP_NUM)
		return HAL_ERROR;

	PWM_CH_ID ch_low = group_id * 2;
	PWM_CH_ID ch_high = ch_low + 1;

	PWM_DeInit(ch_low);
	PWM_DeInit(ch_high);

	return HAL_OK;
}


/**
  * @brief Enable the pwm channel.
  * @param ch_id: The pwm channel id.
  * @param mode: The pwm channel run mode.
  * @param en: set 1 enable , 0 disable.
  * @retval HAL_Status:  The status of driver.
  */
HAL_Status HAL_PWM_EnableCh(PWM_CH_ID ch_id, PWM_Mode mode, uint8_t en)
{
	if (ch_id >= PWM_CH_NUM)
		return HAL_ERROR;

	PWM_EnableClock(ch_id, en);

	if (en) {
		if (mode == PWM_CAPTURE_MODE) {
			HAL_CLR_BIT(PWM->PER, HAL_BIT(ch_id));
			HAL_SET_BIT(PWM->CER, HAL_BIT(ch_id));
		} else {
			HAL_CLR_BIT(PWM->CER, HAL_BIT(ch_id));
			HAL_SET_BIT(PWM->PER, HAL_BIT(ch_id));
		}
	} else {
		HAL_CLR_BIT(PWM->CER, HAL_BIT(ch_id));
		HAL_CLR_BIT(PWM->PER, HAL_BIT(ch_id));
	}

	return HAL_OK;

}


/**
  * @brief Output one pluse.Use this function you need already enable the channel.
  * @param ch_id: The pwm channel id.
  * @retval HAL_Status:  The status of driver.
  */
HAL_Status HAL_PWM_OutputPluse(PWM_CH_ID ch_id)
{
	__IO uint32_t* reg = NULL;
	reg = &PWM->CH_REG[ch_id].PCR;

	if ((*reg & PWM_PCR_PLUSE_START) > 0)
		return HAL_BUSY;

	HAL_SET_BIT(*reg, PWM_PCR_PLUSE_START);

	return HAL_OK;
}

/**
  * @brief Enable the complementary.
  * @param group_id: The pwm group id.
  * @param en: set 1 enable , 0 disable.
  * @retval HAL_Status:  The status of driver.
  */
HAL_Status HAL_PWM_EnableComplementary(PWM_GROUP_ID group_id, uint8_t en)
{
	if (group_id >= PWM_GROUP_NUM)
		return HAL_ERROR;

	PWM_CH_ID ch_id_0 = PWM_GroupToCh0(group_id);
	PWM_CH_ID ch_id_1 = PWM_GroupToCh1(group_id);
	PWM_EnableClock(ch_id_0, en);
	PWM_EnableClock(ch_id_1, en);
	if (en) {
		HAL_CLR_BIT(PWM->CER, HAL_BIT(ch_id_0) | HAL_BIT(ch_id_1));
		HAL_SET_BIT(PWM->PER, HAL_BIT(ch_id_0) | HAL_BIT(ch_id_1));
	} else
		HAL_CLR_BIT(PWM->PER, HAL_BIT(ch_id_0) | HAL_BIT(ch_id_1));

	return HAL_OK;
}

/**
  * @brief Enable the pwm channel's interrupt.
  * @param ch_id: The pwm channel id.
  * @param param:
  *        @arg param->event:The interrupt event.
  *        @arg param->callback:The interrupt callback.
  *        @arg param->arg: The param for callback.
  * @retval HAL_Status:   The status of driver.
  */
HAL_Status HAL_PWM_EnableIRQ(PWM_CH_ID ch_id, const PWM_IrqParam *param)
{

	if (PWM->CIER == 0 &&  PWM->PIER == 0)
		PWM_EnableModuleIRQ();

	switch (param->event) {
		case PWM_IRQ_OUTPUT:
			PWM_EnableOutputIRQ(ch_id, 1);
			break;
		case PWM_IRQ_RISEEDGE:
			PWM_EnableInuptRiseEdgeIRQ(ch_id, 1);
			break;
		case PWM_IRQ_FALLEDGE:
			PWM_EnableInuputFallEdgeIRQ(ch_id, 1);
			break;
		case PWM_IRQ_BOTHEDGE:
			PWM_EnableInputBothEdgeIRQ(ch_id, 1);
			break;
		case PWM_IRQ_NULL:
			break;
		default:
			HAL_WRN("PWM invalid IRQ event, event: %d\n", param->event);
			return HAL_ERROR;
	}

	PWM_IrqPrivate[ch_id] = *param;

	return HAL_OK;
}


/**
  * @brief Disable the pwm channel's interrupt.
  * @param ch_id: The pwm channel id.
  * @retval HAL_Status:   The status of driver.
  */
void HAL_PWM_DisableIRQ(PWM_CH_ID ch_id)
{
	PWM_EnableOutputIRQ(ch_id, 0);
	PWM_EnableInputBothEdgeIRQ(ch_id, 0);

	if (PWM->CIER == 0 &&  PWM->PIER == 0)
		PWM_DisableModuleIRQ();
}


/**
  * @brief Set channel's duty ratio.
  * @param ch_id: The pwm channel id.
  * @param value: The duty ratio value.
  * @retval HAL_Status:   The status of driver.
  */
HAL_Status HAL_PWM_ChSetDutyRatio(PWM_CH_ID ch_id, uint16_t value)
{

	if (ch_id >= PWM_CH_NUM) {
		HAL_ERR("Invalid ch_id\n");
		return HAL_ERROR;
	}

	if (value > PWM_CycleValue(ch_id)) {
		HAL_ERR("value is out of range , should <= %d\n",PWM_CycleValue(ch_id));
		return HAL_ERROR;
	}

	while (PWM_CycleIsReady(ch_id) == 1)
		OS_MSleep(10);

	PWM_SetActCycle(ch_id, value);

	return HAL_OK;
}

/**
  * @brief Set Complementary duty ratio.
  * @param group_id: The pwm group id.
  * @param value: The duty ratio value.
  * @retval HAL_Status:   The status of driver.
  */
HAL_Status HAL_PWM_ComplementarySetDutyRatio(PWM_GROUP_ID group_id, uint16_t value)
{
	PWM_CH_ID ch_0, ch_1;

	ch_0 = PWM_GroupToCh0(group_id);
	ch_1 = PWM_GroupToCh1(group_id);

	if (HAL_PWM_ChSetDutyRatio(ch_0, value) != HAL_OK)
		return HAL_ERROR;
	if (HAL_PWM_ChSetDutyRatio(ch_1, value) != HAL_OK)
		return HAL_ERROR;

	return HAL_OK;
}

/**
  * @brief Set dead zone for complementary mode.
  * @param group_id: The pwm group id.
  * @param value: The dead zone value.
  * @retval HAL_Status:   The status of driver.
  */
HAL_Status HAL_PWM_SetDeadZoneTime(PWM_GROUP_ID group_id, uint8_t dead_zone_value)
{
	__IO uint32_t *reg = NULL;

	if (group_id >= PWM_GROUP_NUM) {
		HAL_ERR("Invalid group id\n");
		return HAL_ERROR;
	}

	reg = &PWM->PDZCR[group_id];
	uint32_t p = *reg;

	p &= ~PWM_CH_DZ_INV;
	p |= (dead_zone_value << 8);
	*reg = p;

	return HAL_OK;
}

/**
  * @brief Enable the dead zone.
  * @param group_id: The pwm group id.
  * @param en: set 1 enable, 0 disable.
  * @retval HAL_Status:   The status of driver.
  */
HAL_Status HAL_PWM_EnableDeadZone(PWM_GROUP_ID group_id, uint8_t en)
{
	if (group_id >= PWM_GROUP_NUM) {
		HAL_ERR("Invalid group id\n");
		return HAL_ERROR;
	}
	if (en)
		PWM_EnableDeadZone(group_id, 1);
	else
		PWM_EnableDeadZone(group_id, 0);

	return HAL_OK;
}

/**
  * @brief The pwm channel capture result.
  * @param mode:
  *        @arg PWM_CAP_PLUSE: used for capture pluse. Run in the mode,
  *                only the highLevlTime is valid.
  *        @arg PWM_CAP_CYCLE: used for cycle signal. capture the signal's period,
  *                high level time and low level time.
  * @param ch_id: The pwm channel id.
  * @retval PWM_CapResult:   The result of capture.
  */
PWM_CapResult HAL_PWM_CaptureResult(PWM_CaptureMode mode, PWM_CH_ID ch_id)
{
	PWM_CapResult result = {0, 0, 0};
	uint16_t fall_time = 0, rise_time = 0;
	uint32_t temp = 0;

	switch (mode) {
		case PWM_CAP_PLUSE:
			if (PWM_FallEdgeLock(ch_id) && PWM_RiseEdgeLock(ch_id)) {
				PWM_ClearFallEdgeLock(ch_id);
				PWM_ClearRiseEdgeLock(ch_id);
				fall_time = PWM_CRLRValue(ch_id);
				rise_time = PWM_CFLRValue(ch_id);

				result.highLevelTime = rise_time ;
				result.lowLevelTime = 0 ;
				result.periodTime = 0;
			}
			break;
		case PWM_CAP_CYCLE:
			if (PWM_RiseEdgeLock(ch_id)) {
				temp = Cap_priv & (1 << ch_id);
				if (temp) {
					PWM_ClearFallEdgeLock(ch_id);
					PWM_ClearRiseEdgeLock(ch_id);
					fall_time = PWM_CRLRValue(ch_id);
					rise_time = PWM_CFLRValue(ch_id);

					result.highLevelTime = rise_time ;
					result.lowLevelTime = fall_time ;
					result.periodTime = rise_time + fall_time;
					Cap_priv &= ~(1 << ch_id);
				} else {
					PWM_ClearRiseEdgeLock(ch_id);
					Cap_priv |= (1 << ch_id);
				}
			}
			break;
		default :
			break;
	}

	return result;
}
