/**
  * @file  hal_uart.c
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
#include "driver/chip/hal_uart.h"
#include "hal_base.h"
#include "pm/pm.h"

#define UART_TRANSMIT_BY_IRQ_HANDLER	1

typedef enum {
	UART_STATE_INVALID 	= 0,
	UART_STATE_READY	= 1,
} UART_State;

typedef struct {
	UART_State         		state;

	HAL_Semaphore	   		txSem;
	HAL_Semaphore	    	rxSem;

#if UART_TRANSMIT_BY_IRQ_HANDLER
	uint8_t                *txBuf;
	int32_t 		   		txBufSize;
#endif /* UART_TRANSMIT_BY_IRQ_HANDLER */

	uint8_t 		  	   *rxBuf;
	int32_t 		   		rxBufSize;

	UART_RxReadyCallback	rxReadyCallback;
	void                   *arg;

	DMA_Channel				txDMAChan;
	DMA_Channel				rxDMAChan;
} UART_Private;

static UART_Private gUartPrivate[UART_NUM];
static UART_T * const gUartInstance[UART_NUM] = { UART0, UART1 };

__STATIC_INLINE IRQn_Type UART_GetIRQnType(UART_ID uartID)
{
	return (uartID == UART0_ID ? UART0_IRQn: UART1_IRQn);
}

#ifdef CONFIG_PM

//#define CONFIG_UART_PM_DEBUG

#ifdef CONFIG_UART_PM_DEBUG
#define PM_UART_PRINT_BUF_LEN 512
static volatile uint32_t pm_print_index;
static char pm_print_buf[PM_UART_PRINT_BUF_LEN];
#endif

static int8_t g_uart_suspending = 0;
static int8_t g_uart_irq_enable = 0;
static UART_InitParam g_uart_param[UART_NUM];
static UART_RxReadyCallback g_uart_cb[UART_NUM];
static void *g_uart_arg[UART_NUM];

static int uart_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	UART_ID uartID = (UART_ID)dev->platform_data;

	g_uart_suspending |= (1 << uartID);

	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		if (g_uart_irq_enable & (1 << uartID))
			HAL_UART_DisableRxCallback(uartID);
		while (!HAL_UART_IsTxEmpty(HAL_UART_GetInstance(uartID)))
			;
		for (volatile int i = 0; i < 1000; i++) /* wait tx done */
			i = i;
		HAL_DBG("%s id:%d okay\n", __func__, uartID);
		HAL_UART_DeInit(uartID);
		break;
	default:
		break;
	}
	return 0;
}

static int uart_resume(struct soc_device *dev, enum suspend_state_t state)
{
	UART_ID uartID = (UART_ID)dev->platform_data;

	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_UART_Init(uartID, &g_uart_param[uartID]);
		if (g_uart_irq_enable & (1 << uartID))
			HAL_UART_EnableRxCallback(uartID, g_uart_cb[uartID],
			                          g_uart_arg[uartID]);
		HAL_DBG("%s id:%d okay\n", __func__, uartID);
		break;
	default:
		break;
	}

	g_uart_suspending &= ~(1 << uartID);

#ifdef CONFIG_UART_PM_DEBUG
	if (pm_print_index) {
		HAL_DBG("%s", pm_print_buf);
		pm_print_index = 0;
	}
#endif
	return 0;
}

static struct soc_device_driver uart_drv = {
	.name = "uart",
	.suspend_noirq = uart_suspend,
	.resume_noirq = uart_resume,
};

static struct soc_device uart_dev[UART_NUM] = {
	{.name = "uart0", .driver = &uart_drv, .platform_data = (void *)UART0_ID,},
	{.name = "uart1", .driver = &uart_drv, .platform_data = (void *)UART1_ID,},
};

#define UART_DEV(id) (&uart_dev[id])

#endif /* CONFIG_PM */

#define UART_ASSERT_ID(uartID)		HAL_ASSERT_PARAM((uartID) < UART_NUM)

__STATIC_INLINE CCM_BusPeriphBit UART_GetCCMPeriphBit(UART_ID uartID)
{
	return (uartID == UART0_ID ? CCM_BUS_PERIPH_BIT_UART0 :
	                             CCM_BUS_PERIPH_BIT_UART1);
}

__STATIC_INLINE UART_Private *UART_GetUartPriv(UART_ID uartID)
{
	return &gUartPrivate[uartID];
}

static void UART_EnableIRQ(UART_T *uart, uint32_t mask)
{
	HAL_SET_BIT(uart->DLH_IER.IRQ_EN, mask);
}

static void UART_DisableIRQ(UART_T *uart, uint32_t mask)
{
	HAL_CLR_BIT(uart->DLH_IER.IRQ_EN, mask);
}

