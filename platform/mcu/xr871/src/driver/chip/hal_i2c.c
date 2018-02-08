/**
  * @file  hal_i2c.c
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

#include "driver/chip/hal_i2c.h"
#include "hal_base.h"
#include "pm/pm.h"

#define I2C_MTX_TIMEOUT_MS	(5000)
#define I2C_SEM_TIMEOUT_MS	(3000)

/* I2C_Private.ctrl */
#define I2C_INIT_STATE_BIT	HAL_BIT(0)
#define I2C_7BIT_ADDR_BIT	HAL_BIT(1)
#define I2C_READ_MODE_BIT	HAL_BIT(2)
#define I2C_SCCB_MODE_BIT	HAL_BIT(3)
#define I2C_MEM_MODE_BIT	HAL_BIT(4)
#define I2C_RESTART_BIT		HAL_BIT(5)

typedef struct {
	uint8_t				ctrl;

	uint8_t				memAddr;
	uint16_t			devAddr;
	uint8_t			   *buf;
	int32_t				size;

	HAL_Mutex			mtx;
	HAL_Semaphore		sem;
} I2C_Private;

static I2C_Private	gI2CPrivate[I2C_NUM];
static I2C_T	   *gI2CInstance[I2C_NUM] = {I2C0, I2C1};

#define I2C_ASSERT_ID(i2cID)	HAL_ASSERT_PARAM((i2cID) < I2C_NUM)

#ifdef CONFIG_PM
static I2C_InitParam hal_i2c_param[I2C_NUM];
static uint8_t hal_i2c_suspending = 0;

static int i2c_suspend(struct soc_device *dev, enum suspend_state_t state)
{
	I2C_ID i2cID = (I2C_ID)dev->platform_data;

	hal_i2c_suspending |= (1 << i2cID);

	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
	case PM_MODE_POWEROFF:
		HAL_I2C_DeInit(i2cID);
		break;
	default:
		break;
	}

	return 0;
}

static int i2c_resume(struct soc_device *dev, enum suspend_state_t state)
{
	I2C_ID i2cID = (I2C_ID)dev->platform_data;

	switch (state) {
	case PM_MODE_SLEEP:
	case PM_MODE_STANDBY:
	case PM_MODE_HIBERNATION:
		HAL_I2C_Init(i2cID, &hal_i2c_param[i2cID]);
		break;
	default:
		break;
	}

	hal_i2c_suspending &= ~(1 << i2cID);

	return 0;
}

static struct soc_device_driver i2c_drv = {
	.name = "i2c",
	.suspend = i2c_suspend,
	.resume = i2c_resume,
};

static struct soc_device i2c_dev[] = {
	{.name = "i2c0", .driver = &i2c_drv, .platform_data = (void *)I2C0_ID,},
	{.name = "i2c1", .driver = &i2c_drv, .platform_data = (void *)I2C1_ID,},
};

#define I2C_DEV(id) (&i2c_dev[id])
#else
#define I2C_DEV(id) NULL
#endif

__STATIC_INLINE I2C_T *I2C_GetI2CInstance(I2C_ID i2cID)
{
	return gI2CInstance[i2cID];
}

__STATIC_INLINE I2C_Private *I2C_GetI2CPriv(I2C_ID i2cID)
{
	return &gI2CPrivate[i2cID];
}

__STATIC_INLINE void I2C_SetInitStateBit(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_INIT_STATE_BIT);
}

__STATIC_INLINE void I2C_ClrInitStateBit(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_INIT_STATE_BIT);
}

__STATIC_INLINE uint8_t I2C_IsInitState(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_INIT_STATE_BIT);
}

__STATIC_INLINE void I2C_Set7BitAddrMode(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_7BIT_ADDR_BIT);
}

__STATIC_INLINE void I2C_Set10BitAddrMode(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_7BIT_ADDR_BIT);
}

__STATIC_INLINE uint8_t I2C_Is7BitAddrMode(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_7BIT_ADDR_BIT);
}

__STATIC_INLINE void I2C_SetReadMode(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_READ_MODE_BIT);
}

__STATIC_INLINE void I2C_SetWriteMode(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_READ_MODE_BIT);
}

__STATIC_INLINE uint8_t I2C_IsReadMode(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_READ_MODE_BIT);
}

__STATIC_INLINE void I2C_SetSCCBMode(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_SCCB_MODE_BIT);
}

__STATIC_INLINE void I2C_ClrSCCBMode(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_SCCB_MODE_BIT);
}

