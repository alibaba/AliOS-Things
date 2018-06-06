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


#ifndef _VOC_CFG_H_
#define _VOC_CFG_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'voc_cfg'."
#endif


#include "voc_global.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// VOC_CFG_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_VOC_CFG_BASE           0x0195FF80

typedef volatile struct
{
    REG16                          Ctrl;                         //0x00000000
    /// Should be configured by VoC_core only (not by a CPU)! If VoC DMA is used
    /// by the CPU the equivalent voc_ahb registers should be configured.
    REG16                          DMA_Wrap;                     //0x00000002
    /// Should be configured by VoC_core only (not by a CPU)! If VoC DMA is used
    /// by the CPU the equivalent voc_ahb registers should be configured.
    REG16                          DMA_Size;                     //0x00000004
    /// Should be configured by VoC_core only (not by a CPU)! If VoC DMA is used
    /// by the CPU the equivalent voc_ahb registers should be configured.
    REG16                          DMA_Laddr;                    //0x00000006
    /// Should be configured by VoC_core only (not by a CPU)! If VoC DMA is used
    /// by the CPU the equivalent voc_ahb registers should be configured.
    REG32                          DMA_EAddr;                    //0x00000008
    REG32                          DMA_Data_Single;              //0x0000000C
    REG32                          DMA_2D;                       //0x00000010
    REG32 Reserved_00000014;                    //0x00000014
    REG16                          DAI_Data_In;                  //0x00000018
    REG16                          DAI_Data_Out;                 //0x0000001A
    REG16                          ROM_Page;                     //0x0000001C
    REG16                          Debug;                        //0x0000001E
    REG32                          BIST_Ctrl;                    //0x00000020
    REG16                          Wakeup_Mask;                  //0x00000024
    REG16 Reserved_00000026;                    //0x00000026
    REG16                          Wakeup_Status;                //0x00000028
    /// Logical AND of Wakeup_Status and Wakeup_Mask (show only the enabled
    /// events).
    REG16                          Wakeup_Cause;                 //0x0000002A
    REG16                          Sema;                         //0x0000002C
    REG16 Reserved_0000002E[9];                 //0x0000002E
    union {
    REG32                          REG01;                        //0x00000040
        struct {
            REG16 REG0;
            REG16 REG1;
        };
    };
    union {
    REG32                          REG23;                        //0x00000044
        struct {
            REG16 REG2;
            REG16 REG3;
        };
    };
    union {
    REG32                          REG45;                        //0x00000048
        struct {
            REG16 REG4;
            REG16 REG5;
        };
    };
    union {
    REG32                          REG67;                        //0x0000004C
        struct {
            REG16 REG6;
            REG16 REG7;
        };
    };
    union {
    REG32                          ACC0;                         //0x00000050
        struct {
            REG16 ACC0_lo;
            REG16 ACC0_hi;
        };
    };
    union {
    REG32                          ACC1;                         //0x00000054
        struct {
            REG16 ACC1_lo;
            REG16 ACC1_hi;
        };
    };
    union {
    REG32                          RL6;                          //0x00000058
        struct {
            REG16 RL6_lo;
            REG16 RL6_hi;
        };
    };
    union {
    REG32                          RL7;                          //0x0000005C
        struct {
            REG16 RL7_lo;
            REG16 RL7_hi;
        };
    };
    REG16                          PC;                           //0x00000060
    REG16                          RA;                           //0x00000062
    REG16                          SP16;                         //0x00000064
    REG16                          SP32;                         //0x00000066
    REG32 Reserved_00000068[2];                 //0x00000068
    REG16                          BKP;                          //0x00000070
    REG16 Reserved_00000072;                    //0x00000072
    REG16                          PC_PREV;                      //0x00000074
    REG16                          LOOP;                         //0x00000076
} HWP_VOC_CFG_T;

#define hwp_vocCfg                 ((HWP_VOC_CFG_T*) KSEG1(REG_VOC_CFG_BASE))


//Ctrl
#define VOC_CFG_RUN_PAUSE          (0<<0)
#define VOC_CFG_RUN_SOFTWAKEUP0    (1<<0)
#define VOC_CFG_RUN_SOFTWAKEUP1    (3<<0)
#define VOC_CFG_RUN_STOP           (4<<0)
#define VOC_CFG_RUN_START          (5<<0)
#define VOC_CFG_BCPU_IRQ           (1<<4)
#define VOC_CFG_XCPU_IRQ           (1<<5)
#define VOC_CFG_RUNNING            (1<<0)
#define VOC_CFG_RUNNING_YES        (1<<0)
#define VOC_CFG_RUNNING_NO         (0<<0)
#define VOC_CFG_SEMA_STATUS        (1<<1)
#define VOC_CFG_STATUS_SPY_WR_ERR  (1<<2)

//DMA_Wrap
#define VOC_CFG_DMA_WRAP(n)        (((n)&15)<<0)

//DMA_Size
#define VOC_CFG_DMA_SIZE(n)        (((n)&0x3FFF)<<0)

//DMA_Laddr
#define VOC_CFG_DMA_LADDR(n)       (((n)&0xFFFF)<<0)

//DMA_EAddr
#define VOC_CFG_DMA_EADDR(n)       (((n)&0x3FFFFFF)<<2)
#define VOC_CFG_DMA_EADDR_MASK     (0x3FFFFFF<<2)
#define VOC_CFG_DMA_B2S_EN         (1<<28)
#define VOC_CFG_DMA_B2S_SIGN       (1<<29)
#define VOC_CFG_DMA_WRITE_READ     (0<<30)
#define VOC_CFG_DMA_WRITE_WRITE    (1<<30)
#define VOC_CFG_DMA_SINGLE         (1<<31)

