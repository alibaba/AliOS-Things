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


#ifndef _SDMMC_ASM_H_
#define _SDMMC_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'sdmmc'."
#endif



//==============================================================================
// sdmmc
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_SDMMC_BASE             0x01A17000

#define REG_SDMMC_BASE_HI          BASE_HI(REG_SDMMC_BASE)
#define REG_SDMMC_BASE_LO          BASE_LO(REG_SDMMC_BASE)

#define REG_SDMMC_APBI_CTRL_SDMMC  REG_SDMMC_BASE_LO + 0x00000000
#define REG_SDMMC_APBI_FIFO_TXRX   REG_SDMMC_BASE_LO + 0x00000008
#define REG_SDMMC_SDMMC_CONFIG     REG_SDMMC_BASE_LO + 0x00000800
#define REG_SDMMC_SDMMC_STATUS     REG_SDMMC_BASE_LO + 0x00000804
#define REG_SDMMC_SDMMC_CMD_INDEX  REG_SDMMC_BASE_LO + 0x00000808
#define REG_SDMMC_SDMMC_CMD_ARG    REG_SDMMC_BASE_LO + 0x0000080C
#define REG_SDMMC_SDMMC_RESP_INDEX REG_SDMMC_BASE_LO + 0x00000810
#define REG_SDMMC_SDMMC_RESP_ARG3  REG_SDMMC_BASE_LO + 0x00000814
#define REG_SDMMC_SDMMC_RESP_ARG2  REG_SDMMC_BASE_LO + 0x00000818
#define REG_SDMMC_SDMMC_RESP_ARG1  REG_SDMMC_BASE_LO + 0x0000081C
#define REG_SDMMC_SDMMC_RESP_ARG0  REG_SDMMC_BASE_LO + 0x00000820
#define REG_SDMMC_SDMMC_DATA_WIDTH REG_SDMMC_BASE_LO + 0x00000824
#define REG_SDMMC_SDMMC_BLOCK_SIZE REG_SDMMC_BASE_LO + 0x00000828
#define REG_SDMMC_SDMMC_BLOCK_CNT  REG_SDMMC_BASE_LO + 0x0000082C
#define REG_SDMMC_SDMMC_INT_STATUS REG_SDMMC_BASE_LO + 0x00000830
#define REG_SDMMC_SDMMC_INT_MASK   REG_SDMMC_BASE_LO + 0x00000834
#define REG_SDMMC_SDMMC_INT_CLEAR  REG_SDMMC_BASE_LO + 0x00000838
#define REG_SDMMC_SDMMC_TRANS_SPEED REG_SDMMC_BASE_LO + 0x0000083C
#define REG_SDMMC_SDMMC_MCLK_ADJUST REG_SDMMC_BASE_LO + 0x00000840

//apbi_ctrl_sdmmc
#define SDMMC_L_ENDIAN(n)          (((n)&7)<<0)
#define SDMMC_SOFT_RST_L           (1<<3)

//APBI_FIFO_TxRx
#define SDMMC_DATA_IN(n)           (((n)&0xFFFFFFFF)<<0)
#define SDMMC_DATA_OUT(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC_CONFIG
#define SDMMC_SDMMC_SENDCMD        (1<<0)
#define SDMMC_SDMMC_SUSPEND        (1<<1)
#define SDMMC_RSP_EN               (1<<4)
#define SDMMC_RSP_SEL(n)           (((n)&3)<<5)
#define SDMMC_RSP_SEL_R2           (2<<5)
#define SDMMC_RSP_SEL_R3           (1<<5)
#define SDMMC_RSP_SEL_OTHER        (0<<5)
#define SDMMC_RD_WT_EN             (1<<8)
#define SDMMC_RD_WT_SEL            (1<<9)
#define SDMMC_RD_WT_SEL_READ       (0<<9)
#define SDMMC_RD_WT_SEL_WRITE      (1<<9)
#define SDMMC_S_M_SEL              (1<<10)
#define SDMMC_S_M_SEL_SIMPLE       (0<<10)
#define SDMMC_S_M_SEL_MULTIPLE     (1<<10)
#define SDMMC_AUTO_FLAG_EN         (1<<16)