__STATIC_INLINE uint8_t I2C_IsSCCBMode(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_SCCB_MODE_BIT);
}

__STATIC_INLINE void I2C_SetMemMode(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_MEM_MODE_BIT);
}

__STATIC_INLINE void I2C_ClrMemMode(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_MEM_MODE_BIT);
}

__STATIC_INLINE uint8_t I2C_IsMemMode(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_MEM_MODE_BIT);
}

__STATIC_INLINE void I2C_SetRestartBit(I2C_Private *priv)
{
	HAL_SET_BIT(priv->ctrl, I2C_RESTART_BIT);
}

__STATIC_INLINE void I2C_ClrRestartBit(I2C_Private *priv)
{
	HAL_CLR_BIT(priv->ctrl, I2C_RESTART_BIT);
}

__STATIC_INLINE uint8_t I2C_IsRestart(I2C_Private *priv)
{
	return !!HAL_GET_BIT(priv->ctrl, I2C_RESTART_BIT);
}

__STATIC_INLINE uint8_t I2C_Get7BitAddrRd(I2C_Private *priv)
{
	return (uint8_t)((priv->devAddr << 1) | 0x1);
}

__STATIC_INLINE uint8_t I2C_Get7BitAddrWr(I2C_Private *priv)
{
	return (uint8_t)((priv->devAddr << 1) | 0x0);
}

__STATIC_INLINE uint8_t I2C_Get10BitAddr1Rd(I2C_Private *priv)
{
	uint8_t tmp = (uint8_t)(priv->devAddr >> 7);
	tmp &= ~(0x08U);
	tmp |= 0xF1U;
	return tmp;
}

__STATIC_INLINE uint8_t I2C_Get10BitAddr1Wr(I2C_Private *priv)
{
	uint8_t tmp = (uint8_t)(priv->devAddr >> 7);
	tmp &= ~(0x09U);
	tmp |= 0xF0U;
	return tmp;
}

__STATIC_INLINE uint8_t I2C_Get10BitAddr2(I2C_Private *priv)
{
	return (uint8_t)(HAL_GET_BIT(priv->devAddr, 0xFFU));
}

__STATIC_INLINE uint8_t I2C_GetData(I2C_T *i2c)
{
	return (uint8_t)(HAL_GET_BIT(i2c->I2C_DATA, I2C_DATA_MASK));
}

__STATIC_INLINE void I2C_PutData(I2C_T *i2c, uint8_t data)
{
	i2c->I2C_DATA = data;
}

__STATIC_INLINE void I2C_EnableIRQ(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_IRQ_EN_BIT);
}

__STATIC_INLINE void I2C_DisableIRQ(I2C_T *i2c)
{
	HAL_CLR_BIT(i2c->I2C_CTRL, I2C_WR_CTRL_MASK | I2C_IRQ_EN_BIT);
}

__STATIC_INLINE void I2C_EnableBus(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_BUS_EN_BIT);
}

__STATIC_INLINE void I2C_DisableBus(I2C_T *i2c)
{
	HAL_CLR_BIT(i2c->I2C_CTRL, I2C_WR_CTRL_MASK | I2C_BUS_EN_BIT);
}

__STATIC_INLINE void I2C_SendStart(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_START_BIT);
}

__STATIC_INLINE uint8_t I2C_GetStartBit(I2C_T *i2c)
{
	return !!HAL_GET_BIT(i2c->I2C_CTRL, I2C_START_BIT);
}

__STATIC_INLINE void I2C_SendStop(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_STOP_BIT);
}

__STATIC_INLINE uint8_t I2C_GetStopBit(I2C_T *i2c)
{
	return !!HAL_GET_BIT(i2c->I2C_CTRL, I2C_STOP_BIT);
}

__STATIC_INLINE void I2C_SendStopStart(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_STOP_BIT | I2C_START_BIT);
}

__STATIC_INLINE void I2C_ClrIRQFlag(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_IRQ_FLAG_BIT);
}

__STATIC_INLINE uint8_t I2C_GetIRQFlag(I2C_T *i2c)
{
	return !!HAL_GET_BIT(i2c->I2C_CTRL, I2C_IRQ_FLAG_BIT);
}

__STATIC_INLINE void I2C_EnableACK(I2C_T *i2c)
{
	HAL_MODIFY_REG(i2c->I2C_CTRL, I2C_WR_CTRL_MASK, I2C_ACK_EN_BIT);
}