__STATIC_INLINE void UART_DisableAllIRQ(UART_T *uart)
{
	uart->DLH_IER.IRQ_EN = 0U;
}

__STATIC_INLINE void UART_EnableTxReadyIRQ(UART_T *uart)
{
	UART_EnableIRQ(uart, UART_TX_FIFO_TRIG_MODE_EN_BIT | UART_TX_READY_IRQ_EN_BIT);
}

__STATIC_INLINE void UART_DisableTxReadyIRQ(UART_T *uart)
{
	UART_DisableIRQ(uart, UART_TX_FIFO_TRIG_MODE_EN_BIT | UART_TX_READY_IRQ_EN_BIT);
}

__STATIC_INLINE void UART_EnableRxReadyIRQ(UART_T *uart)
{
	UART_EnableIRQ(uart, UART_RX_READY_IRQ_EN_BIT);
}

__STATIC_INLINE void UART_DisableRxReadyIRQ(UART_T *uart)
{
	UART_DisableIRQ(uart, UART_RX_READY_IRQ_EN_BIT);
}

__STATIC_INLINE void UART_EnableFIFO(UART_T *uart)
{
	HAL_SET_BIT(uart->IIR_FCR.FIFO_CTRL, UART_FIFO_EN_BIT);
}

__STATIC_INLINE void UART_DisableFIFO(UART_T *uart)
{
	HAL_CLR_BIT(uart->IIR_FCR.FIFO_CTRL, UART_FIFO_EN_BIT);
}

__STATIC_INLINE void UART_ResetTxFIFO(UART_T *uart)
{
	HAL_SET_BIT(uart->IIR_FCR.FIFO_CTRL, UART_TX_FIFO_RESET_BIT);
}

__STATIC_INLINE void UART_ResetRxFIFO(UART_T *uart)
{
	HAL_SET_BIT(uart->IIR_FCR.FIFO_CTRL, UART_RX_FIFO_RESET_BIT);
}

/* baudRate = apb_freq / (16 * div) */
__STATIC_INLINE uint32_t UART_CalcClkDiv(uint32_t baudRate)
{
	uint32_t div;

	div = baudRate << 4;
	div = (HAL_GetAPBClock() + (div >> 1)) / div;
	if (div == 0)
		div = 1;
	return div;
}

__STATIC_INLINE void UART_SetClkDiv(UART_T *uart, uint16_t div)
{
	HAL_SET_BIT(uart->LINE_CTRL, UART_DIV_ACCESS_BIT);

	uart->RBR_THR_DLL.DIV_LOW = div & UART_DIV_LOW_MASK;
	uart->DLH_IER.DIV_HIGH = (div >> 8) & UART_DIV_HIGH_MASK;

	HAL_CLR_BIT(uart->LINE_CTRL, UART_DIV_ACCESS_BIT);
}

__STATIC_INLINE int UART_IsBusy(UART_T *uart)
{
	return HAL_GET_BIT(uart->STATUS, UART_BUSY_BIT);
}

__STATIC_INLINE void UART_EnableTx(UART_T *uart)
{
	HAL_CLR_BIT(uart->HALT, UART_HALT_TX_EN_BIT);
}

__STATIC_INLINE void UART_DisableTx(UART_T *uart)
{
	HAL_SET_BIT(uart->HALT, UART_HALT_TX_EN_BIT);
}

/**
 * @brief Get UART hardware instance by the specified UART ID
 * @param[in] uartID ID of the specified UART
 * @return UART hardware instance
 */
UART_T *HAL_UART_GetInstance(UART_ID uartID)
{
	return gUartInstance[uartID];
}

/**
 * @brief Check whether the specified UART can transmit data or not
 * @param[in] uart UART hardware instance
 * @return 1 The UART can transmit data
 * @return 0 The UART cannot transmit data
 */
int HAL_UART_IsTxReady(UART_T *uart)
{
//	return HAL_GET_BIT(uart->LINE_STATUS, UART_TX_HOLD_EMPTY_BIT);
	return HAL_GET_BIT(uart->STATUS, UART_TX_FIFO_NOT_FULL_BIT);
}

/**
 * @brief Check whether the transmit FIFO of the specified UART is empty or not
 * @param[in] uart UART hardware instance
 * @return 1 The UART transmit FIFO is empty
 * @return 0 The UART transmit FIFO is not empty
 */
int HAL_UART_IsTxEmpty(UART_T *uart)
{
	return HAL_GET_BIT(uart->STATUS, UART_TX_FIFO_EMPTY_BIT);
}

/**
 * @brief Check whether the specified UART has receive data or not
 * @param[in] uart UART hardware instance
 * @return 1 The UART has receive data
 * @return 0 The UART has no receive data
 */
