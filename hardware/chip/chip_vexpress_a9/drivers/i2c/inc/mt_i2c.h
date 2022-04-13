/*
 * MediaTek Inc. (C) 2018. All rights reserved.
 *
 * Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein is
 * confidential and proprietary to MediaTek Inc. and/or its licensors. Without
 * the prior written permission of MediaTek inc. and/or its licensors, any
 * reproduction, modification, use or disclosure of MediaTek Software, and
 * information contained herein, in whole or in part, shall be strictly
 * prohibited.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER
 * ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH
 * RESPECT TO THE SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY,
 * INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES
 * TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO.
 * RECEIVER EXPRESSLY ACKNOWLEDGES THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO
 * OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES CONTAINED IN MEDIATEK
 * SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE
 * RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S
 * ENTIRE AND CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE
 * RELEASED HEREUNDER WILL BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE
 * MEDIATEK SOFTWARE AT ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE
 * CHARGE PAID BY RECEIVER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek
 * Software") have been modified by MediaTek Inc. All revisions are subject to
 * any receiver's applicable license agreements with MediaTek Inc.
 */

/**
 * @file mt_i2c.h
 * This i2c header file is used for i2c driver. It declares\n
 * the external functions which will be used in LK.
 */

/**
 * @defgroup IP_group_i2c I2C
 *
 *   @{
 *       @defgroup IP_group_i2c_external EXTERNAL
 *         The external API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_external_function 1.function
 *              External function in i2c driver.
 *            @defgroup IP_group_i2c_external_struct 2.structure
 *              none.
 *            @defgroup IP_group_i2c_external_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_external_enum 4.enumeration
 *              none.
 *            @defgroup IP_group_i2c_external_def 5.define
 *              none.
 *         @}
 *
 *       @defgroup IP_group_i2c_internal INTERNAL
 *         The internal API document for I2C. \n
 *
 *         @{
 *            @defgroup IP_group_i2c_internal_function 1.function
 *              none.
 *            @defgroup IP_group_i2c_internal_struct 2.structure
 *              Internal structure in i2c driver.
 *            @defgroup IP_group_i2c_internal_typedef 3.typedef
 *              none.
 *            @defgroup IP_group_i2c_internal_enum 4.enumeration
 *              Internal enumeration in i2c driver.
 *            @defgroup IP_group_i2c_internal_def 5.define
 *              Internal define in i2c driver.
 *         @}
 *   @}
 */

#ifndef __MTK_I2C_H__
#define __MTK_I2C_H__
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "mt_reg_base.h"
//#define CONFIG_MTK_FPGA

/** @ingroup IP_group_i2c_internal_def
 * @{
 */
#define MTK_I2C_SOURCE_CLK 124800
#define MTK_I2C_CLK_DIV 1
#define I2C_DEFAULT_CLK_DIV 2
#define I2C_CLK_DIV_100K 10
#define I2C_TIMING_100K 0x126
#define I2C_CLK_DIV_400K I2C_DEFAULT_CLK_DIV
#define I2C_TIMING_400K 0x637
#define I2C_CLK_DIV_1000K I2C_DEFAULT_CLK_DIV
#define I2C_TIMING_1000K 0x407

#define MTK_I2C_ULPOSC_DIV8 32750
#define MTK_I2C_ULPOSC_DIV16 16375
#define MTK_I2C_CLK_26M 26000

#define I2CTAG "[I2C-LK] "
#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(x, y) (((x) + ((y) - 1)) / (y))
#endif
#define I2C_NR		3 /* Number of I2C controllers */

#define I2CBIT(nr) (1UL << (nr))
#define I2CBITMASK(a, b) (I2CBIT(a+1) - I2CBIT(b))

#define I2C_POLL_VALUE 0xfffff

#define MTK_I2C_NUM 3

#define MTK_I2C0_CLK_OFFSET  (0x1 << 24)
#define MTK_I2C1_CLK_OFFSET  (0x1 << 25)
#define MTK_I2C2_CLK_OFFSET  (0x1 << 26)
#define MTK_APDMA_CLK_OFFSET (0x1 << 18)