//DMA_Data_Single
#define VOC_CFG_DMA_DATA_SINGLE(n) (((n)&0xFFFFFFFF)<<0)

//DMA_2D
#define VOC_CFG_DMA_STRIDE(n)      (((n)&0x1FFF)<<0)
#define VOC_CFG_DMA_LINES(n)       (((n)&0xFFF)<<16)

//DAI_Data_In
#define VOC_CFG_DAI_DATA_IN(n)     (((n)&0x1FFF)<<0)
#define VOC_CFG_DAI_DATA_READY     (1<<15)

//DAI_Data_Out
#define VOC_CFG_DAI_DATA_OUT(n)    (((n)&0x1FFF)<<0)

//ROM_Page
#define VOC_CFG_ROM_PAGE(n)        (((n)&0xFFFF)<<0)

//Debug
#define VOC_CFG_STALL_ON_BKP       (1<<0)
#define VOC_CFG_STALL_ON_BRANCH_TAKEN (1<<1)
#define VOC_CFG_STEP               (1<<2)

//BIST_Ctrl
#define VOC_CFG_BIST_RUN           (1<<0)
#define VOC_CFG_BIST_RUNNING       (1<<0)
#define VOC_CFG_BIST_STATUS        (1<<1)
#define VOC_CFG_BIST_CRC(n)        (((n)&0xFFFF)<<16)

//Wakeup_Mask
#define VOC_CFG_IFC0_EVENT_MASK    (1<<0)
#define VOC_CFG_IFC1_EVENT_MASK    (1<<1)
#define VOC_CFG_DMAE_EVENT_MASK    (1<<2)
#define VOC_CFG_DMAI_EVENT_MASK    (1<<3)
#define VOC_CFG_SOF0_EVENT_MASK    (1<<4)
#define VOC_CFG_SOF1_EVENT_MASK    (1<<5)
#define VOC_CFG_WAKEUP_MASK(n)     (((n)&0x3F)<<0)
#define VOC_CFG_WAKEUP_MASK_MASK   (0x3F<<0)
#define VOC_CFG_WAKEUP_MASK_SHIFT  (0)

//Wakeup_Status
#define VOC_CFG_IFC0_EVENT_STATUS  (1<<0)
#define VOC_CFG_IFC1_EVENT_STATUS  (1<<1)
#define VOC_CFG_DMAE_EVENT_STATUS  (1<<2)
#define VOC_CFG_DMAI_EVENT_STATUS  (1<<3)
#define VOC_CFG_SOF0_EVENT_STATUS  (1<<4)
#define VOC_CFG_SOF1_EVENT_STATUS  (1<<5)
#define VOC_CFG_WAKEUP_STATUS(n)   (((n)&0x3F)<<0)
#define VOC_CFG_WAKEUP_STATUS_MASK (0x3F<<0)
#define VOC_CFG_WAKEUP_STATUS_SHIFT (0)

//Wakeup_Cause
#define VOC_CFG_IFC0_EVENT_CAUSE   (1<<0)
#define VOC_CFG_IFC1_EVENT_CAUSE   (1<<1)
#define VOC_CFG_DMAE_EVENT_CAUSE   (1<<2)
#define VOC_CFG_DMAI_EVENT_CAUSE   (1<<3)
#define VOC_CFG_SOF0_EVENT_CAUSE   (1<<4)
#define VOC_CFG_SOF1_EVENT_CAUSE   (1<<5)

//Sema
#define VOC_CFG_SEMA               (1<<0)

//REG01
#define VOC_CFG_REG0(n)            (((n)&0xFFFF)<<0)
#define VOC_CFG_REG1(n)            (((n)&0xFFFF)<<16)

//REG23
#define VOC_CFG_REG2(n)            (((n)&0xFFFF)<<0)
#define VOC_CFG_REG3(n)            (((n)&0xFFFF)<<16)

//REG45
#define VOC_CFG_REG4(n)            (((n)&0xFFFF)<<0)
#define VOC_CFG_REG5(n)            (((n)&0xFFFF)<<16)

//REG67
#define VOC_CFG_REG6(n)            (((n)&0xFFFF)<<0)
#define VOC_CFG_REG7(n)            (((n)&0xFFFF)<<16)

//ACC0
#define VOC_CFG_ACC0_LO(n)         (((n)&0xFFFF)<<0)
#define VOC_CFG_ACC0_HI(n)         (((n)&0xFFFF)<<16)

//ACC1
#define VOC_CFG_ACC1_LO(n)         (((n)&0xFFFF)<<0)
#define VOC_CFG_ACC1_HI(n)         (((n)&0xFFFF)<<16)

//RL6
#define VOC_CFG_RL6_LO(n)          (((n)&0xFFFF)<<0)
#define VOC_CFG_RL6_HI(n)          (((n)&0xFFFF)<<16)

//RL7
#define VOC_CFG_RL7_LO(n)          (((n)&0xFFFF)<<0)
#define VOC_CFG_RL7_HI(n)          (((n)&0xFFFF)<<16)

//PC
#define VOC_CFG_PC(n)              (((n)&0xFFFF)<<0)

//RA
#define VOC_CFG_RA(n)              (((n)&0xFFFF)<<0)

//SP16
#define VOC_CFG_SP16(n)            (((n)&0xFFFF)<<0)

//SP32
#define VOC_CFG_SP32(n)            (((n)&0xFFFF)<<0)

//BKP
#define VOC_CFG_BKP(n)             (((n)&0xFFFF)<<0)

//PC_PREV
#define VOC_CFG_PC_PREV(n)         (((n)&0xFFFF)<<0)

//LOOP
#define VOC_CFG_LOOP0(n)           (((n)&0xFF)<<0)
#define VOC_CFG_LOOP1(n)           (((n)&0xFF)<<8)





#endif

