/*************************************************
  Copyright (C), 2011-2015 ZheJiang Dahua Technology Co., Ltd.
  文件名:   lcsfc.h
  作  者:   zheng_xingjian(11853)<zheng_xingjian@dahuatech.com>
  版  本:   V1.0.0
  日  期：  2016-02-22
  描  述:   LCSFC模块头文件
  
            1、使用说明
            无
           
            2、局限性
            仅适用于eCos3.0
  
  修订历史:
  1. 日    期: 
     修订版本:
     作    者:
     修订备注:
     
  2. 日    期:
     修订版本:
     作    者:
     修订备注:
*************************************************/
#ifndef _LCSFC_H_
#define _LCSFC_H_


#define DAHUA_FLAG		            0x0C646874

/* */
#define LCSFC_REG_BASE              (0x03000000)

#define LCSFC_WRITE_HOLD_BIT        (23)
#define LCSFC_WRITE_HOLD            (1 << LCSFC_WRITE_HOLD_BIT)
#define LCSFC_WRTIE_HOLD_ADDLINE    (LCSFC_REG_BASE | LCSFC_WRITE_HOLD)

#define LCSFC_KV_ADDR               (0x030f0800)
#define LCSFC_CODE_ADDR             (0x03001000)

#define LCSFC_CMD_ERASE             (0x20000000)
#define LCSFC_CMD_PROGPAGE          (0x02000000)

#define LCSFC_ADDR_MASK             (0x7FFFFF)
#define LCSFC_CMD_MASK              (0xFF000000)

/* 
为便于硬件设计，LCSFC没有查询Flash状态操作，因此，查忙
用读取Flash的0x00地址，此地址为Flash的大华标志区域。
因硬件为提高读效率，会缓冲一个SPI FIFO深度的Flash数据在
LCSFC内部，所以，如果连续读0x00地址的话，仅会读缓冲的数据，而不是
真实的Flash数据，因此，查忙首先要读取一下0x1000这个地址，
此时内部缓冲被替换成了0x1000到0x1020地址的数据，再读取0x00这个地址，
已确保是真实Flash中的数据
*/
#define LCSFC_CHK_FLAGADDR          (LCSFC_REG_BASE )
#define LCSFC_CHK_FLUSHADDR         (LCSFC_REG_BASE + 0x1000)

/* SPI能力 */
#define LCSFC_TX_FIFO_DEPTH         (92 )
#define LCSFC_RX_FIFO_DEPTH         (32 )
#define LCSFC_FLASH_MAX_SIZE        (4 * 1024 * 1024)
#define LCSFC_FLASH_PAGE_SIZE       (0x100)



/* 波特率配置寄存器 */
#define LCSFC_REG_BAUDRATE          (0x11003104)
#define LCSFC_REG_SAMDLY            (0x11003108)


#define SPI_PAGE_MASK               0xFF
#define SPI_BLOCK_MASK              0xFFF
#define SPI_BLOCK_NOTMASK           (~SPI_BLOCK_MASK)
#define SPI_SECTOR_MASK             0xFFFF

#define SPI_BLOCK_SIZE              0x1000

/* protection command */
#define SPI_CMD_WRITE_EN            0x06
#define SPI_CMD_WRITE_DIS           0x04
#define SPI_CMD_4KBLOCKE            0x20
#define SPI_CMD_PAGE_PROG           0x02

#endif /* #define _LCSFC_H_ */
