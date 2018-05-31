/******************************************************************************

   Copyright (C) 2011-2014 ZheJiang Dahua Technology CO.,LTD.

 ******************************************************************************
  File Name     : I2C_reg.h
  Version       : Initial Draft
  Author        : Zxj
  Created       : 2014.4.29
  Last Modified :
  Description   : I2C register define
  Function List :
  DHstory       :
  1.Date        : 2014/4/29
    Author      : 11853
    Modification: Create
******************************************************************************/

#ifndef __I2C_REG_H__
#define __I2C_REG_H__
    
    
#ifdef __cplusplus
#if __cplusplus
    extern "C"{
#endif
#endif /* __cplusplus */

#define I2C_DEV_NUM             (1)
#define DHC_I2C0_REG_BASE       (0x14060000)
#define DHC_I2C1_REG_BASE       (0)
#define DHC_I2C2_REG_BASE       (0)
#define DHC_I2C_REG_SIZE        (0xfff)
#define DHC_IRQ_I2C0            (8)
#define DHC_IRQ_I2C1            (-1)
#define DHC_IRQ_I2C2            (-1)

#ifdef I2C_CLK
#undef I2C_CLK
#endif

#define I2C_CLK     (24 * 1000000)  //54

#ifdef I2C_SPEED
#undef I2C_SPEED
#endif

#define I2C_SPEED   (400*1000)

/*fifo深度*/
#define I2C_BUF_DEP ( 8 )

/*一次i2c操作读写数据的最大个数*/
#define I2C_RW_MAX ( 64 )

/*发送FIFO中数据个数，低于此数，会产生中断*/
#define I2C_FIFO_DATA_CNT ( 4 )

#define BIT(x)                  (1 << (x))


#define I2C_REG_CON                     (0x00)
#define I2C_REG_TAR                     (0x04)
#define I2C_REG_DATA_CMD                (0x10)
#define I2C_REG_SS_SCL_HCNT             (0x14)
#define I2C_REG_SS_SCL_LCNT             (0x18)
#define I2C_REG_FS_SCL_HCNT             (0x1c)
#define I2C_REG_FS_SCL_LCNT             (0x20)
#define I2C_REG_HS_SCL_HCNT             (0x24)
#define I2C_REG_HS_SCL_LCNT             (0x28)
#define I2C_REG_INTR_STAT               (0x2c)
#define I2C_REG_INTR_MASK               (0x30)
#define I2C_REG_RAW_INTSTATUS           (0x34)
#define I2C_REG_RX_TL                   (0x38)
#define I2C_REG_TX_TL                   (0x3c)
#define I2C_REG_CLR_INTR                (0x40)
#define I2C_REG_CLR_RX_UNDER            (0x44)
#define I2C_REG_CLR_RX_OVER             (0x48)
#define I2C_REG_CLR_TX_OVER             (0x4c)
#define I2C_REG_CLR_RD_REQ              (0x50)
#define I2C_REG_CLR_TX_ABRT             (0x54)
#define I2C_REG_CLR_RX_DONE             (0x58)
#define I2C_REG_CLR_ACTIVITY            (0x5c)
#define I2C_REG_CLR_STOP_DET            (0x60)
#define I2C_REG_CLR_START_DET           (0x64)
#define I2C_REG_ENABLE                  (0x6c)
#define I2C_REG_STATUS                  (0x70)
#define I2C_REG_TXFLR                   (0x74)
#define I2C_REG_RXFLR                   (0x78)
#define I2C_REG_TX_ABRT_SOURCE          (0x80)
#define I2C_REG_DMA_CR                  (0x88)
#define I2C_REG_DMA_TDLR                (0x8c)
#define I2C_REG_DMA_RDLR                (0x90)
#define I2C_REG_ENABLE_STATUS           (0x9c)
#define I2C_REG_START                   (0xa0)

#define I2C_REG_BIT_INT_STOP            BIT(9)
#define I2C_REG_BIT_INT_FREE            BIT(8)
#define I2C_REG_BIT_INT_TX_ABRT         BIT(6)

#define I2C_ENABLES     (1)
#define I2C_DISABLES    (0)
#define I2C_START       (1)
#define I2C_STOP        (0)

/*
#define IIC_REG_BIT_SPEED_STANDARD      0x1
#define IIC_REG_BIT_SPEED_FAST          0x2
#define IIC_REG_BIT_SPEED_HIGH          0x3
*/
/* IC_CON */
#define I2C_REG_BIT_MASTER_MODE         BIT(0)
#define I2C_REG_BIT_SPDMD_FAST          BIT(2)
#define I2C_REG_BIT_10_ADDR             BIT(4)
#define I2C_REG_BIT_RESTART_EN          BIT(5)

#define I2C_REG_BITSHF_SPDMODE          (0x01)
#define I2C_REG_BITMASK_SPDMODE         (~0x06)

/* IC_STATUS */
#define I2C_REG_BITMASK_ACTIVITY        (0x01)
#define I2C_REG_BIT_ACTIVITY            (0x01)

/* IC_DATA_CMD */
#define I2C_REG_BIT_CDM_READ            BIT(8)


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */


#endif /* __I2C_REG_H__ */
