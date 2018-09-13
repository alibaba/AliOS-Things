/**
  * @file  hal_gpio.c
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

#include "driver/chip/hal_gpio.h"
#include "hal_base.h"

/*
 * register bits of GPIO_CTRL_T for each GPIO pin
 */
#define GPIO_CTRL_MODE_BITS		4
#define GPIO_CTRL_MODE_MASK		0xFU
#define GPIO_CTRL_MODE_MAX		GPIOx_Pn_F7_DISABLE

#define GPIO_CTRL_DATA_BITS		1

#define GPIO_CTRL_DRIVING_BITS	2
#define GPIO_CTRL_DRIVING_MASK	0x3U
#define GPIO_CTRL_DRIVING_MAX	GPIO_DRIVING_LEVEL_3

#define GPIO_CTRL_PULL_BITS		2
#define GPIO_CTRL_PULL_MASK		0x3U
#define GPIO_CTRL_PULL_MAX		GPIO_PULL_DOWN

/*
 * register bits of GPIO_IRQ_T for each GPIO pin
 */
#define GPIO_IRQ_EVT_BITS		4
#define GPIO_IRQ_EVT_MASK		0xFU
#define GPIO_IRQ_EVT_MAX		GPIO_IRQ_EVT_BOTH_EDGE

#define GPIO_IRQ_EN_BITS		1

#define GPIO_IRQ_STAUTS_BITS	1

/* useful macros */
#define GPIO_PINS_MASK(pinNum)	((1U << pinNum) - 1)

#define GPIO_REG_BITS			32
#define GPIO_GET_REG_IDX_SHIFT(idx, shift, pin, bits)	\
	do {												\
		(shift) = (pin) * (bits);						\
		(idx) = (shift) / GPIO_REG_BITS;				\
		(shift) = (shift) % GPIO_REG_BITS;				\
	} while (0)

typedef struct {
	GPIO_IRQCallback	callback;
	void			   *arg;
} GPIO_Private;

static GPIO_Private gGPIOAPrivate[GPIOA_PIN_NUM];
static GPIO_Private gGPIOBPrivate[GPIOB_PIN_NUM];

static uint8_t gGPIOUsedCnt = 0;

#define GPIOA_CTRL	((GPIO_CTRL_T *)GPIOA_CTRL_BASE)
#define GPIOB_CTRL	((GPIO_CTRL_T *)GPIOB_CTRL_BASE)

#define GPIOA_IRQ	((GPIO_IRQ_T *)GPIOA_IRQ_BASE)
#define GPIOB_IRQ	((GPIO_IRQ_T *)GPIOB_IRQ_BASE)

static GPIO_CTRL_T * const gGPIOPortCtrl[GPIO_PORT_NUM] = { GPIOA_CTRL, GPIOB_CTRL };
static GPIO_IRQ_T * const gGPIOPortIRQ[GPIO_PORT_NUM] = { GPIOA_IRQ, GPIOB_IRQ };


__STATIC_INLINE GPIO_CTRL_T *GPIO_GetCtrlInstance(GPIO_Port port)
{
	return gGPIOPortCtrl[port];
}

__STATIC_INLINE GPIO_IRQ_T *GPIO_GetIRQInstance(GPIO_Port port)
{
	return gGPIOPortIRQ[port];
}

/*
 * IRQ handling
 */
static void GPIO_IRQHandler(GPIO_IRQ_T *gpiox, uint32_t pinNum, uint32_t pinMask, GPIO_Private *priv)
{
	uint32_t i;
	uint32_t irqStatus;
	uint32_t isPending;

	irqStatus = gpiox->IRQ_STATUS & gpiox->IRQ_EN & pinMask; /* get pending bits */
	gpiox->IRQ_STATUS = irqStatus; /* clear pending bits */

	for (i = GPIO_PIN_0; i < pinNum && irqStatus != 0; ++i) {
		isPending = irqStatus & HAL_BIT(0);
		if (isPending && priv[i].callback) {
			priv[i].callback(priv[i].arg);
		}
		irqStatus >>= 1;
	}
}

void GPIOA_IRQHandler(void)
{
	GPIO_IRQHandler(GPIOA_IRQ, GPIOA_PIN_NUM, GPIO_PINS_MASK(GPIOA_PIN_NUM), gGPIOAPrivate);
}

void GPIOB_IRQHandler(void)
{
	GPIO_IRQHandler(GPIOB_IRQ, GPIOB_PIN_NUM, GPIO_PINS_MASK(GPIOB_PIN_NUM), gGPIOBPrivate);
}

static void GPIO_EnableIRQ(GPIO_IRQ_T *gpiox, GPIO_Pin pin)
{
	HAL_SET_BIT(gpiox->IRQ_EN, HAL_BIT(pin));
}

static void GPIO_DisableIRQ(GPIO_IRQ_T *gpiox, GPIO_Pin pin)
{
	HAL_CLR_BIT(gpiox->IRQ_EN, HAL_BIT(pin));
}

