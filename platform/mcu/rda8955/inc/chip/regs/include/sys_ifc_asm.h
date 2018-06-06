/* Copyright (C) 2016 RDA Technologies Limited and/or its affiliates("RDA").
* All rights reserved.
*
* This software is supplied "AS IS" without any warranties.
* RDA assumes no responsibility or liability for the use of the software,
* conveys no license or title under any patent, copyright, or mask work
* right to the product. RDA reserves the right to make changes in the
* software without notification.  RDA also make no representation or
* warranty that such application will be suitable for the specified use
* without further testing or modification.
*/


#ifndef _SYS_IFC_ASM_H_
#define _SYS_IFC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'sys_ifc'."
#endif

#include "globals_asm.h"
#include "gallite_generic_config_asm.h"

#define SYS_IFC_ADDR_ALIGN                      (0)
#define SYS_IFC_TC_LEN                          (23)
#define SYS_IFC_STD_CHAN_NB                     (SYS_IFC_NB_STD_CHANNEL)
#define SYS_IFC_RFSPI_CHAN                      (1)

//==============================================================================
// sys_ifc
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SYS_IFC_BASE           0x01A09000

#define REG_SYS_IFC_BASE_HI        BASE_HI(REG_SYS_IFC_BASE)
#define REG_SYS_IFC_BASE_LO        BASE_LO(REG_SYS_IFC_BASE)

#define REG_SYS_IFC_GET_CH         REG_SYS_IFC_BASE_LO + 0x00000000
#define REG_SYS_IFC_DMA_STATUS     REG_SYS_IFC_BASE_LO + 0x00000004
#define REG_SYS_IFC_DEBUG_STATUS   REG_SYS_IFC_BASE_LO + 0x00000008
#define REG_SYS_IFC_STD_CH_0_CONTROL REG_SYS_IFC_BASE_LO + 0x00000010
#define REG_SYS_IFC_STD_CH_0_STATUS REG_SYS_IFC_BASE_LO + 0x00000014
#define REG_SYS_IFC_STD_CH_0_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000018
#define REG_SYS_IFC_STD_CH_0_TC    REG_SYS_IFC_BASE_LO + 0x0000001C
#define REG_SYS_IFC_STD_CH_1_CONTROL REG_SYS_IFC_BASE_LO + 0x00000020
#define REG_SYS_IFC_STD_CH_1_STATUS REG_SYS_IFC_BASE_LO + 0x00000024
#define REG_SYS_IFC_STD_CH_1_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000028
#define REG_SYS_IFC_STD_CH_1_TC    REG_SYS_IFC_BASE_LO + 0x0000002C
#define REG_SYS_IFC_STD_CH_2_CONTROL REG_SYS_IFC_BASE_LO + 0x00000030
#define REG_SYS_IFC_STD_CH_2_STATUS REG_SYS_IFC_BASE_LO + 0x00000034
#define REG_SYS_IFC_STD_CH_2_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000038
#define REG_SYS_IFC_STD_CH_2_TC    REG_SYS_IFC_BASE_LO + 0x0000003C
#define REG_SYS_IFC_STD_CH_3_CONTROL REG_SYS_IFC_BASE_LO + 0x00000040
#define REG_SYS_IFC_STD_CH_3_STATUS REG_SYS_IFC_BASE_LO + 0x00000044
#define REG_SYS_IFC_STD_CH_3_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000048
#define REG_SYS_IFC_STD_CH_3_TC    REG_SYS_IFC_BASE_LO + 0x0000004C
#define REG_SYS_IFC_STD_CH_4_CONTROL REG_SYS_IFC_BASE_LO + 0x00000050
#define REG_SYS_IFC_STD_CH_4_STATUS REG_SYS_IFC_BASE_LO + 0x00000054
#define REG_SYS_IFC_STD_CH_4_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000058
#define REG_SYS_IFC_STD_CH_4_TC    REG_SYS_IFC_BASE_LO + 0x0000005C
#define REG_SYS_IFC_STD_CH_5_CONTROL REG_SYS_IFC_BASE_LO + 0x00000060
#define REG_SYS_IFC_STD_CH_5_STATUS REG_SYS_IFC_BASE_LO + 0x00000064
#define REG_SYS_IFC_STD_CH_5_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000068
#define REG_SYS_IFC_STD_CH_5_TC    REG_SYS_IFC_BASE_LO + 0x0000006C
#define REG_SYS_IFC_STD_CH_6_CONTROL REG_SYS_IFC_BASE_LO + 0x00000070
#define REG_SYS_IFC_STD_CH_6_STATUS REG_SYS_IFC_BASE_LO + 0x00000074
#define REG_SYS_IFC_STD_CH_6_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000078
#define REG_SYS_IFC_STD_CH_6_TC    REG_SYS_IFC_BASE_LO + 0x0000007C
#define REG_SYS_IFC_CH_RFSPI_CONTROL REG_SYS_IFC_BASE_LO + 0x00000080
#define REG_SYS_IFC_CH_RFSPI_STATUS REG_SYS_IFC_BASE_LO + 0x00000084
#define REG_SYS_IFC_CH_RFSPI_START_ADDR REG_SYS_IFC_BASE_LO + 0x00000088
#define REG_SYS_IFC_CH_RFSPI_END_ADDR REG_SYS_IFC_BASE_LO + 0x0000008C
#define REG_SYS_IFC_CH_RFSPI_TC    REG_SYS_IFC_BASE_LO + 0x00000090