int HAL_UART_IsRxReady(UART_T *uart)
{
#ifdef CONFIG_PM
	UART_ID uartID = (uart == gUartInstance[UART0_ID]) ? UART0_ID : UART1_ID;

	if (g_uart_suspending & (1 << uartID))
		return 0;
#endif

	return HAL_GET_BIT(uart->LINE_STATUS, UART_RX_READY_BIT);
//	return HAL_GET_BIT(uart->STATUS, UART_RX_FIFO_NOT_EMPTY_BIT);
}

/**
 * @brief Get one byte received data of the specified UART
 * @param[in] uart UART hardware instance
 * @return One byte received data
 *
 * @note Before calling this function, make sure the specified UART has
 *       receive data by calling HAL_UART_IsRxReady()
 */
uint8_t HAL_UART_GetRxData(UART_T *uart)
{
	return (uint8_t)(uart->RBR_THR_DLL.RX_BUF);
}

uint8_t HAL_UART_GetRxFIFOLevel(UART_T *uart)
{
       return (uint8_t)(uart->RX_FIFO_LEVEL);
}

/**
 * @brief Transmit one byte data for the specified UART
 * @param[in] uart UART hardware instance
 * @param[in] data one byte data
 * @return None
 *
 * @note Before calling this function, make sure the specified UART can
 *       transmit data by calling HAL_UART_IsTxReady()
 */
void HAL_UART_PutTxData(UART_T *uart, uint8_t data)
{
	uart->RBR_THR_DLL.TX_HOLD = data;
}

__STATIC_INLINE uint32_t UART_GetInterruptID(UART_T *uart)
{
	return HAL_GET_BIT(uart->IIR_FCR.IRQ_ID, UART_IID_MASK);
}

__STATIC_INLINE uint32_t UART_GetLineStatus(UART_T *uart)
{
	return HAL_GET_BIT(uart->LINE_STATUS, UART_LINE_STATUS_MASK);
}

__STATIC_INLINE uint32_t UART_GetModemStatus(UART_T *uart)
{
	return HAL_GET_BIT(uart->MODEM_STATUS, UART_MODEM_STATUS_MASK);
}

__STATIC_INLINE uint32_t UART_GetUartStatus(UART_T *uart)
{
	return HAL_GET_BIT(uart->STATUS, UART_STATUS_MASK);
}

static void UART_IRQHandler(UART_T *uart, UART_Private *priv)
{
	uint32_t iid = UART_GetInterruptID(uart);

	switch (iid) {
	case UART_IID_MODEM_STATUS:
		UART_GetModemStatus(uart);
		break;
	case UART_IID_TX_READY:
#if UART_TRANSMIT_BY_IRQ_HANDLER
		if (priv->txBuf) {
			while (priv->txBufSize > 0) {
				if (HAL_UART_IsTxReady(uart)) {
					HAL_UART_PutTxData(uart, *priv->txBuf);
					++priv->txBuf;
					--priv->txBufSize;
				} else {
					break;
				}
			}
			if (priv->txBufSize == 0) {
				UART_DisableTxReadyIRQ(uart);
				HAL_SemaphoreRelease(&priv->txSem); /* end transmitting */
			}
		}
#else /* UART_TRANSMIT_BY_IRQ_HANDLER */
		UART_DisableTxReadyIRQ(uart);
		HAL_SemaphoreRelease(&priv->txSem);
#endif /* UART_TRANSMIT_BY_IRQ_HANDLER */
		break;
	case UART_IID_RX_READY:
	case UART_IID_CHAR_TIMEOUT:
		if (priv->rxReadyCallback) {
			priv->rxReadyCallback(priv->arg);
		} else if (priv->rxBuf) {
			while (priv->rxBufSize > 0) {
				if (HAL_UART_IsRxReady(uart)) {
					*priv->rxBuf = HAL_UART_GetRxData(uart);
					++priv->rxBuf;
					--priv->rxBufSize;
				} else {
					break;
				}
			}
			if (priv->rxBufSize == 0 || iid == UART_IID_CHAR_TIMEOUT) {
				UART_DisableRxReadyIRQ(uart);
				HAL_SemaphoreRelease(&priv->rxSem); /* end receiving */
			}
		} else {
			HAL_WRN("no one receive data, but uart irq is enable\n");
			/* discard received data */
			while (HAL_UART_IsRxReady(uart)) {
				HAL_UART_GetRxData(uart);
			}
		}
		break;
	case UART_IID_LINE_STATUS:
		UART_GetLineStatus(uart);
		break;
	case UART_IID_BUSY_DETECT:
		UART_GetUartStatus(uart);
		break;
	case UART_IID_RS485:
	default:
		break;
	}
}

