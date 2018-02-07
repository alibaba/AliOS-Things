/**
  * @file  hal_i2c.h
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

#ifndef _DRIVER_CHIP_HAL_I2C_H_
#define _DRIVER_CHIP_HAL_I2C_H_

#include "driver/chip/hal_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief I2C ID definition
 */
typedef enum {
	I2C0_ID	= 0,
	I2C1_ID	= 1,
	I2C_NUM	= 2
} I2C_ID;

/**
 * @brief I2C register block structure
 */
typedef struct {
	__IO uint32_t I2C_ADDR;      /* offset: 0x00, I2C slave address register */
	__IO uint32_t I2C_XADDR;     /* offset: 0x04, I2C extend slave address register */
	__IO uint32_t I2C_DATA;      /* offset: 0x08, I2C data register */
	__IO uint32_t I2C_CTRL;      /* offset: 0x0C, I2C control register */
	__I  uint32_t I2C_STATUS;    /* offset: 0x10, I2C status register */
	__IO uint32_t I2C_CLK_CTRL;  /* offset: 0x14, I2C clock register */
	__IO uint32_t I2C_SOFT_RST;  /* offset: 0x18, I2C software reset register */
	__IO uint32_t I2C_EFR;       /* offset: 0x1C, I2C enhance feature register */
	__IO uint32_t I2C_LINE_CTRL; /* offset: 0x20, I2C line control register */
} I2C_T;

#define I2C0	((I2C_T *)I2C0_BASE) /* address: 0x40041C00 */
#define I2C1	((I2C_T *)I2C1_BASE) /* address: 0x40042000 */

/* I2Cx->I2C_ADDR, R/W */
#define I2C_SLAVE_ADDR_SHIFT	1
#define I2C_SLAVE_ADDR_MASK		(0x7FU << I2C_SLAVE_ADDR_SHIFT)

#define I2C_GCE_BIT				HAL_BIT(0)

/* I2Cx->I2C_XADDR, R/W */
#define I2C_SLAVE_XADDR_MASK	0xFFU

/* I2Cx->I2C_DATA, R/W */
#define I2C_DATA_MASK			0xFFU

/* I2Cx->I2C_CTRL, R/W */
#define I2C_IRQ_EN_BIT			HAL_BIT(7)
#define I2C_BUS_EN_BIT			HAL_BIT(6)
#define I2C_START_BIT			HAL_BIT(5)
#define I2C_STOP_BIT			HAL_BIT(4)
#define I2C_IRQ_FLAG_BIT		HAL_BIT(3)
#define	I2C_ACK_EN_BIT			HAL_BIT(2)

#define I2C_WR_CTRL_MASK		(I2C_START_BIT | I2C_STOP_BIT | I2C_IRQ_FLAG_BIT)

/* I2Cx->I2C_STATUS, R */
#define I2C_STATUS_MASK			0xFFU

typedef enum {
	I2C_BUS_ERROR				= 0x00U, /* bus error */
	I2C_START_TRAN				= 0x08U, /* start condition transmitted */
	I2C_RE_START_TRAN			= 0x10U, /* repeated start condition transmitted */
	I2C_ADDR_WR_TRAN_ACK		= 0x18U, /* address + write bit transmitted, ACK received */
	I2C_ADDR_WR_TRAN_NACK		= 0x20U, /* address + write bit transmitted, ACK not received */
	I2C_MASTER_DATA_TRAN_ACK	= 0x28U, /* data byte transmitted in master mode, ACK received */
	I2C_MASTER_DATA_TRAN_NACK	= 0x30U, /* data byte transmitted in master mode, ACK not received */
	I2C_ARB_LOST				= 0x38U, /* arbitration lost in address or data byte */
	I2C_ADDR_RD_TRAN_ACK		= 0x40U, /* address + read bit transmitted, ACK received */
	I2C_ADDR_RD_TRAN_NACK		= 0x48U, /* address + read bit transmitted, ACK not received */
	I2C_MASTER_DATA_RECV_ACK	= 0x50U, /* data byte received in master mode, ACK transmitted */
	I2C_MASTER_DATA_RECV_NACK	= 0x58U, /* data byte received in master mode, ACK not transmitted */
	I2C_ADDR_WR_RECV_ACK		= 0x60U, /* slave address + write bit received, ACK transmitted */
	I2C_ARB_LOST_ADDR_WR		= 0x68U, /* arbitration lost in address as master, slave address + write bit received, ACK transmitted */
	I2C_GC_ADDR_RECV			= 0x70U, /* general call adress received, ACK transmitted */
	I2C_ARB_LOST_GC_ADDR		= 0x78U, /* arbitration lost in address as master, general call address received, ACK transmitted */
	I2C_DATA_RECV_SLAVE_ACK		= 0x80U, /* data byte received after slave address received, ACK transmitted */
	I2C_DATA_RECV_SLAVE_NACK	= 0x88U, /* data byte reveived after slave address received, ACK not transmitted */
	I2C_DATA_RECV_GC_ACK		= 0x90U, /* data byte received after general call received, ACK transmitted */
	I2C_DATA_RECV_GC_NACK		= 0x98U, /* data byte received after general call received, ACK not transmitted */
	I2C_STOP_RE_START_RECV		= 0xA0U, /* stop or repeated start condition received in slave mode */
	I2C_ADDR_RD_RECV_ACK		= 0xA8U, /* slave address + read bit received, ACK transmitted */
	I2C_ARB_LOST_ADDR_RD		= 0xB0U, /* arbitration lost in address as master, slave address + read bit received, ACK transmitted */
	I2C_SLAVE_DATA_TRAN_ACK		= 0xB8U, /* data byte transmitted in slave mode, ACK received */
	I2C_SLAVE_DATA_TRAN_NACK	= 0xC0U, /* data byte transmitted in slave mode, ACK not received */
	I2C_SLAVE_LAST_TRAN_ACK		= 0xC8U, /* last byte transmitted in slave mode, ACK received */
	I2C_SEC_ADDR_WR_ACK			= 0xD0U, /* second address byte + write bit transmitted, ACK received */
	I2C_SEC_ADDR_WR_NACK		= 0xD8U, /* second address byte + write bit transmitted, ACK not received */
	I2C_NO_STATUS_INFO			= 0xF8U	 /* no relevant status information, IRQ_FLAG = 0 */
} I2C_IRQStatus;

