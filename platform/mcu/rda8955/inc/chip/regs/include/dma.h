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


#ifndef _DMA_H_
#define _DMA_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'dma'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// DMA_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_DMA_BASE               0x01A08000

typedef volatile struct
{
    REG32                          get_channel;                  //0x00000000
    REG32                          status;                       //0x00000004
    REG32                          control;                      //0x00000008
    REG32                          src_addr;                     //0x0000000C
    REG32                          dst_addr;                     //0x00000010
    REG32                          sd_dst_addr;                  //0x00000014
    REG32                          xfer_size;                    //0x00000018
    REG32                          pattern;                      //0x0000001C
    REG32                          gea_kc_low;                   //0x00000020
    REG32                          gea_kc_high;                  //0x00000024
    REG32                          gea_messkey;                  //0x00000028
    REG32                          fcs;                          //0x0000002C
} HWP_DMA_T;

#define hwp_dma                    ((HWP_DMA_T*) KSEG1(REG_DMA_BASE))


//get_channel
#define DMA_GET_CHANNEL            (1<<0)

//status
#define DMA_ENABLE                 (1<<0)
#define DMA_INT_DONE_CAUSE         (1<<1)
#define DMA_INT_DONE_STATUS        (1<<2)
#define DMA_CHANNEL_LOCK           (1<<4)

//control
//#define DMA_ENABLE               (1<<0)
#define DMA_INT_DONE_MASK          (1<<1)
#define DMA_INT_DONE_MASK_SHIFT    (1)
#define DMA_INT_DONE_CLEAR         (1<<2)
#define DMA_USE_PATTERN            (1<<4)
#define DMA_STOP_TRANSFER          (1<<8)
#define DMA_GEA_ENABLE             (1<<12)
#define DMA_GEA_ENABLE_DMA         (0<<12)
#define DMA_GEA_ENABLE_GEA         (1<<12)
#define DMA_GEA_ALGORITHM          (1<<13)
#define DMA_GEA_ALGORITHM_GEA1     (0<<13)
#define DMA_GEA_ALGORITHM_GEA2     (1<<13)
#define DMA_GEA_DIRECTION          (1<<14)
#define DMA_GEA_DIRECTION_SHIFT    (14)
#define DMA_FCS_ENABLE             (1<<16)
#define DMA_FCS_ENABLE_NORMAL_DMA  (0<<16)
#define DMA_FCS_ENABLE_FCS_PROCESS (1<<16)
#define DMA_DST_ADDR_MGT(n)        (((n)&3)<<20)
#define DMA_DST_ADDR_MGT_NORMAL_DMA (0<<20)
#define DMA_DST_ADDR_MGT_CONST_ADDR (1<<20)
#define DMA_DST_ADDR_MGT_ALTERN_ADDR (2<<20)
#define DMA_DST_ADDR_MGT_RESERVED  (3<<20)

//src_addr
#define DMA_SRC_ADDRESS(n)         (((n)&0xFFFFFFF)<<0)

//dst_addr
#define DMA_DST_ADDRESS(n)         (((n)&0xFFFFFFF)<<0)

//sd_dst_addr
#define DMA_SD_DST_ADDRESS(n)      (((n)&0xFFFFFFF)<<0)

//xfer_size
#define DMA_TRANSFER_SIZE(n)       (((n)&0x3FFFF)<<0)

//pattern
#define DMA_PATTERN(n)             (((n)&0xFFFFFFFF)<<0)

//gea_kc_low
#define DMA_KC_LSB(n)              (((n)&0xFFFFFFFF)<<0)

//gea_kc_high
#define DMA_KC_MSB(n)              (((n)&0xFFFFFFFF)<<0)

//gea_messkey
#define DMA_MESSKEY(n)             (((n)&0xFFFFFFFF)<<0)

//fcs
#define DMA_FCS(n)                 (((n)&0xFFFFFF)<<0)
#define DMA_FCS_CORRECT            (1<<31)





#endif