void UART0_IRQHandler(void)
{
	UART_IRQHandler(UART0, &gUartPrivate[UART0_ID]);
}

void UART1_IRQHandler(void)
{
	UART_IRQHandler(UART1, &gUartPrivate[UART1_ID]);
}

void N_UART_IRQHandler(void)
{
	HAL_NVIC_DisableIRQ(N_UART_IRQn);
	HAL_NVIC_ClearPendingIRQ(N_UART_IRQn);
}

/**
 * @brief Initialize the UART according to the specified parameters
 * @param[in] uartID ID of the specified UART
 * @param[in] param Pointer to UART_InitParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_UART_Init(UART_ID uartID, const UART_InitParam *param)
{
	UART_T *uart;
	UART_Private *priv;
	CCM_BusPeriphBit ccmPeriphBit;
	IRQn_Type IRQn;
	uint32_t tmp;
	unsigned long flags;

	UART_ASSERT_ID(uartID);

	flags = HAL_EnterCriticalSection();

	priv = UART_GetUartPriv(uartID);
	if (priv->state == UART_STATE_INVALID)
		priv->state = UART_STATE_READY;
	else
		priv = NULL;

	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("uart %d already inited\n", uartID);
		return HAL_BUSY;
	}

	uart = HAL_UART_GetInstance(uartID);

	HAL_SemaphoreInitBinary(&priv->txSem);
	HAL_SemaphoreInitBinary(&priv->rxSem);
#if UART_TRANSMIT_BY_IRQ_HANDLER
	priv->txBuf = NULL;
	priv->txBufSize = 0;
#endif
	priv->rxBuf = NULL;
	priv->rxBufSize = 0;
	priv->rxReadyCallback = NULL;
	priv->arg = NULL;
	priv->txDMAChan = DMA_CHANNEL_INVALID;
	priv->rxDMAChan = DMA_CHANNEL_INVALID;

	/* config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_UART, uartID), 0);

	/* enable uart clock and release reset */
	ccmPeriphBit = UART_GetCCMPeriphBit(uartID);
	HAL_CCM_BusEnablePeriphClock(ccmPeriphBit);
	HAL_CCM_BusReleasePeriphReset(ccmPeriphBit);

	UART_DisableAllIRQ(uart); /* disable all IRQ */

	UART_DisableTx(uart);

	/* wait uart become not busy if necessary */
	while (UART_IsBusy(uart)) {
		;
	}

	/* set baud rate, parity, stop bits, data bits */
	tmp = UART_CalcClkDiv(param->baudRate);
	UART_SetClkDiv(uart, (uint16_t)tmp);

	HAL_MODIFY_REG(uart->LINE_CTRL,
		           UART_PARITY_MASK | UART_STOP_BITS_MASK |
		           UART_DATA_BITS_MASK | UART_BREAK_CTRL_BIT,
		           param->parity | param->stopBits | param->dataBits);

	/* set uart->MODEM_CTRL */
	if (uartID != UART0_ID && param->isAutoHwFlowCtrl) {
		tmp = UART_WORK_MODE_UART | UART_AUTO_FLOW_CTRL_EN_BIT | UART_RTS_ASSERT_BIT;
	} else {
		tmp = UART_WORK_MODE_UART;
	}
	HAL_MODIFY_REG(uart->MODEM_CTRL,
		           UART_WORK_MODE_MASK | UART_LOOP_BACK_EN_BIT |
		           UART_AUTO_FLOW_CTRL_EN_BIT | UART_RTS_ASSERT_BIT,
		           tmp);

	/* set FIFO level, DMA mode, reset FIFO, enable FIFO */
	uart->IIR_FCR.FIFO_CTRL = UART_RX_FIFO_TRIG_LEVEL_HALF_FULL |
	                          UART_TX_FIFO_TRIG_LEVEL_EMPTY |
	                          UART_DMA_MODE_1 |
	                          UART_TX_FIFO_RESET_BIT |
	                          UART_RX_FIFO_RESET_BIT |
	                          UART_FIFO_EN_BIT;
	UART_EnableTx(uart);

	/* enable NVIC IRQ */
	IRQn = UART_GetIRQnType(uartID);
	HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_EnableIRQ(IRQn);

#ifdef CONFIG_PM
	if (!(g_uart_suspending & (1 << uartID))) {
		memcpy(&g_uart_param[uartID], param, sizeof(UART_InitParam));
		pm_register_ops(UART_DEV(uartID));
	}
#endif

	return HAL_OK;
}