static int GPIO_IsPendingIRQ(GPIO_IRQ_T *gpiox, GPIO_Pin pin)
{
	return HAL_GET_BIT_VAL(gpiox->IRQ_STATUS, pin, 1);
}

static void GPIO_ClearPendingIRQ(GPIO_IRQ_T *gpiox, GPIO_Pin pin)
{
	HAL_SET_BIT(gpiox->IRQ_STATUS, HAL_BIT(pin));
}

void HAL_GPIO_EnableIRQ(GPIO_Port port, GPIO_Pin pin, const GPIO_IrqParam *param)
{
	uint32_t regIdx;
	uint32_t bitShift;
	GPIO_IRQ_T *gpiox;
	GPIO_Private *gpioPriv;
	IRQn_Type IRQn;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();

	if (port == GPIO_PORT_A) {
		gpioPriv = gGPIOAPrivate;
		IRQn = GPIOA_IRQn;
	} else if (port == GPIO_PORT_B) {
		gpioPriv = gGPIOBPrivate;
		IRQn = GPIOB_IRQn;
	} else {
		HAL_ERR("Invalid port %d for IRQ\n", port);
		return;
	}
	gpiox = GPIO_GetIRQInstance(port);

	/* set callback */
	gpioPriv[pin].callback = param->callback;
	gpioPriv[pin].arg = param->arg;

	/* set IRQ trigger mode */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_IRQ_EVT_BITS);
	HAL_MODIFY_REG(gpiox->IRQ_MODE[regIdx],
				   GPIO_IRQ_EVT_MASK << bitShift,
				   (param->event & GPIO_IRQ_EVT_MASK) << bitShift);

	if (GPIO_IsPendingIRQ(gpiox, pin)) {
		GPIO_ClearPendingIRQ(gpiox, pin);
	}
	GPIO_EnableIRQ(gpiox, pin);
	HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_EnableIRQ(IRQn);

	HAL_ExitCriticalSection(flags);
}

void HAL_GPIO_DisableIRQ(GPIO_Port port, GPIO_Pin pin)
{
	GPIO_IRQ_T *gpiox;
	GPIO_Private *gpioPriv;
	IRQn_Type IRQn;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();

	if (port == GPIO_PORT_A) {
		gpioPriv = gGPIOAPrivate;
		IRQn = GPIOA_IRQn;
	} else if (port == GPIO_PORT_B) {
		gpioPriv = gGPIOBPrivate;
		IRQn = GPIOB_IRQn;
	} else {
		HAL_ERR("Invalid port %d for IRQ\n", port);
		return;
	}
	gpiox = GPIO_GetIRQInstance(port);

	HAL_NVIC_DisableIRQ(IRQn);
	GPIO_DisableIRQ(gpiox, pin);
	if (GPIO_IsPendingIRQ(gpiox, pin)) {
		GPIO_ClearPendingIRQ(gpiox, pin);
	}

	gpioPriv[pin].callback = NULL;
	gpioPriv[pin].arg = NULL;

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Initialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] param Pointer to GPIO_InitParam structure
 * @return None
 */
void HAL_GPIO_Init(GPIO_Port port, GPIO_Pin pin, const GPIO_InitParam *param)
{
	uint32_t regIdx;
	uint32_t bitShift;
	GPIO_CTRL_T *gpiox;
	unsigned long flags;

#if 0
	HAL_ASSERT_PARAM(pin <= GPIO_PIN_MAX);
	HAL_ASSERT_PARAM(param->mode <= GPIO_CTRL_MODE_MAX);
	HAL_ASSERT_PARAM(param->driving <= GPIO_CTRL_DRIVING_MAX);
	HAL_ASSERT_PARAM(param->pull <= GPIO_CTRL_PULL_MAX);
#endif

	flags = HAL_EnterCriticalSection();

	if (gGPIOUsedCnt++ == 0) {
		HAL_CCM_BusEnablePeriphClock(CCM_BUS_PERIPH_BIT_GPIO);
	}

	gpiox = GPIO_GetCtrlInstance(port);

	/* set working mode (function) */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_MODE_BITS);
	HAL_MODIFY_REG(gpiox->MODE[regIdx],
				   GPIO_CTRL_MODE_MASK << bitShift,
				   (param->mode & GPIO_CTRL_MODE_MASK) << bitShift);

	/* set driving */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_DRIVING_BITS);
	HAL_MODIFY_REG(gpiox->DRIVING[regIdx],
				   GPIO_CTRL_DRIVING_MASK << bitShift,
				   (param->driving & GPIO_CTRL_DRIVING_MASK) << bitShift);

	/* set pull */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_PULL_BITS);
	HAL_MODIFY_REG(gpiox->PULL[regIdx],
				   GPIO_CTRL_PULL_MASK << bitShift,
				   (param->pull & GPIO_CTRL_PULL_MASK) << bitShift);

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Deinitialize the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @return None
 * @note After deinitialization, the GPIO is in its reset state:
 *       (GPIOx_Pn_F7_DISABLE, GPIO_DRIVING_LEVEL_1, GPIO_PULL_NONE).
 */
