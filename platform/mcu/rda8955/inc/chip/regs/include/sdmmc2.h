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


#ifndef _SDMMC2_H_
#define _SDMMC2_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'sdmmc'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// SDMMC2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_SDMMC2_BASE              0x01A19000

typedef volatile struct
{
    REG32                          apbi_ctrl_sdmmc2;              //0x00000000
    REG32 Reserved_00000004;                    //0x00000004
    REG32                          APBI_FIFO_TxRx;               //0x00000008
    REG32 Reserved_0000000C[509];               //0x0000000C
    REG32                          SDMMC2_CONFIG;                 //0x00000800
    REG32                          SDMMC2_STATUS;                 //0x00000804
    REG32                          SDMMC2_CMD_INDEX;              //0x00000808
    REG32                          SDMMC2_CMD_ARG;                //0x0000080C
    REG32                          SDMMC2_RESP_INDEX;             //0x00000810
    REG32                          SDMMC2_RESP_ARG3;              //0x00000814
    REG32                          SDMMC2_RESP_ARG2;              //0x00000818
    REG32                          SDMMC2_RESP_ARG1;              //0x0000081C
    REG32                          SDMMC2_RESP_ARG0;              //0x00000820
    REG32                          SDMMC2_DATA_WIDTH;             //0x00000824
    REG32                          SDMMC2_BLOCK_SIZE;             //0x00000828
    REG32                          SDMMC2_BLOCK_CNT;              //0x0000082C
    REG32                          SDMMC2_INT_STATUS;             //0x00000830
    REG32                          SDMMC2_INT_MASK;               //0x00000834
    REG32                          SDMMC2_INT_CLEAR;              //0x00000838
    REG32                          SDMMC2_TRANS_SPEED;            //0x0000083C
    REG32                          SDMMC2_MCLK_ADJUST;            //0x00000840
} HWP_SDMMC2_T;

#define hwp_sdmmc2                   ((HWP_SDMMC2_T*) KSEG1(REG_SDMMC2_BASE))


//apbi_ctrl_sdmmc2
#define SDMMC2_L_ENDIAN(n)           (((n)&7)<<0)
#define SDMMC2_SOFT_RST_L            (1<<3)

//APBI_FIFO_TxRx
#define SDMMC2_DATA_IN(n)            (((n)&0xFFFFFFFF)<<0)
#define SDMMC2_DATA_OUT(n)           (((n)&0xFFFFFFFF)<<0)

//SDMMC2_CONFIG
#define SDMMC2_SDMMC2_SENDCMD         (1<<0)
#define SDMMC2_SDMMC2_SUSPEND         (1<<1)
#define SDMMC2_RSP_EN                (1<<4)
#define SDMMC2_RSP_SEL(n)            (((n)&3)<<5)
#define SDMMC2_RSP_SEL_R2            (2<<5)
#define SDMMC2_RSP_SEL_R3            (1<<5)
#define SDMMC2_RSP_SEL_OTHER         (0<<5)
#define SDMMC2_RD_WT_EN              (1<<8)
#define SDMMC2_RD_WT_SEL             (1<<9)
#define SDMMC2_RD_WT_SEL_READ        (0<<9)
#define SDMMC2_RD_WT_SEL_WRITE       (1<<9)
#define SDMMC2_S_M_SEL               (1<<10)
#define SDMMC2_S_M_SEL_SIMPLE        (0<<10)
#define SDMMC2_S_M_SEL_MULTIPLE      (1<<10)
#define SDMMC2_AUTO_FLAG_EN          (1<<16)

//SDMMC2_STATUS
#define SDMMC2_NOT_SDMMC2_OVER        (1<<0)
#define SDMMC2_BUSY                  (1<<1)
#define SDMMC2_DL_BUSY               (1<<2)
#define SDMMC2_SUSPEND               (1<<3)
#define SDMMC2_RSP_ERROR             (1<<8)
#define SDMMC2_NO_RSP_ERROR          (1<<9)
#define SDMMC2_CRC_STATUS(n)         (((n)&7)<<12)
#define SDMMC2_DATA_ERROR(n)         (((n)&0xFF)<<16)
#define SDMMC2_DAT3_VAL              (1<<24)