/* I2Cx->I2C_CLK_CTRL, R/W */
#define I2C_CLK_M_SHIFT			3
#define I2C_CLK_M_MASK			(0xFU << I2C_CLK_M_SHIFT)
#define I2C_CLK_M_MAX			15

#define I2C_CLK_N_SHIFT			0
#define I2C_CLK_N_MASK			(0x7U << I2C_CLK_N_SHIFT)
#define I2C_CLK_N_MAX			7

/* I2Cx->I2C_SOFT_RST, R/W */
#define I2C_SOFT_RST_BIT		HAL_BIT(0)

/* I2Cx->I2C_EFR, R/W */
#define I2C_DATA_BYTE_MASK		0x3U
typedef enum {
	I2C_DATA_BYTE_0	= 0U,
	I2C_DATA_BYTE_1	= 1U,
	I2C_DATA_BYTE_2	= 2U,
	I2C_DATA_BYTE_3	= 3U
} I2C_DataByte;

/* I2Cx->I2C_LINE_CTRL, R/W */
#define I2C_SCL_STATE_BIT		HAL_BIT(5)
#define I2C_SDA_STATE_BIT		HAL_BIT(4)
#define I2C_SCL_CTRL_BIT		HAL_BIT(3)
#define I2C_SCL_CTRL_EN_BIT		HAL_BIT(2)
#define I2C_SDA_CTRL_BIT		HAL_BIT(1)
#define I2C_SDA_CTRL_EN_BIT		HAL_BIT(0)

/******************************************************************************/

/**
 * @brief I2C addressing mode
 */
typedef enum {
	I2C_ADDR_MODE_7BIT	= 0U, /* 7-bit addressing mode */
	I2C_ADDR_MODE_10BIT	= 1U  /* 10-bit addressing mode */
} I2C_AddrMode;

/**
 * @brief I2C initialization parameters
 */
typedef struct {
	I2C_AddrMode	addrMode;  /* addressing mode */
	uint32_t		clockFreq; /* clock frequency */
} I2C_InitParam;

HAL_Status HAL_I2C_Init(I2C_ID i2cID, const I2C_InitParam *initParam);
HAL_Status HAL_I2C_DeInit(I2C_ID i2cID);

int32_t HAL_I2C_Master_Transmit_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t *buf, int32_t size);
int32_t HAL_I2C_Master_Receive_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t *buf, int32_t size);
int32_t HAL_I2C_Master_Transmit_Mem_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size);
int32_t HAL_I2C_Master_Receive_Mem_IT(I2C_ID i2cID, uint16_t devAddr, uint8_t memAddr, uint8_t *buf, int32_t size);

int32_t HAL_I2C_SCCB_Master_Transmit_IT(I2C_ID i2cID, uint8_t devAddr, uint8_t subAddr, uint8_t *buf);
int32_t HAL_I2C_SCCB_Master_Receive_IT(I2C_ID i2cID, uint8_t devAddr, uint8_t subAddr, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif /* _DRIVER_CHIP_HAL_I2C_H_ */