__STATIC_INLINE void I2C_DisableACK(I2C_T *i2c)
{
	HAL_CLR_BIT(i2c->I2C_CTRL, I2C_WR_CTRL_MASK | I2C_ACK_EN_BIT);
}

__STATIC_INLINE uint32_t I2C_GetIRQStatus(I2C_T *i2c)
{
	return HAL_GET_BIT(i2c->I2C_STATUS, I2C_STATUS_MASK);
}

__STATIC_INLINE void I2C_SetClockReg(I2C_T *i2c, uint8_t clkM, uint8_t clkN)
{
	HAL_MODIFY_REG(i2c->I2C_CLK_CTRL, I2C_CLK_M_MASK | I2C_CLK_N_MASK,
				   (clkM << I2C_CLK_M_SHIFT) | (clkN << I2C_CLK_N_SHIFT));
}

__STATIC_INLINE void I2C_SetClockFreq(I2C_T *i2c, uint32_t clockFreq)
{
	uint8_t	clkM 	= 0;
	uint8_t	clkN 	= 0;
	uint8_t	pow2N	= 1;

	uint32_t	APBClkDiv10;
	uint32_t	div;
	uint32_t	clockReal;

	APBClkDiv10 = HAL_CCM_BusGetAPBClock() / 10;
	div = APBClkDiv10 / clockFreq;
	if (div == 0) {
		I2C_SetClockReg(i2c, clkM, clkN);
		return;
	}

	while (clkN <= I2C_CLK_N_MAX) {
		clkM = div / pow2N - 1;
		while (clkM <= I2C_CLK_M_MAX) {
			clockReal = APBClkDiv10 / pow2N / (clkM + 1);
			if (clockReal <= clockFreq) {
				I2C_SetClockReg(i2c, clkM, clkN);
				return;
			} else {
				clkM++;
			}
		}
		clkN++;
		pow2N *= 2;
	}
}

__STATIC_INLINE void I2C_SoftReset(I2C_T *i2c)
{
	HAL_SET_BIT(i2c->I2C_SOFT_RST, I2C_SOFT_RST_BIT);
}

static void I2C_IRQHandler(I2C_T *i2c, I2C_Private *priv)
{
	uint8_t		end = 0;
	uint32_t	IRQStatus = I2C_GetIRQStatus(i2c);

	HAL_I2C_DBG("IRQ Status: %#x\n", IRQStatus);

	switch (IRQStatus) {
	case I2C_START_TRAN:
		if (I2C_Is7BitAddrMode(priv)) {
			if ((!I2C_IsMemMode(priv)) && (I2C_IsReadMode(priv)))
				I2C_PutData(i2c, I2C_Get7BitAddrRd(priv));
			else
				I2C_PutData(i2c, I2C_Get7BitAddrWr(priv));
		} else {
			if ((!I2C_IsMemMode(priv)) && (I2C_IsReadMode(priv)))
				I2C_PutData(i2c, I2C_Get10BitAddr1Rd(priv));
			else
				I2C_PutData(i2c, I2C_Get10BitAddr1Wr(priv));
		}
		break;
	case I2C_RE_START_TRAN:
		if (I2C_Is7BitAddrMode(priv))
			I2C_PutData(i2c, I2C_Get7BitAddrRd(priv));
		else
			I2C_PutData(i2c, I2C_Get10BitAddr1Rd(priv));
		break;
	case I2C_ADDR_WR_TRAN_ACK:
		if (I2C_Is7BitAddrMode(priv)) {
			if (I2C_IsMemMode(priv)) {
				I2C_PutData(i2c, priv->memAddr);
			} else {
				I2C_PutData(i2c, *priv->buf);
				priv->buf++;
				priv->size--;
			}
		} else {
			I2C_PutData(i2c, I2C_Get10BitAddr2(priv));
		}
		break;
	case I2C_ADDR_RD_TRAN_ACK:
		if (!I2C_Is7BitAddrMode(priv))
			I2C_PutData(i2c, I2C_Get10BitAddr2(priv));
		if (priv->size == 1)
			I2C_DisableACK(i2c);
		break;
	case I2C_SEC_ADDR_WR_ACK:
		if (I2C_IsMemMode(priv)) {
			I2C_PutData(i2c, priv->memAddr);
		} else {
			I2C_PutData(i2c, *priv->buf);
			priv->buf++;
			priv->size--;
		}
		break;
	case I2C_MASTER_DATA_TRAN_ACK:
		if (I2C_IsMemMode(priv) && I2C_IsReadMode(priv)) {
			I2C_SendStart(i2c);
			I2C_SetRestartBit(priv);
		} else {
			if (priv->size > 0) {
				I2C_PutData(i2c, *priv->buf);
				priv->buf++;
				priv->size--;
			} else {
				end = 1;
			}
		}
		break;
	case I2C_MASTER_DATA_RECV_ACK:
		*priv->buf = I2C_GetData(i2c);
		priv->buf++;
		priv->size--;
		if (priv->size == 1)
			I2C_DisableACK(i2c);
		break;
	case I2C_MASTER_DATA_RECV_NACK:
		*priv->buf = I2C_GetData(i2c);
		priv->buf++;
		priv->size--;
		end = 1;
		break;
	case I2C_ADDR_WR_TRAN_NACK:
		HAL_ERR("Invalid IIC address\n");
		end = 1;
		break;
	case I2C_ADDR_RD_TRAN_NACK:
		if (!I2C_IsMemMode(priv))
			HAL_ERR("Invalid IIC address\n");
		else
			HAL_ERR("No ACK received after 2nd-address-send\n");
		end = 1;
		break;
	case I2C_MASTER_DATA_TRAN_NACK:
		HAL_ERR("In writing, no ACK received\n");
		end = 1;
		break;
	default:
		end = 1;
		break;
	}

	if (end) {
		I2C_SendStop(i2c);
		HAL_SemaphoreRelease(&priv->sem);
	}

	I2C_ClrIRQFlag(i2c);
	while (I2C_GetIRQFlag(i2c))
		;

	return;
}

