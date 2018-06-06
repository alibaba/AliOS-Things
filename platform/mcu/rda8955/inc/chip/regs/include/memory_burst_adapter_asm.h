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

#ifndef _MEMORY_BURST_ADAPTER_ASM_H_
#define _MEMORY_BURST_ADAPTER_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'memory_burst_adapter'."
#endif



//==============================================================================
// memory_burst_adapter
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CS0_CONF_BASE          0x02000000
#define REG_CS1_CONF_BASE          0x00400000
#define REG_CS2_CONF_BASE          0x01800000
#define REG_CS3_CONF_BASE          0x01880000
#define REG_CS4_CONF_BASE          0x01000000

#define REG_CS0_CONF_BASE_HI       BASE_HI(REG_CS0_CONF_BASE)
#define REG_CS0_CONF_BASE_LO       BASE_LO(REG_CS0_CONF_BASE)

#define REG_CS0_CONF_BURST_CONFIG  REG_CS0_CONF_BASE_LO + 0x00000000
#define REG_CS0_CONF_SCRATCH        REG_CS0_CONF_BASE_LO + 0x00000002

#define REG_CS1_CONF_BASE_HI       BASE_HI(REG_CS1_CONF_BASE)
#define REG_CS1_CONF_BASE_LO       BASE_LO(REG_CS1_CONF_BASE)

#define REG_CS1_CONF_BURST_CONFIG  REG_CS1_CONF_BASE_LO + 0x00000000
#define REG_CS1_CONF_SCRATCH        REG_CS1_CONF_BASE_LO + 0x00000002

#define REG_CS2_CONF_BASE_HI       BASE_HI(REG_CS2_CONF_BASE)
#define REG_CS2_CONF_BASE_LO       BASE_LO(REG_CS2_CONF_BASE)

#define REG_CS2_CONF_BURST_CONFIG  REG_CS2_CONF_BASE_LO + 0x00000000
#define REG_CS2_CONF_SCRATCH        REG_CS2_CONF_BASE_LO + 0x00000002

#define REG_CS3_CONF_BASE_HI       BASE_HI(REG_CS3_CONF_BASE)
#define REG_CS3_CONF_BASE_LO       BASE_LO(REG_CS3_CONF_BASE)

#define REG_CS3_CONF_BURST_CONFIG  REG_CS3_CONF_BASE_LO + 0x00000000
#define REG_CS3_CONF_SCRATCH        REG_CS3_CONF_BASE_LO + 0x00000002

#define REG_CS4_CONF_BASE_HI       BASE_HI(REG_CS4_CONF_BASE)
#define REG_CS4_CONF_BASE_LO       BASE_LO(REG_CS4_CONF_BASE)

#define REG_CS4_CONF_BURST_CONFIG  REG_CS4_CONF_BASE_LO + 0x00000000
#define REG_CS4_CONF_SCRATCH        REG_CS4_CONF_BASE_LO + 0x00000002

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