/**
 * @brief DeInitialize the specified UART
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_UART_DeInit(UART_ID uartID)
{
	UART_T *uart;
	UART_Private *priv;
	CCM_BusPeriphBit ccmPeriphBit;
	unsigned long flags;

	UART_ASSERT_ID(uartID);

	priv = UART_GetUartPriv(uartID);
	if (priv->state == UART_STATE_INVALID) {
		return HAL_OK;
	}
	uart = HAL_UART_GetInstance(uartID);

#ifdef CONFIG_PM
	if (!(g_uart_suspending & (1 << uartID))) {
		pm_unregister_ops(UART_DEV(uartID));
	}
#endif

	HAL_UART_DisableTxDMA(uartID);
	HAL_UART_DisableRxDMA(uartID);

	if (priv->rxReadyCallback != NULL) {
		HAL_WRN("RX callback should be disabled first\n");
	}
	HAL_NVIC_DisableIRQ(UART_GetIRQnType(uartID));
	UART_DisableAllIRQ(uart);
	UART_DisableTx(uart);
	UART_DisableFIFO(uart);

	/* disable uart clock and force reset */
	ccmPeriphBit = UART_GetCCMPeriphBit(uartID);
	HAL_CCM_BusForcePeriphReset(ccmPeriphBit);
	HAL_CCM_BusDisablePeriphClock(ccmPeriphBit);

	/* De-config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_UART, uartID), 0);

	HAL_SemaphoreDeinit(&priv->txSem);
	HAL_SemaphoreDeinit(&priv->rxSem);

	flags = HAL_EnterCriticalSection();
	priv->state = UART_STATE_INVALID;
	HAL_ExitCriticalSection(flags);

	return HAL_OK;
}

/**
 * @brief Transmit an amount of data in interrupt mode
 * @param[in] uartID ID of the specified UART
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be transmitted
 * @return Number of bytes transmitted, -1 on error
 *
 * @note This function is not thread safe. If using the UART transmit series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t HAL_UART_Transmit_IT(UART_ID uartID, uint8_t *buf, int32_t size)
{
	UART_T *uart;
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

#if UART_TRANSMIT_BY_IRQ_HANDLER
	priv->txBuf = buf;
	priv->txBufSize = size;

	UART_EnableTxReadyIRQ(uart);
	HAL_SemaphoreWait(&priv->txSem, HAL_WAIT_FOREVER);
	UART_DisableTxReadyIRQ(uart);
 	priv->txBuf = NULL;
	size -= priv->txBufSize;
	priv->txBufSize = 0;
#else /* UART_TRANSMIT_BY_IRQ_HANDLER */
	uint8_t *ptr = buf;
	int32_t left = size;
	while (left > 0) {
		if (HAL_UART_IsTxReady(uart)) {
			HAL_UART_PutTxData(uart, *ptr);
			++ptr;
			--left;
		} else {
			UART_EnableTxReadyIRQ(uart);
			HAL_SemaphoreWait(&priv->txSem, HAL_WAIT_FOREVER);
		}
	}
	UART_DisableTxReadyIRQ(uart); /* just in case */
//	size -= left;
#endif /* UART_TRANSMIT_BY_IRQ_HANDLER */

	/* TODO: add protection */

	return size;
}

/**
 * @brief Receive an amount of data in interrupt mode
 * @param[in] uartID ID of the specified UART
 * @param[out] buf Pointer to the data buffer
 * @param[in] size The maximum number of bytes to be received.
 *                 The actual received bytes can be less than this.
 * @param[in] msec Timeout value in millisecond to receive data.
 *                 HAL_WAIT_FOREVER for no timeout.
 * @return Number of bytes received, -1 on error
 *
 * @note This function is not thread safe. If using the UART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t HAL_UART_Receive_IT(UART_ID uartID, uint8_t *buf, int32_t size, uint32_t msec)
{
	UART_T *uart;
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	if (priv->rxReadyCallback != NULL) {
		HAL_WRN("rx callback is enabled\n");
		return -1;
	}

	priv->rxBuf = buf;
	priv->rxBufSize = size;

	UART_EnableRxReadyIRQ(uart);
	HAL_SemaphoreWait(&priv->rxSem, msec);
	UART_DisableRxReadyIRQ(uart);

	priv->rxBuf = NULL;
	size -= priv->rxBufSize;
	priv->rxBufSize = 0;

	/* TODO: add protection */

	return size;
}

/**
 * @brief Enable receive ready callback function for the specified UART
 * @param[in] uartID ID of the specified UART
 * @param[in] cb The UART receive ready callback function
 * @param[in] arg Argument of the UART receive ready callback function
 * @retval HAL_Status, HAL_OK on success
 *
 * @note To handle receive data externally, use this function to enable the
 *       receive ready callback function, then receive and process the data in
 *       the callback function.
 * @note If the receive ready callback function is enabled, all other receive
 *       series functions cannot be used to receive data.
 * @note This function is not thread safe. If using the UART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
HAL_Status HAL_UART_EnableRxCallback(UART_ID uartID, UART_RxReadyCallback cb, void *arg)
{
	UART_T *uart;
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	priv->rxReadyCallback = cb;
	priv->arg = arg;

	UART_EnableRxReadyIRQ(uart);

	/* TODO: add protection */