static void I2C_SCCBIRQHandler(I2C_T *i2c, I2C_Private *priv)
{
	uint8_t		end = 0;
	uint32_t	IRQStatus = I2C_GetIRQStatus(i2c);

	switch (IRQStatus) {
	case I2C_START_TRAN:
		if (I2C_IsRestart(priv))
			I2C_PutData(i2c, I2C_Get7BitAddrRd(priv));
		else
			I2C_PutData(i2c, I2C_Get7BitAddrWr(priv));
		break;
	case I2C_ADDR_WR_TRAN_ACK:
	case I2C_ADDR_WR_TRAN_NACK:
		I2C_PutData(i2c, priv->memAddr);
		break;
	case I2C_ADDR_RD_TRAN_ACK:
	case I2C_ADDR_RD_TRAN_NACK:
		I2C_DisableACK(i2c);
		break;
	case I2C_MASTER_DATA_TRAN_ACK:
	case I2C_MASTER_DATA_TRAN_NACK:
		if (I2C_IsReadMode(priv)) {
			I2C_SendStopStart(i2c);
			I2C_SetRestartBit(priv);
		} else {
			if (priv->size > 0) {
				I2C_PutData(i2c, *priv->buf);
				priv->buf++;
				priv->size--;
			} else {
				end = 1;
			}
		}
		break;
	case I2C_MASTER_DATA_RECV_ACK:
	case I2C_MASTER_DATA_RECV_NACK:
		*priv->buf = I2C_GetData(i2c);
		priv->buf++;
		priv->size--;
		end = 1;
		break;
	default:
		end = 1;
		break;
	}

	if (end) {
		I2C_SendStop(i2c);
		HAL_SemaphoreRelease(&priv->sem);
	}

	I2C_ClrIRQFlag(i2c);
	while (I2C_GetIRQFlag(i2c))
		;

	return;
}

void TWI0_IRQHandler(void)
{
	if (I2C_IsSCCBMode(&gI2CPrivate[I2C0_ID]))
		I2C_SCCBIRQHandler(I2C0, &gI2CPrivate[I2C0_ID]);
	else
		I2C_IRQHandler(I2C0, &gI2CPrivate[I2C0_ID]);
}

void TWI1_IRQHandler(void)
{
	if (I2C_IsSCCBMode(&gI2CPrivate[I2C1_ID]))
		I2C_SCCBIRQHandler(I2C1, &gI2CPrivate[I2C1_ID]);
	else
		I2C_IRQHandler(I2C1, &gI2CPrivate[I2C1_ID]);
}