// #define MTK_I2C_CLK_SET  (0x100010C0)
// #define MTK_I2C_CLK_CLR  (0x100010C4)
// #define MTK_I2C_CLK_STA  (0x100010C8)
// #define MTK_I2C_CLK_MAIN  (0x100000A8)
// #define MTK_APDMA_CLK_SET (0x10001088)
// #define MTK_APDMA_CLK_CLR (0x1000108C)
// #define MTK_APDMA_CLK_STA (0x10001094)

#define MTK_GPIO_SDA0 12
#define MTK_GPIO_SCL0 15
#define MTK_GPIO_SDA1 18
#define MTK_GPIO_SCL1 21
#define MTK_GPIO_SDA2 24
#define MTK_GPIO_SCL2 27

#define MTK_GPIO_PULL_ENABLE_SDA0 0
#define MTK_GPIO_PULL_ENABLE_SCL0 1
#define MTK_GPIO_PULL_ENABLE_SDA1 2
#define MTK_GPIO_PULL_ENABLE_SCL1 3
#define MTK_GPIO_PULL_ENABLE_SDA2 4
#define MTK_GPIO_PULL_ENABLE_SCL2 5

#define MTK_GPIO_RESL_SDA0 0
#define MTK_GPIO_RESL_SCL0 2
#define MTK_GPIO_RESL_SDA1 4
#define MTK_GPIO_RESL_SCL1 6
#define MTK_GPIO_RESL_SDA2 8
#define MTK_GPIO_RESL_SCL2 10

#define I2C_CONTROL_RS			I2CBIT(1)
#define I2C_CONTROL_DMA_EN		I2CBIT(2)
#define I2C_CONTROL_CLK_EXT_EN		I2CBIT(3)
#define I2C_CONTROL_DIR_CHANGE		I2CBIT(4)
#define I2C_CONTROL_ACKERR_DET_EN	I2CBIT(5)
#define I2C_CONTROL_TRANSFER_LEN_CHANGE	I2CBIT(6)
#define I2C_CONTROL_AYNCS_MODE		I2CBIT(9)

#define I2C_RS_TRANSFER			I2CBIT(4)
#define I2C_ARB_LOSE			I2CBIT(3)
#define I2C_ACKERR			    I2CBIT(1)
#define I2C_TRANSAC_COMP		I2CBIT(0)
#define I2C_TRANSAC_START		I2CBIT(0)
#define I2C_RS_MUL_CNFG			I2CBIT(15)
#define I2C_RS_MUL_TRIG			I2CBIT(14)
#define I2C_SOFT_RST			0x0001
#define I2C_FIFO_ADDR_CLR		0x0001

#define I2C_IO_CONFIG_OPEN_DRAIN	0x0003
#define I2C_IO_CONFIG_PUSH_PULL		0x0000
#define I2C_CONTROL_DEFAULT		0x0d00
#define I2C_DELAY_LEN			0x0002
#define I2C_ST_START_CON		0x8001
#define I2C_FS_START_CON		0x1800
#define I2C_DCM_OPEN			0x0003
#define I2C_WRRD_TRANAC_VALUE		0x0002
#define I2C_M_RD			0x0001

#define I2C_DMA_CON_TX			0x0000
#define I2C_DMA_CON_RX			0x0001
#define I2C_DMA_START_EN		0x0001
#define I2C_DMA_INT_FLAG_NONE		0x0000
#define I2C_DMA_CLR_FLAG		0x0000
#define I2C_DMA_HARD_RST		0x0002

#define I2C_FIFO_SIZE			16
#define I2C_DEFAULT_SPEED		100
#define MAX_FS_MODE_SPEED		400
#define MAX_FS_PLUS_SPEED		1000
#define MAX_SAMPLE_CNT_DIV		8
#define MAX_STEP_CNT_DIV		64
#define MAX_HS_STEP_CNT_DIV		8
#define I2C_TIME_DEFAULT_VALUE		0x0003