#ifdef CONFIG_PM
	if (!(g_uart_suspending & (1 << uartID))) {
		g_uart_cb[uartID] = cb;
		g_uart_arg[uartID] = arg;
		g_uart_irq_enable |= (1 << uartID);
	}
#endif

	return HAL_OK;
}

/**
 * @brief Disable receive ready callback function for the specified UART
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_UART_DisableRxCallback(UART_ID uartID)
{
	UART_T *uart;
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	UART_DisableRxReadyIRQ(uart);

	priv->rxReadyCallback = NULL;
	priv->arg = NULL;

	/* TODO: add protection */

#ifdef CONFIG_PM
	if (!(g_uart_suspending & (1 << uartID))) {
		g_uart_cb[uartID] = NULL;
		g_uart_arg[uartID] = NULL;
		g_uart_irq_enable &= ~(1 << uartID);
	}
#endif

	return HAL_OK;
}

/**
 * @internal
 * @brief UART DMA transfer complete callback fucntion to release waiting
 *        semaphore
 * @param[in] arg Pointer to waiting semaphore for DMA transfer
 * @return None
 */
static void UART_DMAEndCallback(void *arg)
{
	HAL_SemaphoreRelease((HAL_Semaphore *)arg);
}

/**
 * @brief Enable UART transmitting data in DMA mode
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success, HAL_ERROR on no valid DMA channel
 *
 * @note To transmit data in DMA mode, a DMA channel for the specified
 *       UART to transmit data MUST be configured first by this function.
 */
HAL_Status HAL_UART_EnableTxDMA(UART_ID uartID)
{
	UART_T *uart;
	UART_Private *priv;
	DMA_ChannelInitParam dmaParam;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	if (priv->txDMAChan != DMA_CHANNEL_INVALID) {
		return HAL_OK;
	}

	priv->txDMAChan = HAL_DMA_Request();
	if (priv->txDMAChan == DMA_CHANNEL_INVALID) {
		return HAL_ERROR;
	}

	HAL_Memset(&dmaParam, 0, sizeof(dmaParam));
	dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
	                                          DMA_WAIT_CYCLE_2,
	                                          DMA_BYTE_CNT_MODE_REMAIN,
	                                          DMA_DATA_WIDTH_8BIT,
	                                          DMA_BURST_LEN_1,
	                                          DMA_ADDR_MODE_FIXED,
	                                          uartID == UART0_ID ?
	                                                    DMA_PERIPH_UART0 :
	                                                    DMA_PERIPH_UART1,
	                                          DMA_DATA_WIDTH_8BIT,
	                                          DMA_BURST_LEN_1,
	                                          DMA_ADDR_MODE_INC,
	                                          DMA_PERIPH_SRAM);
	dmaParam.irqType = DMA_IRQ_TYPE_END;
	dmaParam.endCallback = UART_DMAEndCallback;
	dmaParam.endArg = &priv->txSem;

	HAL_DMA_Init(priv->txDMAChan, &dmaParam);

	/* set DMA mode of UART */
//	HAL_MODIFY_REG(uart->IIR_FCR.FIFO_CTRL, UART_DMA_MODE_MASK, UART_DMA_MODE_1);
	HAL_SET_BIT(uart->HALT, UART_DMA_PTE_TX_BIT);

	return HAL_OK;
}

/**
 * @brief Enable UART receiving data in DMA mode
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success, HAL_ERROR on no valid DMA channel
 *
 * @note To reveive data in DMA mode, a DMA channel for the specified
 *       UART to receive data MUST be configured first by this function.
 */