/**
 * @brief Initialize the I2C according to the specified parameters
 * @param[in] i2cID ID of the specified I2C
 * @param[in] initParam Pointer to I2C_InitParam structure
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_I2C_Init(I2C_ID i2cID, const I2C_InitParam *initParam)
{
	I2C_T			   *i2c;
	I2C_Private		   *priv;
	CCM_BusPeriphBit	ccmPeriphBit;
	IRQn_Type			IRQn;
	unsigned long		flags;

	I2C_ASSERT_ID(i2cID);

	if (initParam->clockFreq == 0) {
		HAL_ERR("clock frequency is zero.\n");
		return HAL_ERROR;
	}

	flags = HAL_EnterCriticalSection();
	priv = I2C_GetI2CPriv(i2cID);
	if (!I2C_IsInitState(priv))
		I2C_SetInitStateBit(priv);
	else
		priv = NULL;
	HAL_ExitCriticalSection(flags);

	if (priv == NULL) {
		HAL_WRN("i2c %d already inited\n", i2cID);
		return HAL_BUSY;
	}
#ifdef CONFIG_PM
	if (!(hal_i2c_suspending & (1 << i2cID))) {
		pm_register_ops(I2C_DEV(i2cID));
		HAL_Memcpy(&hal_i2c_param[i2cID], initParam, sizeof(I2C_InitParam));
	}
#endif
	i2c = I2C_GetI2CInstance(i2cID);

	if (initParam->addrMode == I2C_ADDR_MODE_7BIT)
		I2C_Set7BitAddrMode(priv);
	else
		I2C_Set10BitAddrMode(priv);

	priv->memAddr = 0;
	priv->devAddr = 0;
	priv->buf = NULL;
	priv->size = 0;
	HAL_MutexInit(&priv->mtx);
	HAL_SemaphoreInitBinary(&priv->sem);

	/* config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_INIT, HAL_MKDEV(HAL_DEV_MAJOR_I2C, i2cID), 0);

	/* enable i2c clock and release reset */
	ccmPeriphBit = (i2cID == I2C0_ID ? CCM_BUS_PERIPH_BIT_I2C0 :
									   CCM_BUS_PERIPH_BIT_I2C1);
	HAL_CCM_BusEnablePeriphClock(ccmPeriphBit);
	HAL_CCM_BusReleasePeriphReset(ccmPeriphBit);

	I2C_DisableIRQ(i2c);
	I2C_DisableBus(i2c);
	I2C_SetClockFreq(i2c, initParam->clockFreq);
	I2C_EnableBus(i2c);

	/* enable NVIC IRQ */
	IRQn = (i2cID == I2C0_ID) ? I2C0_IRQn : I2C1_IRQn;
	HAL_NVIC_SetPriority(IRQn, NVIC_PERIPHERAL_PRIORITY_DEFAULT);
	HAL_NVIC_EnableIRQ(IRQn);

	return HAL_OK;
}

/**
 * @brief DeInitialize the specified I2C
 * @param[in] i2cID ID of the specified I2C
 * @retval HAL_Status, HAL_OK on success
 */
HAL_Status HAL_I2C_DeInit(I2C_ID i2cID)
{
	I2C_T			   *i2c;
	I2C_Private		   *priv;
	CCM_BusPeriphBit	ccmPeriphBit;
	unsigned long		flags;

	I2C_ASSERT_ID(i2cID);

#ifdef CONFIG_PM
	if (!(hal_i2c_suspending & (1 << i2cID))) {
		pm_unregister_ops(I2C_DEV(i2cID));
	}
#endif

	i2c = I2C_GetI2CInstance(i2cID);
	priv = I2C_GetI2CPriv(i2cID);

	HAL_NVIC_DisableIRQ((i2cID == I2C0_ID) ? I2C0_IRQn : I2C1_IRQn);

	I2C_DisableBus(i2c);

	/* disable i2c clock and force reset */
	ccmPeriphBit = (i2cID == I2C0_ID ? CCM_BUS_PERIPH_BIT_I2C0 :
									   CCM_BUS_PERIPH_BIT_I2C1);
	HAL_CCM_BusForcePeriphReset(ccmPeriphBit);
	HAL_CCM_BusDisablePeriphClock(ccmPeriphBit);

	/* De-config pinmux */
	HAL_BoardIoctl(HAL_BIR_PINMUX_DEINIT, HAL_MKDEV(HAL_DEV_MAJOR_I2C, i2cID), 0);

	HAL_MutexDeinit(&priv->mtx);
	HAL_SemaphoreDeinit(&priv->sem);

	flags = HAL_EnterCriticalSection();
	I2C_ClrInitStateBit(priv);
	HAL_ExitCriticalSection(flags);

	return HAL_OK;
}

