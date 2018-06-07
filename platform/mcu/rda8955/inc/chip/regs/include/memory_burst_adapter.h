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


#ifndef _MEMORY_BURST_ADAPTER_H_
#define _MEMORY_BURST_ADAPTER_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'memory_burst_adapter'."
#endif



// =============================================================================
//  MACROS
// =============================================================================

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// MEMORY_BURST_ADAPTER_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CS0_CONF_BASE          0x02000000
#define REG_CS1_CONF_BASE          0x00400000
#define REG_CS2_CONF_BASE          0x01800000
#define REG_CS3_CONF_BASE          0x01880000
#define REG_CS4_CONF_BASE          0x01000000

typedef volatile struct
{
    /// This register is accessible only when CRE is set to 1.
    /// When read and write are not both in same mode, a cycle with CS high must
    /// be insterted between read and write access.
    REG16                          Burst_Config;                 //0x00000000
    REG16                          Scratch;                      //0x00000002
} HWP_MEMORY_BURST_ADAPTER_T;

#define hwp_cs0Conf                ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS0_CONF_BASE))
#define hwp_cs1Conf                ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS1_CONF_BASE))
#define hwp_cs2Conf                ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS2_CONF_BASE))
#define hwp_cs3Conf                ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS3_CONF_BASE))
#define hwp_cs4Conf                ((HWP_MEMORY_BURST_ADAPTER_T*) KSEG1(REG_CS4_CONF_BASE))


//Burst_Config
#define MEMORY_BURST_ADAPTER_READ_MODE_ASYNCHRONOUS (0<<0)
#define MEMORY_BURST_ADAPTER_READ_MODE_SYNCHRONOUS (1<<0)
#define MEMORY_BURST_ADAPTER_WRITE_MODE_ASYNCHRONOUS (0<<1)
#define MEMORY_BURST_ADAPTER_WRITE_MODE_SYNCHRONOUS (1<<1)
#define MEMORY_BURST_ADAPTER_BURST_LAT(n) (((n)&15)<<4)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE(n) (((n)&7)<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_MASK (7<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_SHIFT (8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_4HW (0<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_8HW (1<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_16HW (2<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_32HW (3<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_64HW (4<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_128HW (5<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_256HW (6<<8)
#define MEMORY_BURST_ADAPTER_PAGE_SIZE_FULL (7<<8)

//Scratch_REG
#define MEMORY_BURST_ADAPTER_SCRATCH(n) (((n)&0xFFFF)<<0)





#endif