HAL_Status HAL_UART_EnableRxDMA(UART_ID uartID)
{
	UART_T *uart;
	UART_Private *priv;
	DMA_ChannelInitParam dmaParam;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	if (priv->rxDMAChan != DMA_CHANNEL_INVALID) {
		return HAL_OK;
	}

	priv->rxDMAChan = HAL_DMA_Request();
	if (priv->rxDMAChan == DMA_CHANNEL_INVALID) {
		return HAL_ERROR;
	}

	HAL_Memset(&dmaParam, 0, sizeof(dmaParam));
	dmaParam.cfg = HAL_DMA_MakeChannelInitCfg(DMA_WORK_MODE_SINGLE,
	                                          DMA_WAIT_CYCLE_2,
	                                          DMA_BYTE_CNT_MODE_REMAIN,
	                                          DMA_DATA_WIDTH_8BIT,
	                                          DMA_BURST_LEN_1,
	                                          DMA_ADDR_MODE_INC,
	                                          DMA_PERIPH_SRAM,
	                                          DMA_DATA_WIDTH_8BIT,
	                                          DMA_BURST_LEN_1,
	                                          DMA_ADDR_MODE_FIXED,
	                                          uartID == UART0_ID ?
	                                                    DMA_PERIPH_UART0 :
	                                                    DMA_PERIPH_UART1);
	dmaParam.irqType = DMA_IRQ_TYPE_END;
	dmaParam.endCallback = UART_DMAEndCallback;
	dmaParam.endArg = &priv->rxSem;

	HAL_DMA_Init(priv->rxDMAChan, &dmaParam);

	/* set DMA mode of UART */
//	HAL_MODIFY_REG(uart->IIR_FCR.FIFO_CTRL, UART_DMA_MODE_MASK, UART_DMA_MODE_1);
	HAL_SET_BIT(uart->HALT, UART_DMA_PTE_RX_BIT);

	return HAL_OK;
}

/**
 * @brief Disable UART transmitting data in DMA mode
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_UART_DisableTxDMA(UART_ID uartID)
{
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	priv = UART_GetUartPriv(uartID);

	if (priv->txDMAChan != DMA_CHANNEL_INVALID) {
		HAL_DMA_Stop(priv->txDMAChan);
		HAL_DMA_DeInit(priv->txDMAChan);
		HAL_DMA_Release(priv->txDMAChan);
		priv->txDMAChan = DMA_CHANNEL_INVALID;
	}

	return HAL_OK;
}

/**
 * @brief Disable UART receiving data in DMA mode
 * @param[in] uartID ID of the specified UART
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_UART_DisableRxDMA(UART_ID uartID)
{
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	/* TODO: add protection */

	priv = UART_GetUartPriv(uartID);

	if (priv->rxDMAChan != DMA_CHANNEL_INVALID) {
		HAL_DMA_Stop(priv->rxDMAChan);
		HAL_DMA_DeInit(priv->rxDMAChan);
		HAL_DMA_Release(priv->rxDMAChan);
		priv->rxDMAChan = DMA_CHANNEL_INVALID;
	}

	return HAL_OK;
}

/**
 * @brief Transmit an amount of data in DMA mode
 *
 * Steps to transmit data in DMA mode:
 *     - use HAL_UART_EnableTxDMA() to enable UART transmit DMA mode
 *     - use HAL_UART_Transmit_DMA() to transmit data, it can be called
 *       repeatedly after HAL_UART_EnableTxDMA()
 *     - use HAL_UART_DisableTxDMA() to disable UART transmit DMA mode if needed
 *
 * @param[in] uartID ID of the specified UART
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be transmitted
 * @return Number of bytes transmitted, -1 on error
 *
 * @note This function is not thread safe. If using the UART transmit series
 *       functions in multi-thread, make sure they are executed exclusively.
 * @note To transmit data in DMA mode, HAL_UART_EnableTxDMA() MUST be executed
 *       before calling this function.
 */
int32_t HAL_UART_Transmit_DMA(UART_ID uartID, uint8_t *buf, int32_t size)
{
	UART_T *uart;
	UART_Private *priv;
	int32_t left;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	if (priv->txDMAChan == DMA_CHANNEL_INVALID) {
		HAL_WRN("tx dma is disabled\n");
		return -1;
	}

	HAL_DMA_Start(priv->txDMAChan, (uint32_t)buf, (uint32_t)&uart->RBR_THR_DLL.TX_HOLD, size);
	HAL_SemaphoreWait(&priv->txSem, HAL_WAIT_FOREVER);
	HAL_DMA_Stop(priv->txDMAChan);
	left = HAL_DMA_GetByteCount(priv->txDMAChan);

	/* TODO: add protection */

	return (size - left);
}

/**
 * @brief Receive an amount of data in DMA mode
 *
 * Steps to receive data in DMA mode:
 *     - use HAL_UART_EnableRxDMA() to enable UART receive DMA mode
 *     - use HAL_UART_Receive_DMA() to receive data, it can be called
 *       repeatedly after HAL_UART_EnableRxDMA()
 *     - use HAL_UART_DisableRxDMA() to disable UART receive DMA mode if needed
 *
 * @param[in] uartID ID of the specified UART
 * @param[out] buf Pointer to the data buffer
 * @param[in] size The maximum number of bytes to be received.
 *                 The actual received bytes can be less than this.
 * @param[in] msec Timeout value in millisecond to receive data.
 *                 HAL_WAIT_FOREVER for no timeout.
 * @return Number of bytes received, -1 on error
 *
 * @note This function is not thread safe. If using the UART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 * @note To receive data in DMA mode, HAL_UART_EnableRxDMA() MUST be executed
 *       before calling this function.
 */