void HAL_GPIO_DeInit(GPIO_Port port, GPIO_Pin pin)
{
	uint32_t regIdx;
	uint32_t bitShift;
	GPIO_CTRL_T *gpiox;
	unsigned long flags;

	flags = HAL_EnterCriticalSection();

	gpiox = GPIO_GetCtrlInstance(port);

	/* set working mode (function) to disable */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_MODE_BITS);
	HAL_MODIFY_REG(gpiox->MODE[regIdx],
				   GPIO_CTRL_MODE_MASK << bitShift,
				   (GPIOx_Pn_F7_DISABLE & GPIO_CTRL_MODE_MASK) << bitShift);

	/* set driving to default value (GPIO_DRIVING_LEVEL_1) */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_DRIVING_BITS);
	HAL_MODIFY_REG(gpiox->DRIVING[regIdx],
				   GPIO_CTRL_DRIVING_MASK << bitShift,
				   (GPIO_DRIVING_LEVEL_1 & GPIO_CTRL_DRIVING_MASK) << bitShift);

	/* set pull to default value (GPIO_PULL_NONE) */
	GPIO_GET_REG_IDX_SHIFT(regIdx, bitShift, pin, GPIO_CTRL_PULL_BITS);
	HAL_MODIFY_REG(gpiox->PULL[regIdx],
				   GPIO_CTRL_PULL_MASK << bitShift,
				   (GPIO_PULL_NONE & GPIO_CTRL_PULL_MASK) << bitShift);

	if ((gGPIOUsedCnt > 0) && (--gGPIOUsedCnt == 0)) {
		HAL_CCM_BusDisablePeriphClock(CCM_BUS_PERIPH_BIT_GPIO);
	}

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Set the state of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @param[in] state GPIO pin state
 * @return None
 */
void HAL_GPIO_WritePin(GPIO_Port port, GPIO_Pin pin, GPIO_PinState state)
{
	GPIO_CTRL_T *gpiox;
	unsigned long flags;

	gpiox = GPIO_GetCtrlInstance(port);

	flags = HAL_EnterCriticalSection();

	if (state == GPIO_PIN_LOW)
		HAL_CLR_BIT(gpiox->DATA, HAL_BIT(pin));
	else
		HAL_SET_BIT(gpiox->DATA, HAL_BIT(pin));

	HAL_ExitCriticalSection(flags);
}

/**
 * @brief Get the state of the specified GPIO
 * @param[in] port GPIO port
 * @param[in] pin GPIO pin number
 * @return GPIO pin state
 */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_Port port, GPIO_Pin pin)
{
	GPIO_CTRL_T *gpiox;

	gpiox = GPIO_GetCtrlInstance(port);
	return (GPIO_PinState)HAL_GET_BIT_VAL(gpiox->DATA, pin, 1);
}

/**
 * @brief Set the state of the specified GPIO port
 * @param[in] port GPIO port
 * @param[in] portMask GPIO port state, bit mask of all pins
 * @return None
 */
void HAL_GPIO_WritePort(GPIO_Port port, uint32_t portMask)
{
	GPIO_CTRL_T *gpiox;

	gpiox = GPIO_GetCtrlInstance(port);
	gpiox->DATA = portMask;
}

/**
 * @brief Get the state of the specified GPIO port
 * @param[in] port GPIO port
 * @return GPIO port state, bit mask of all pins
 */
uint32_t HAL_GPIO_ReadPort(GPIO_Port port)
{
	GPIO_CTRL_T *gpiox;

	gpiox = GPIO_GetCtrlInstance(port);
	return gpiox->DATA;
}

/**
 * @brief Configure the GPIOs pinmux by the specified parameters
 * @param[in] param Pointer to the array of GPIO_PinMuxParam structure, one
 *                  array element for one GPIO pinmux
 * @param[in] count Elements number of the GPIO pinmux parameters array
 * @return None
 */
void HAL_GPIO_PinMuxConfig(const GPIO_PinMuxParam *param, uint32_t count)
{
	uint32_t i;

	for (i = 0; i < count; ++i) {
		HAL_GPIO_Init(param[i].port, param[i].pin, &param[i].config);
	}
}

/**
 * @brief Deconfigure the GPIOs pinmux by the specified parameters
 * @param[in] param Pointer to the array of GPIO_PinMuxParam structure, one
 *                  array element for one GPIO pinmux, param->config is ignored.
 * @param[in] count Elements number of the GPIO pinmux parameters array
 * @return None
 */
void HAL_GPIO_PinMuxDeConfig(const GPIO_PinMuxParam *param, uint32_t count)
{
	uint32_t i;

	for (i = 0; i < count; ++i) {
		HAL_GPIO_DeInit(param[i].port, param[i].pin);
	}
}