static int32_t I2C_Master_common(I2C_ID i2cID, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size)
{
	I2C_T			   *i2c;
	I2C_Private 	   *priv;

	I2C_ASSERT_ID(i2cID);

	if (buf == NULL || size <= 0) {
		HAL_ERR("buf %p, size %d\n", buf, size);
		return -1;
	}

	i2c = I2C_GetI2CInstance(i2cID);
	priv = I2C_GetI2CPriv(i2cID);

	priv->devAddr = devAddr;
	priv->memAddr = memAddr;
	priv->buf = buf;
	priv->size = size;

	I2C_EnableACK(i2c);
	I2C_EnableIRQ(i2c);
	I2C_SendStart(i2c);

	if (HAL_SemaphoreWait(&priv->sem, I2C_SEM_TIMEOUT_MS) != HAL_OK)
		HAL_WRN("I2C wait semaphore failed, i2c ID %d\n", i2cID);

	while (I2C_GetStopBit(i2c))
		;
	I2C_DisableIRQ(i2c);
	I2C_DisableACK(i2c);

	size -= priv->size;
	priv->devAddr = 0;
	priv->memAddr = 0;
	priv->buf = NULL;
	priv->size = 0;

	return size;
}

/**
 * @brief Transmit an amount of data in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be transmitted
 * @return Number of bytes transmitted, -1 on error
 */
int32_t HAL_I2C_Master_Transmit_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t *buf, int32_t size)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetWriteMode(priv);
	I2C_ClrSCCBMode(priv);
	I2C_ClrMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, 0, buf, size);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

/**
 * @brief Receive an amount of data in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be received
 * @return Number of bytes received, -1 on error
 */
int32_t HAL_I2C_Master_Receive_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t *buf, int32_t size)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetReadMode(priv);
	I2C_ClrSCCBMode(priv);
	I2C_ClrMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, 0, buf, size);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

/**
 * @brief Transmit an amount of data to specified memory or register of the
 *        slave device in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] memAddr Memory or register address
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be transmitted
 * @return Number of bytes transmitted, -1 on error
 */
int32_t HAL_I2C_Master_Transmit_Mem_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetWriteMode(priv);
	I2C_ClrSCCBMode(priv);
	I2C_SetMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, memAddr, buf, size);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

/**
 * @brief Receive an amount of data from specified memory or register of the
 *        slave device in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] memAddr Memory or register address
 * @param[in] buf Pointer to the data buffer
 * @param[in] size Number of bytes to be received
 * @return Number of bytes received, -1 on error
 */
int32_t HAL_I2C_Master_Receive_Mem_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetReadMode(priv);
	I2C_ClrSCCBMode(priv);
	I2C_SetMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, memAddr, buf, size);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

/**
 * @brief Transmit one byte data through SCCB protocol in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] subAddr Sub-address
 * @param[in] buf Pointer to the data buffer
 * @return Number of bytes transmitted, -1 on error
 */
int32_t HAL_I2C_SCCB_Master_Transmit_IT(I2C_ID i2cID, uint8_t devAddr, uint8_t subAddr, uint8_t *buf)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetWriteMode(priv);
	I2C_SetSCCBMode(priv);
	I2C_ClrMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, subAddr, buf, 0x01);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

/**
 * @brief Receive one byte data through SCCB protocol in interrupt mode
 * @param[in] i2cID ID of the specified I2C
 * @param[in] devAddr Device address
 * @param[in] subAddr Sub-address
 * @param[in] buf Pointer to the data buffer
 * @return Number of bytes received, -1 on error
 */
int32_t HAL_I2C_SCCB_Master_Receive_IT(I2C_ID i2cID, uint8_t devAddr, uint8_t subAddr, uint8_t *buf)
{
	I2C_Private *priv = I2C_GetI2CPriv(i2cID);

	if (HAL_MutexLock(&priv->mtx, I2C_MTX_TIMEOUT_MS) != HAL_OK) {
		HAL_WRN("I2C wait mutex failed, i2c ID %d\n", i2cID);
		return 0;
	}

	I2C_SetReadMode(priv);
	I2C_SetSCCBMode(priv);
	I2C_ClrMemMode(priv);
	I2C_ClrRestartBit(priv);

	int32_t ret = I2C_Master_common(i2cID, devAddr, subAddr, buf, 0x01);

	HAL_MutexUnlock(&priv->mtx);

	return ret;
}

