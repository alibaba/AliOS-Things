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


#ifndef _DEBUG_HOST_INTERNALS_H_
#define _DEBUG_HOST_INTERNALS_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'debug_host_internals'."
#endif


#include "globals.h"

// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// DEBUG_HOST_INTERNAL_REGISTERS_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_INT_REG_DBG_HOST_BASE  0x00000000

typedef volatile struct
{
    /// General control signals set.
    REG8                           CTRL_SET;                     //0x00000000
    /// General control signals clear.
    REG8                           CTRL_CLR;                     //0x00000001
    /// Configure Debug UART Clock divider.
    REG8                           CLKDIV;                       //0x00000002
    /// Configure Debug UART.
    REG8                           CFG;                          //0x00000003
    /// Status of CRC.
    REG8                           CRC_REG;                      //0x00000004
    /// Host write, APB readable register.
    REG8                           H2P_STATUS;                   //0x00000005
    /// APB write, Host readable register.
    REG8                           P2H_STATUS;                   //0x00000006
    /// Debug information of system side AHB bus status.
    REG8                           AHB_SYS_HMBURSREQ;            //0x00000007
    REG8                           AHB_SYS_HMGRANT;              //0x00000008
    /// Debug information of AHB bus status HSEL.
    REG8                           AHB_SYS_HSEL;                 //0x00000009
    REG8                           AHB_SYS_HSREADY;              //0x0000000A
    /// Debug information of baseband side AHB bus status.
    REG8                           AHB_BB_MASTER;                //0x0000000B
    /// Debug information of AHB bus status HSEL.
    REG8                           AHB_BB_HSREADY;               //0x0000000C
    /// Debug information of AHB bus status HSEL.
    REG8                           AHB_BB_HSEL;                  //0x0000000D
    /// Represents the split status register of the SYS_AHBC.
    REG8                           AHB_SYS_MASK_SPLIT;           //0x0000000E
    /// Represents the split status register of the BB_AHBC.
    REG8                           AHB_BB_MASK_SPLIT;            //0x0000000F
} HWP_DEBUG_HOST_INTERNAL_REGISTERS_T;

#define hwp_intRegDbgHost          ((HWP_DEBUG_HOST_INTERNAL_REGISTERS_T*) KSEG1(REG_INT_REG_DBG_HOST_BASE))


//CTRL_SET
#define DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_RESET (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_XCPU_FORCE_RESET (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_WAKEUP (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_BP_XCPU (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_BP_BCPU (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_IT_XCPU (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_IT_BCPU (1<<6)
#define DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_PORT_LOCK (1<<7)

//CTRL_CLR
//#define DEBUG_HOST_INTERNAL_REGISTERS_XCPU_FORCE_RESET (1<<1)
//#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_WAKEUP (1<<2)
//#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_BP_XCPU (1<<3)
//#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_BP_BCPU (1<<4)
//#define DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_PORT_LOCK (1<<7)

//CLKDIV
#define DEBUG_HOST_INTERNAL_REGISTERS_CFG_CLK(n) (((n)&0x3F)<<0)

//CFG
#define DEBUG_HOST_INTERNAL_REGISTERS_DISABLE_UART_H (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_DISABLE_IFC_H (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_DEBUG_HOST_SEL (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_FORCE_PRIO_H (1<<7)

//CRC_REG
#define DEBUG_HOST_INTERNAL_REGISTERS_CRC (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_FC_FIFO_OVF (1<<1)

//H2P_STATUS
#define DEBUG_HOST_INTERNAL_REGISTERS_STATUS(n) (((n)&0xFF)<<0)

//P2H_STATUS
//#define DEBUG_HOST_INTERNAL_REGISTERS_STATUS(n) (((n)&0xFF)<<0)

//AHB_SYS_HMBURSREQ
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_IFC_HMBURSREQ (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_DMA_HMBURSREQ (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_AHB2AHB_HMBURSREQ (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_XCPU_HMBURSREQ (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_USBC_HMBURSREQ (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_GOUDA_HMBURSREQ (1<<5)

//AHB_SYS_HMGRANT
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_IFC_HMGRANT (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_DMA_HMGRANT (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_AHB2AHB_HMGRANT (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_XCPU_HMGRANT (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_USBC_HMGRANT (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_GOUDA_HMGRANT (1<<5)

//AHB_SYS_HSEL
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MEM_EXT_HSEL (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MEM_INT_HSEL (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_IFC_HSEL (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_AHB2AHB_HSEL (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_USBC_HSEL (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_GOUDA_HSEL (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_XCPU_RAM_HSEL (1<<6)

//AHB_SYS_HSREADY
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_IFC_HSREADY (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MEM_HSREADY (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_AHB2AHB_HSREADY (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_USBC_HSREADY (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_GOUDA_HSREADY (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_XCPU_RAM_HSREADY (1<<6)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_HREADY (1<<7)

//AHB_BB_MASTER
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_IFC_HMBURSREQ (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_VOC_HMBURSREQ (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_AHB2AHB_HMBURSREQ (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_BCPU_HMBRSREQ (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_IFC_HMGRANT (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_VOC_HMGRANT (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_AHB2AHB_HMGRANT (1<<6)
#define DEBUG_HOST_INTERNAL_REGISTERS_BCPU_HMGRANT (1<<7)

//AHB_BB_HSREADY
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MEM_HSREADY (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_VOC_HSREADY (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_SRAM_HSREADY (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_IFC_HSREADY (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_AHB2AHB_HSREADY (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_HREADY (1<<7)

//AHB_BB_HSEL
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MEM_EXT_HSEL (1<<0)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MEM_INT_HSEL (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_VOC_HSEL (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_SRAM_HSEL (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_IFC_HSEL (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_AHB2AHB_HSEL (1<<5)

//AHB_SYS_MASK_SPLIT
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_BIST (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_DMA (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_XCPU (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_AHB2AHB (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_IFC (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_USB (1<<6)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_GOUDA (1<<7)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_SYS_MID_VOC (1<<8)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MASK_SPLIT(n) (((n)&0xFF)<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MASK_SPLIT_MASK (0xFF<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_SYS_MASK_SPLIT_SHIFT (1)

//AHB_BB_MASK_SPLIT
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_BB_MID_BIST (1<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_BB_MID_IFC (1<<2)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_BB_MID_VOC (1<<3)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_BB_MID_BCPU (1<<4)
#define DEBUG_HOST_INTERNAL_REGISTERS_MASK_SPLIT_BB_MID_AHB2AHB (1<<5)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MASK_SPLIT(n) (((n)&31)<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MASK_SPLIT_MASK (31<<1)
#define DEBUG_HOST_INTERNAL_REGISTERS_BB_MASK_SPLIT_SHIFT (1)





#endif