int32_t HAL_UART_Receive_DMA(UART_ID uartID, uint8_t *buf, int32_t size, uint32_t msec)
{
	UART_T *uart;
	UART_Private *priv;
	int32_t left;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	priv = UART_GetUartPriv(uartID);

	if (priv->rxReadyCallback != NULL) {
		HAL_WRN("rx callback is enabled\n");
		return -1;
	}

	if (priv->rxDMAChan == DMA_CHANNEL_INVALID) {
		HAL_WRN("rx dma is disabled\n");
		return -1;
	}

	HAL_DMA_Start(priv->rxDMAChan, (uint32_t)&uart->RBR_THR_DLL.RX_BUF, (uint32_t)buf, size);
	HAL_SemaphoreWait(&priv->rxSem, msec);
	HAL_DMA_Stop(priv->rxDMAChan);
	left = HAL_DMA_GetByteCount(priv->rxDMAChan);

	/* TODO: add protection */

	return (size - left);
}

/**
 * @brief Transmit an amount of data in polling mode
 * @param[in] uartID ID of the specified UART
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be transmitted
 * @return Number of bytes transmitted, -1 on error
 *
 * @note This function is not thread safe. If using the UART transmit series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t HAL_UART_Transmit_Poll(UART_ID uartID, uint8_t *buf, int32_t size)
{
	UART_T *uart;
	uint8_t *ptr;
	int32_t left;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

#ifdef CONFIG_PM
	if (g_uart_suspending & (1 << uartID)) {
#ifdef CONFIG_UART_PM_DEBUG
		if (pm_print_index + size < (PM_UART_PRINT_BUF_LEN - 1)) {
			memcpy(pm_print_buf + pm_print_index, buf, size);
			pm_print_index += size;
		}
#endif
		return size;
	}
#endif

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	ptr = buf;
	left = size;
	while (left > 0) {
		while (!HAL_UART_IsTxReady(uart)) {
			; /* wait FIFO become not full */
		}

		HAL_UART_PutTxData(uart, *ptr);
		++ptr;
		--left;
	}

	/* TODO: add protection */

	return size;
}

/**
 * @brief Receive an amount of data in polling mode
 * @param[in] uartID ID of the specified UART
 * @param[out] buf Pointer to the data buffer
 * @param[in] size The maximum number of bytes to be received.
 *                 The actual received bytes can be less than this.
 * @param[in] msec Timeout value in millisecond to receive data.
 *                 HAL_WAIT_FOREVER for no timeout.
 * @return Number of bytes received, -1 on error
 *
 * @note This function is not thread safe. If using the UART receive series
 *       functions in multi-thread, make sure they are executed exclusively.
 */
int32_t HAL_UART_Receive_Poll(UART_ID uartID, uint8_t *buf, int32_t size, uint32_t msec)
{
	UART_T *uart;
	uint8_t *ptr;
	int32_t left;
	uint32_t endTick;
	UART_Private *priv;

	UART_ASSERT_ID(uartID);

	if (buf == NULL || size <= 0) {
		return -1;
	}

	priv = UART_GetUartPriv(uartID);
	if (priv->rxReadyCallback != NULL) {
		HAL_WRN("rx callback is enabled\n");
		return -1;
	}

	/* TODO: add protection */

	uart = HAL_UART_GetInstance(uartID);
	ptr = buf;
	left = size;

	if (msec != HAL_WAIT_FOREVER) {
		endTick = HAL_Ticks() + HAL_MSecsToTicks(msec);
	}
	while (left > 0) {
		if (HAL_UART_IsRxReady(uart)) {
			*ptr = HAL_UART_GetRxData(uart);
			++ptr;
			--left;
		} else if (msec != HAL_WAIT_FOREVER) {
			if (HAL_TimeAfter(HAL_Ticks(), endTick)) {
				break;
			}
		}
	}

	/* TODO: add protection */

	return (size - left);
}

/**
 * @brief Start or stop to transmit break characters
 * @param[in] uartID ID of the specified UART
 * @param[in] isSet
 *     @arg !0 Start to transmit break characters
 *     @arg  0 Stop to transmit break characters
 * @return None
 */
void HAL_UART_SetBreakCmd(UART_ID uartID, int8_t isSet)
{
	UART_T *uart;

	UART_ASSERT_ID(uartID);

	uart = HAL_UART_GetInstance(uartID);

	if (isSet) {
		HAL_SET_BIT(uart->LINE_CTRL, UART_BREAK_CTRL_BIT);
	} else {
		HAL_CLR_BIT(uart->LINE_CTRL, UART_BREAK_CTRL_BIT);
	}
}