#define I2C_OK				0
#define ENXIO_I2C			6
#define EINVAL_I2C			22
#define ETIMEDOUT_I2C			110
#define EREMOTEIO_I2C			121

#define I2C_FIFO_FORCE			I2CBIT(0)
#define I2C_DCM_ENABLE			I2CBIT(1)
#define I2C_CONTI_TRANS			I2CBIT(2)
#define I2C_EXTEN_SET			I2CBIT(3)
#define I2C_ACTIME_SET			I2CBIT(4)
#define I2C_MULTI_TRANS			I2CBIT(5)
#define I2C_MULTI_STOP			I2CBIT(6)
#define I2C_CLOCK_STRETCH		I2CBIT(7)
/** @}
 */
 #define dsb() __asm__ __volatile__ ("dsb" : : : "memory")

#define mb()    \
    do {    \
        dsb();    \
    } while (0)

#define DRV_Reg32(addr)             (*(volatile unsigned int *)(addr))
#define DRV_WriteReg32(addr, data)    \
    do {    \
        ((*(volatile unsigned int *)(addr)) = (unsigned int)data);    \
        mb();    \
    } while(0)

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C controller register offset.
 */
enum I2C_REGS_OFFSET {
	OFFSET_DATA_PORT = 0x0,
	OFFSET_SLAVE_ADDR = 0x04,
	OFFSET_INTR_MASK = 0x08,
	OFFSET_INTR_STAT = 0x0c,
	OFFSET_CONTROL = 0x10,
	OFFSET_TRANSFER_LEN = 0x14,
	OFFSET_TRANSAC_LEN = 0x18,
	OFFSET_DELAY_LEN = 0x1c,
	OFFSET_TIMING = 0x20,
	OFFSET_START = 0x24,
	OFFSET_EXT_CONF = 0x28,
	OFFSET_FIFO_STAT1 = 0x2c,
	OFFSET_FIFO_STAT = 0x30,
	OFFSET_FIFO_THRESH = 0x34,
	OFFSET_FIFO_ADDR_CLR = 0x38,
	OFFSET_IO_CONFIG = 0x40,
	OFFSET_MULTI_MASTER = 0x44,
	OFFSET_HS = 0x48,
	OFFSET_SOFTRESET = 0x50,
	OFFSET_DCM_EN = 0x54,
	OFFSET_DEBUGSTAT = 0x64,
	OFFSET_DEBUGCTRL = 0x68,
	OFFSET_TRANSFER_LEN_AUX = 0x6c,
	OFFSET_CLOCK_DIV = 0x70,
	OFFSET_SCL_HL_RATIO = 0x74,
	OFFSET_HS_SCL_HL_RATIO = 0x78,
	OFFSET_SCL_MIS_COMP_POINT = 0x7C,
	OFFSET_STA_STOP_AC_TIME = 0x80,
	OFFSET_HS_STA_STOP_AC_TIME = 0x84,
	OFFSET_SDA_TIME = 0x88,
	OFFSET_FIFO_PAUSE = 0x8C,
};

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C transfer operation mode.
 */
enum mtk_trans_op {
	I2C_MASTER_WR = 1,
	I2C_MASTER_RD,
	I2C_MASTER_WRRD,
};

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C GDMA register offset.
 */
enum DMA_REGS_OFFSET {
	OFFSET_DMA_INT_FLAG       = 0x0,
	OFFSET_DMA_INT_EN         = 0x04,
	OFFSET_DMA_EN             = 0x08,
	OFFSET_DMA_RST            = 0x0C,
	OFFSET_DMA_CON            = 0x18,
	OFFSET_DMA_TX_MEM_ADDR    = 0x1C,
	OFFSET_DMA_RX_MEM_ADDR    = 0x20,
	OFFSET_DMA_TX_LEN         = 0x24,
	OFFSET_DMA_RX_LEN         = 0x28,
};