//SDMMC2_CMD_INDEX
#define SDMMC2_COMMAND(n)            (((n)&0x3F)<<0)

//SDMMC2_CMD_ARG
#define SDMMC2_ARGUMENT(n)           (((n)&0xFFFFFFFF)<<0)

//SDMMC2_RESP_INDEX
#define SDMMC2_RESPONSE(n)           (((n)&0x3F)<<0)

//SDMMC2_RESP_ARG3
#define SDMMC2_ARGUMENT3(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC2_RESP_ARG2
#define SDMMC2_ARGUMENT2(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC2_RESP_ARG1
#define SDMMC2_ARGUMENT1(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC2_RESP_ARG0
#define SDMMC2_ARGUMENT0(n)          (((n)&0xFFFFFFFF)<<0)

//SDMMC2_DATA_WIDTH
#define SDMMC2_SDMMC2_DATA_WIDTH(n)   (((n)&15)<<0)

//SDMMC2_BLOCK_SIZE
#define SDMMC2_SDMMC2_BLOCK_SIZE(n)   (((n)&15)<<0)

//SDMMC2_BLOCK_CNT
#define SDMMC2_SDMMC2_BLOCK_CNT(n)    (((n)&0xFFFF)<<0)

//SDMMC2_INT_STATUS
#define SDMMC2_NO_RSP_INT            (1<<0)
#define SDMMC2_RSP_ERR_INT           (1<<1)
#define SDMMC2_RD_ERR_INT            (1<<2)
#define SDMMC2_WR_ERR_INT            (1<<3)
#define SDMMC2_DAT_OVER_INT          (1<<4)
#define SDMMC2_TXDMA_DONE_INT        (1<<5)
#define SDMMC2_RXDMA_DONE_INT        (1<<6)
#define SDMMC2_DATA_IND_INT          (1<<7)
#define SDMMC2_NO_RSP_SC             (1<<8)
#define SDMMC2_RSP_ERR_SC            (1<<9)
#define SDMMC2_RD_ERR_SC             (1<<10)
#define SDMMC2_WR_ERR_SC             (1<<11)
#define SDMMC2_DAT_OVER_SC           (1<<12)
#define SDMMC2_TXDMA_DONE_SC         (1<<13)
#define SDMMC2_RXDMA_DONE_SC         (1<<14)

//SDMMC2_INT_MASK
#define SDMMC2_NO_RSP_MK             (1<<0)
#define SDMMC2_RSP_ERR_MK            (1<<1)
#define SDMMC2_RD_ERR_MK             (1<<2)
#define SDMMC2_WR_ERR_MK             (1<<3)
#define SDMMC2_DAT_OVER_MK           (1<<4)
#define SDMMC2_TXDMA_DONE_MK         (1<<5)
#define SDMMC2_RXDMA_DONE_MK         (1<<6)
#define SDMMC2_DATA_IND_MK           (1<<7)
//SDMMC2_INT_CLEAR
#define SDMMC2_NO_RSP_CL             (1<<0)
#define SDMMC2_RSP_ERR_CL            (1<<1)
#define SDMMC2_RD_ERR_CL             (1<<2)
#define SDMMC2_WR_ERR_CL             (1<<3)
#define SDMMC2_DAT_OVER_CL           (1<<4)
#define SDMMC2_TXDMA_DONE_CL         (1<<5)
#define SDMMC2_RXDMA_DONE_CL         (1<<6)
#define SDMMC2_DATA_IND_CL           (1<<7)

//SDMMC2_TRANS_SPEED
#define SDMMC2_SDMMC2_TRANS_SPEED(n)  (((n)&0xFF)<<0)

//SDMMC2_MCLK_ADJUST
#define SDMMC2_SDMMC2_MCLK_ADJUST(n)  (((n)&15)<<0)
#define SDMMC2_CLK_INV               (1<<4)





#endif