//SDMMC_STATUS
#define SDMMC_NOT_SDMMC_OVER       (1<<0)
#define SDMMC_BUSY                 (1<<1)
#define SDMMC_DL_BUSY              (1<<2)
#define SDMMC_SUSPEND              (1<<3)
#define SDMMC_RSP_ERROR            (1<<8)
#define SDMMC_NO_RSP_ERROR         (1<<9)
#define SDMMC_CRC_STATUS(n)        (((n)&7)<<12)
#define SDMMC_DATA_ERROR(n)        (((n)&0xFF)<<16)
#define SDMMC_DAT3_VAL             (1<<24)

//SDMMC_CMD_INDEX
#define SDMMC_COMMAND(n)           (((n)&0x3F)<<0)

//SDMMC_CMD_ARG
#define SDMMC_ARGUMENT(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC_RESP_INDEX
#define SDMMC_RESPONSE(n)          (((n)&0x3F)<<0)

//SDMMC_RESP_ARG3
#define SDMMC_ARGUMENT3(n)         (((n)&0xFFFFFFFF)<<0)

//SDMMC_RESP_ARG2
#define SDMMC_ARGUMENT2(n)         (((n)&0xFFFFFFFF)<<0)

//SDMMC_RESP_ARG1
#define SDMMC_ARGUMENT1(n)         (((n)&0xFFFFFFFF)<<0)

//SDMMC_RESP_ARG0
#define SDMMC_ARGUMENT0(n)         (((n)&0xFFFFFFFF)<<0)

//SDMMC_DATA_WIDTH
#define SDMMC_SDMMC_DATA_WIDTH(n)  (((n)&15)<<0)

//SDMMC_BLOCK_SIZE
#define SDMMC_SDMMC_BLOCK_SIZE(n)  (((n)&15)<<0)

//SDMMC_BLOCK_CNT
#define SDMMC_SDMMC_BLOCK_CNT(n)   (((n)&0xFFFF)<<0)

//SDMMC_INT_STATUS
#define SDMMC_NO_RSP_INT           (1<<0)
#define SDMMC_RSP_ERR_INT          (1<<1)
#define SDMMC_RD_ERR_INT           (1<<2)
#define SDMMC_WR_ERR_INT           (1<<3)
#define SDMMC_DAT_OVER_INT         (1<<4)
#define SDMMC_TXDMA_DONE_INT       (1<<5)
#define SDMMC_RXDMA_DONE_INT       (1<<6)
#define SDMMC_NO_RSP_SC            (1<<8)
#define SDMMC_RSP_ERR_SC           (1<<9)
#define SDMMC_RD_ERR_SC            (1<<10)
#define SDMMC_WR_ERR_SC            (1<<11)
#define SDMMC_DAT_OVER_SC          (1<<12)
#define SDMMC_TXDMA_DONE_SC        (1<<13)
#define SDMMC_RXDMA_DONE_SC        (1<<14)

//SDMMC_INT_MASK
#define SDMMC_NO_RSP_MK            (1<<0)
#define SDMMC_RSP_ERR_MK           (1<<1)
#define SDMMC_RD_ERR_MK            (1<<2)
#define SDMMC_WR_ERR_MK            (1<<3)
#define SDMMC_DAT_OVER_MK          (1<<4)
#define SDMMC_TXDMA_DONE_MK        (1<<5)
#define SDMMC_RXDMA_DONE_MK        (1<<6)

//SDMMC_INT_CLEAR
#define SDMMC_NO_RSP_CL            (1<<0)
#define SDMMC_RSP_ERR_CL           (1<<1)
#define SDMMC_RD_ERR_CL            (1<<2)
#define SDMMC_WR_ERR_CL            (1<<3)
#define SDMMC_DAT_OVER_CL          (1<<4)
#define SDMMC_TXDMA_DONE_CL        (1<<5)
#define SDMMC_RXDMA_DONE_CL        (1<<6)

//SDMMC_TRANS_SPEED
#define SDMMC_SDMMC_TRANS_SPEED(n) (((n)&0xFF)<<0)

//SDMMC_MCLK_ADJUST
#define SDMMC_SDMMC_MCLK_ADJUST(n) (((n)&15)<<0)
#define SDMMC_CLK_INV              (1<<4)




#endif