/** @ingroup IP_group_i2c_internal_enum
 * @brief I2C bus num.
 */
enum mtk_i2c_bus_num {
	I2C0 = 0,
	I2C1 = 1,
	I2C2 = 2,
};

/** @ingroup IP_group_i2c_internal_struct
 * @brief Struct keeping i2c message data.
 */
struct i2c_msg {
	/** slave address */
	uint16_t addr;
	/** i2c transfer operation mode */
	uint16_t flags;
	/** msg length */
	uint16_t len;
	/** pointer to msg data */
	uint8_t *buf;
};

/** @ingroup IP_group_i2c_internal_struct
 * @brief Struct keeping i2c driver data.
 */
struct mtk_i2c {
	/** dma mode flag */
	bool dma_en;
	/** polling mode flag */
	bool poll_en;
	/** IO config push-pull mode */
	bool pushpull;
	/** filter error message */
	bool filter_msg;
	/** multi-transfer repeated start enable */
	bool auto_restart;
	bool zero_len;
	bool msg_complete;
	/** slave device 7bits address */
	uint8_t addr;
	/** i2c bus number */
	uint8_t id;
	uint8_t mode;
	uint16_t irqnr;
	/** i2c interrupt status */
	uint16_t irq_stat;
	/** clock_div register value */
	uint16_t clock_div_reg;
	/** timing register value */
	uint16_t timing_reg;
	uint16_t high_speed_reg;
	uint16_t con_num;
	uint16_t delay_len;
	uint16_t ext_time;
	uint16_t scl_ratio;
	uint16_t hs_scl_ratio;
	uint16_t scl_mis_comp;
	uint16_t sta_stop_time;
	uint16_t hs_sta_stop_time;
	uint16_t sda_time;
	/** i2c base address */
	uint32_t base;
	/** i2c dma base address */
	uint32_t dmabase;
	/** source clock KHz */
	uint32_t clk;
	/** source clock divide */
	uint32_t clk_src_div;
	/** i2c transfer speed */
	uint32_t speed;
	uint32_t timeout;
	/** i2c transfer operation mode */
	enum mtk_trans_op op;
};

/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, then read data from\n
 *     slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[out]
 *     buffer: read data buffer pointer.
 * @param[in]
 *     len: read data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_read(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		 uint8_t *buffer, uint16_t len);

/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, then write data to\n
 *     slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[in]
 *     buffer: write data buffer pointer.
 * @param[in]
 *     len: write data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.\n
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_write(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		  uint8_t *buffer, uint16_t len);

/** @ingroup IP_group_i2c_external_function
 * @par Description
 *     Initialize struct mtk_i2c and i2c_msg, first write data to\n
 *     slave device then read data from slave device.
 * @param[in]
 *     bus_num: i2c bus number.
 * @param[in]
 *     device_addr: slave device 7bits address.
 * @param[in]
 *     speed_khz: i2c transfer speed.
 * @param[in]
 *     write_buffer: write data buffer pointer.
 * @param[out]
 *     read_buffer: read data buffer pointer.
 * @param[in]
 *     write_len: write data length.
 * @param[in]
 *     read_len: read data length.
 * @return
 *     0, i2c transfer successfully.\n
 *     error code from mtk_i2c_transfer().
 * @par Boundary case and Limitation
 *     none.
 * @par Error case and Error handling
 *     If mtk_i2c_transfer() fails, return its error code.\n
 * @par Call graph and Caller graph
 * @par Refer to the source code
 */
int mtk_i2c_write_read(uint8_t bus_num, uint8_t device_addr, uint32_t speed_khz,
		       uint8_t *write_buffer, uint8_t *read_buffer,
		       uint16_t write_len, uint16_t read_len);

int mtk_i2c_transfer(struct mtk_i2c *i2c, struct i2c_msg *msgs, int num);
int I2C_Test(void);
void i2c_cli_init(void);
#endif /* __I2C_TEST_H__ */