//get_ch
#define SYS_IFC_CH_TO_USE(n)       (((n)&15)<<0)
#define SYS_IFC_CH_TO_USE_MASK     (15<<0)
#define SYS_IFC_CH_TO_USE_SHIFT    (0)

//dma_status
#define SYS_IFC_CH_ENABLE(n)       (((n)&0xFF)<<0)
#define SYS_IFC_CH_BUSY(n)         (((n)&0x7F)<<16)

//debug_status
#define SYS_IFC_DBG_STATUS         (1<<0)

//control
#define SYS_IFC_ENABLE             (1<<0)
#define SYS_IFC_DISABLE            (1<<1)
#define SYS_IFC_CH_RD_HW_EXCH      (1<<2)
#define SYS_IFC_CH_WR_HW_EXCH      (1<<3)
#define SYS_IFC_AUTODISABLE        (1<<4)
#define SYS_IFC_SIZE               (1<<5)
#define SYS_IFC_REQ_SRC(n)         (((n)&31)<<8)
#define SYS_IFC_REQ_SRC_MASK       (31<<8)
#define SYS_IFC_REQ_SRC_SHIFT      (8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SCI (0<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SCI (1<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SPI1 (2<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SPI1 (3<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SPI2 (4<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SPI2 (5<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SPI3 (6<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SPI3 (7<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_DEBUG_UART (8<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_DEBUG_UART (9<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_UART (10<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_UART (11<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_UART2 (12<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_UART2 (13<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SDMMC (14<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SDMMC (15<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_FREE (16<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_CAMERA (17<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_TX_SDMMC2 (18<<8)
#define SYS_IFC_REQ_SRC_SYS_ID_RX_SDMMC2 (19<<8)
#define SYS_IFC_FLUSH              (1<<16)
#define SYS_IFC_MAX_BURST_LENGTH(n) (((n)&3)<<17)

//status
//#define SYS_IFC_ENABLE           (1<<0)
#define SYS_IFC_FIFO_EMPTY         (1<<4)

//start_addr
#define SYS_IFC_START_ADDR(n)      (((n)&0x3FFFFFF)<<0)

//tc
#define SYS_IFC_TC(n)              (((n)&0x7FFFFF)<<0)

//ch_rfspi_control
//#define SYS_IFC_ENABLE           (1<<0)
//#define SYS_IFC_DISABLE          (1<<1)

//ch_rfspi_status
//#define SYS_IFC_ENABLE           (1<<0)
//#define SYS_IFC_FIFO_EMPTY       (1<<4)
#define SYS_IFC_FIFO_LEVEL(n)      (((n)&31)<<8)

//ch_rfspi_start_addr
#define SYS_IFC_START_AHB_ADDR(n)  (((n)&0x3FFFFFF)<<0)

//ch_rfspi_end_addr
#define SYS_IFC_END_AHB_ADDR(n)    (((n)&0x3FFFFFF)<<0)

//ch_rfspi_tc
#define SYS_IFC_CH_RFSPI_TC(n)     (((n)&0x3FFF)<<0)




#endif
