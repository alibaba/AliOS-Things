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


#ifndef _MEM_BRIDGE_ASM_H_
#define _MEM_BRIDGE_ASM_H_

//THIS FILE HAS BEEN GENERATED WITH COOLWATCHER. PLEASE EDIT WITH CARE !

#ifndef CT_ASM
#error "You are trying to use in a normal C code the assembly H description of 'mem_bridge'."
#endif

#include "globals_asm.h"
#include "gallite_generic_config_asm.h"

#define EBC_NB_BITS_CS                          (3)
#define EBC_NB_CS                               (5)
#define FIRST_CS_WITH_POLARITY                  (2)
#define NB_BITS_EXTADDR                         (EBC_NB_BITS_ADDR+1)
#define NB_BITS_MAXSPACE                        (25)
#define NB_BITS_CS4                             (23)
#define EBC_CS0_SIZE                            (EXP2(NB_BITS_MAXSPACE))
#define EBC_CS1_SIZE                            (EXP2(NB_BITS_MAXSPACE-1))
#define EBC_CS2_SIZE                            (EXP2(19))
#define EBC_CS3_SIZE                            (EXP2(18))
#define EBC_CS4_SIZE                            (EXP2(NB_BITS_CS4))
#define INT_RAM_NB_BITS                         (17)
#define INT_RAM_NB_BLOCK                        (MEMBRIDGE_RAM_NB_BLOCK)
#define INT_RAM_SIZE                            (32768*INT_RAM_NB_BLOCK)
#define INT_ROM_NB_BITS                         (17)
#define INT_ROM_SIZE                            (80*1024)
#define NB_BIT_ROM_PACHT                        (4)
#define NB_ROM_PACHT                            (16)
#define BCPU_ROM_SIZE                           (32*1024*3)

//==============================================================================
// mem_bridge
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BB_MEM_BRIDGE_BASE     0x01912000

#define REG_BB_MEM_BRIDGE_BASE_HI  BASE_HI(REG_BB_MEM_BRIDGE_BASE)
#define REG_BB_MEM_BRIDGE_BASE_LO  BASE_LO(REG_BB_MEM_BRIDGE_BASE)

#define REG_BB_MEM_BRIDGE_FIFO_CTRL REG_BB_MEM_BRIDGE_BASE_LO + 0x00000000
#define REG_BB_MEM_BRIDGE_FIFO_STATUS REG_BB_MEM_BRIDGE_BASE_LO + 0x00000004
#define REG_BB_MEM_BRIDGE_MONITOR_CTRL REG_BB_MEM_BRIDGE_BASE_LO + 0x00000008
#define REG_BB_MEM_BRIDGE_ROM_BIST REG_BB_MEM_BRIDGE_BASE_LO + 0x0000000C
#define REG_BB_MEM_BRIDGE_SRAM_BIST REG_BB_MEM_BRIDGE_BASE_LO + 0x00000010
#define REG_BB_MEM_BRIDGE_ROM_PATCH_0 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000100
#define REG_BB_MEM_BRIDGE_ROM_PATCH_1 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000104
#define REG_BB_MEM_BRIDGE_ROM_PATCH_2 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000108
#define REG_BB_MEM_BRIDGE_ROM_PATCH_3 REG_BB_MEM_BRIDGE_BASE_LO + 0x0000010C
#define REG_BB_MEM_BRIDGE_ROM_PATCH_4 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000110
#define REG_BB_MEM_BRIDGE_ROM_PATCH_5 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000114
#define REG_BB_MEM_BRIDGE_ROM_PATCH_6 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000118
#define REG_BB_MEM_BRIDGE_ROM_PATCH_7 REG_BB_MEM_BRIDGE_BASE_LO + 0x0000011C
#define REG_BB_MEM_BRIDGE_ROM_PATCH_8 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000120
#define REG_BB_MEM_BRIDGE_ROM_PATCH_9 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000124
#define REG_BB_MEM_BRIDGE_ROM_PATCH_10 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000128
#define REG_BB_MEM_BRIDGE_ROM_PATCH_11 REG_BB_MEM_BRIDGE_BASE_LO + 0x0000012C
#define REG_BB_MEM_BRIDGE_ROM_PATCH_12 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000130
#define REG_BB_MEM_BRIDGE_ROM_PATCH_13 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000134
#define REG_BB_MEM_BRIDGE_ROM_PATCH_14 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000138
#define REG_BB_MEM_BRIDGE_ROM_PATCH_15 REG_BB_MEM_BRIDGE_BASE_LO + 0x0000013C
#define REG_BB_MEM_BRIDGE_EBC_CTRL REG_BB_MEM_BRIDGE_BASE_LO + 0x00000400
#define REG_BB_MEM_BRIDGE_EBC_STATUS REG_BB_MEM_BRIDGE_BASE_LO + 0x00000404
#define REG_BB_MEM_BRIDGE_CS_TIME_WRITE REG_BB_MEM_BRIDGE_BASE_LO + 0x0000040C
#define REG_BB_MEM_BRIDGE_CS_CONFIG_0_CS_MODE REG_BB_MEM_BRIDGE_BASE_LO + 0x00000410
#define REG_BB_MEM_BRIDGE_CS_CONFIG_0_CS_TIME REG_BB_MEM_BRIDGE_BASE_LO + 0x00000414
#define REG_BB_MEM_BRIDGE_CS_CONFIG_1_CS_MODE REG_BB_MEM_BRIDGE_BASE_LO + 0x00000418
#define REG_BB_MEM_BRIDGE_CS_CONFIG_1_CS_TIME REG_BB_MEM_BRIDGE_BASE_LO + 0x0000041C
#define REG_BB_MEM_BRIDGE_CS_CONFIG_2_CS_MODE REG_BB_MEM_BRIDGE_BASE_LO + 0x00000420
#define REG_BB_MEM_BRIDGE_CS_CONFIG_2_CS_TIME REG_BB_MEM_BRIDGE_BASE_LO + 0x00000424
#define REG_BB_MEM_BRIDGE_CS_CONFIG_3_CS_MODE REG_BB_MEM_BRIDGE_BASE_LO + 0x00000428
#define REG_BB_MEM_BRIDGE_CS_CONFIG_3_CS_TIME REG_BB_MEM_BRIDGE_BASE_LO + 0x0000042C
#define REG_BB_MEM_BRIDGE_CS_CONFIG_4_CS_MODE REG_BB_MEM_BRIDGE_BASE_LO + 0x00000430
#define REG_BB_MEM_BRIDGE_CS_CONFIG_4_CS_TIME REG_BB_MEM_BRIDGE_BASE_LO + 0x00000434
#define REG_BB_MEM_BRIDGE_AHBM_CTRL REG_BB_MEM_BRIDGE_BASE_LO + 0x00000500
#define REG_BB_MEM_BRIDGE_AHBM_STATUS REG_BB_MEM_BRIDGE_BASE_LO + 0x00000504
#define REG_BB_MEM_BRIDGE_SPACEBASE_0 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000510
#define REG_BB_MEM_BRIDGE_SPACEBASE_1 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000514
#define REG_BB_MEM_BRIDGE_SPACEBASE_2 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000518
#define REG_BB_MEM_BRIDGE_SPACEBASE_3 REG_BB_MEM_BRIDGE_BASE_LO + 0x0000051C
#define REG_BB_MEM_BRIDGE_SPACEBASE_4 REG_BB_MEM_BRIDGE_BASE_LO + 0x00000520

//FIFO_Ctrl
#define MEM_BRIDGE_LOCK_ENABLE     (1<<0)
#define MEM_BRIDGE_LOCK_DISABLE    (0<<0)
#define MEM_BRIDGE_AHB_CMD_FULL_SHIFT_ENABLE (0<<4)
#define MEM_BRIDGE_AHB_CMD_FULL_SHIFT_DISABLE (1<<4)

//FIFO_Status
#define MEM_BRIDGE_FIFO_A_LEVEL(n) (((n)&3)<<4)
#define MEM_BRIDGE_FIFO_A_LEVEL_MASK (3<<4)
#define MEM_BRIDGE_FIFO_A_LEVEL_SHIFT (4)
#define MEM_BRIDGE_FIFO_B_LEVEL(n) (((n)&3)<<8)
#define MEM_BRIDGE_FIFO_B_LEVEL_MASK (3<<8)
#define MEM_BRIDGE_FIFO_B_LEVEL_SHIFT (8)

//Monitor_Ctrl
#define MEM_BRIDGE_MONITOR_SEL_SIDE_BY_SIDE (0<<0)
#define MEM_BRIDGE_MONITOR_SEL_DATA_VS_EXTERNAL (1<<0)

//Rom_Bist
#define MEM_BRIDGE_CTRL_DONE       (0<<0)
#define MEM_BRIDGE_CTRL_RUNNING    (1<<0)
#define MEM_BRIDGE_CRC(n)          (((n)&0xFFFF)<<16)

//SRam_Bist
//#define MEM_BRIDGE_CTRL_DONE     (0<<0)
//#define MEM_BRIDGE_CTRL_RUNNING  (1<<0)
#define MEM_BRIDGE_P_STATUS        (1<<3)
#define MEM_BRIDGE_BLOCK_SEL(n)    (((n)&3)<<15)
#define MEM_BRIDGE_STATUS_BAD      (1<<31)
#define MEM_BRIDGE_STATUS_OK       (0<<31)

//Rom_Patch
#define MEM_BRIDGE_BLOCK_ADDR(n)   (((n)&0x1FFF)<<4)
#define MEM_BRIDGE_BLOCK_ADDR_MASK (0x1FFF<<4)
#define MEM_BRIDGE_BLOCK_ADDR_SHIFT (4)
#define MEM_BRIDGE_PATCH_ENABLE    (1<<31)
#define MEM_BRIDGE_PATCH_DISABLE   (0<<31)

//EBC_Ctrl
#define MEM_BRIDGE_ENABLE          (1<<0)
#define MEM_BRIDGE_IDLEDRIVE       (1<<12)

//EBC_Status
//#define MEM_BRIDGE_ENABLE        (1<<0)
#define MEM_BRIDGE_UPDATE_CS       (1<<4)

//CS_Time_Write
#define MEM_BRIDGE_RDLATHALFMODE   (1<<0)
#define MEM_BRIDGE_RDLAT(n)        (((n)&31)<<1)
#define MEM_BRIDGE_RDLAT_MASK      (31<<1)
#define MEM_BRIDGE_RDLAT_SHIFT     (1)
#define MEM_BRIDGE_OEDLYHALFMODE   (1<<8)
#define MEM_BRIDGE_OEDLY(n)        (((n)&7)<<9)
#define MEM_BRIDGE_OEDLY_MASK      (7<<9)
#define MEM_BRIDGE_OEDLY_SHIFT     (9)
#define MEM_BRIDGE_PAGELAT(n)      (((n)&7)<<13)
#define MEM_BRIDGE_PAGELAT_MASK    (7<<13)
#define MEM_BRIDGE_PAGELAT_SHIFT   (13)
#define MEM_BRIDGE_WRLATHALFMODE   (1<<16)
#define MEM_BRIDGE_WRLAT(n)        (((n)&31)<<17)
#define MEM_BRIDGE_WRLAT_MASK      (31<<17)
#define MEM_BRIDGE_WRLAT_SHIFT     (17)
#define MEM_BRIDGE_WEDLYHALFMODE   (1<<24)
#define MEM_BRIDGE_WEDLY(n)        (((n)&7)<<25)
#define MEM_BRIDGE_WEDLY_MASK      (7<<25)
#define MEM_BRIDGE_WEDLY_SHIFT     (25)
#define MEM_BRIDGE_RELAX(n)        (((n)&7)<<28)
#define MEM_BRIDGE_RELAX_MASK      (7<<28)
#define MEM_BRIDGE_RELAX_SHIFT     (28)
#define MEM_BRIDGE_FORCERELAX      (1<<31)

//CS_Mode
#define MEM_BRIDGE_CSEN            (1<<0)
#define MEM_BRIDGE_POLARITY        (1<<1)
#define MEM_BRIDGE_POLARITY_MASK   (1<<1)
#define MEM_BRIDGE_POLARITY_SHIFT  (1)
#define MEM_BRIDGE_POLARITY_ACTIVEHIGH (1<<1)
#define MEM_BRIDGE_POLARITY_ACTIVELOW (0<<1)
#define MEM_BRIDGE_ADMUXMODE       (1<<2)
#define MEM_BRIDGE_WRITEALLOW      (1<<4)
#define MEM_BRIDGE_WBEMODE         (1<<5)
#define MEM_BRIDGE_BEDLYMODE       (1<<6)
#define MEM_BRIDGE_WAITMODE        (1<<8)
#define MEM_BRIDGE_WRITEWAITMODE   (1<<9)
#define MEM_BRIDGE_PAGESIZE(n)     (((n)&7)<<12)
#define MEM_BRIDGE_PAGESIZE_MASK   (7<<12)
#define MEM_BRIDGE_PAGESIZE_SHIFT  (12)
#define MEM_BRIDGE_PAGESIZE_4HW    (0<<12)
#define MEM_BRIDGE_PAGESIZE_8HW    (1<<12)
#define MEM_BRIDGE_PAGESIZE_16HW   (2<<12)
#define MEM_BRIDGE_PAGESIZE_32HW   (3<<12)
#define MEM_BRIDGE_PAGESIZE_64HW   (4<<12)
#define MEM_BRIDGE_PAGESIZE_128HW  (5<<12)
#define MEM_BRIDGE_PAGESIZE_256HW  (6<<12)
#define MEM_BRIDGE_PAGESIZE_FULL   (7<<12)
#define MEM_BRIDGE_PAGEMODE        (1<<16)
#define MEM_BRIDGE_WRITEPAGEMODE   (1<<17)
#define MEM_BRIDGE_BURSTMODE       (1<<20)
#define MEM_BRIDGE_WRITEBURSTMODE  (1<<21)
#define MEM_BRIDGE_ADVASYNC        (1<<24)
#define MEM_BRIDGE_ADVWEPULSE      (1<<25)
#define MEM_BRIDGE_ADMUXDLY(n)     (((n)&3)<<26)
#define MEM_BRIDGE_ADMUXDLY_MASK   (3<<26)
#define MEM_BRIDGE_ADMUXDLY_SHIFT  (26)
#define MEM_BRIDGE_ADMUXDLY_NORMAL (0<<26)
#define MEM_BRIDGE_ADMUXDLY_F1     (1<<26)
#define MEM_BRIDGE_ADMUXDLY_F1_5   (2<<26)
#define MEM_BRIDGE_ADMUXDLY_F2     (3<<26)
#define MEM_BRIDGE_WRITESINGLE     (1<<28)
#define MEM_BRIDGE_WRITEHOLD       (1<<29)

//CS_Time
//#define MEM_BRIDGE_RDLATHALFMODE (1<<0)
//#define MEM_BRIDGE_RDLAT(n)      (((n)&31)<<1)
//#define MEM_BRIDGE_RDLAT_MASK    (31<<1)
//#define MEM_BRIDGE_RDLAT_SHIFT   (1)
//#define MEM_BRIDGE_OEDLYHALFMODE (1<<8)
//#define MEM_BRIDGE_OEDLY(n)      (((n)&7)<<9)
//#define MEM_BRIDGE_OEDLY_MASK    (7<<9)
//#define MEM_BRIDGE_OEDLY_SHIFT   (9)
//#define MEM_BRIDGE_PAGELAT(n)    (((n)&7)<<13)
//#define MEM_BRIDGE_PAGELAT_MASK  (7<<13)
//#define MEM_BRIDGE_PAGELAT_SHIFT (13)
//#define MEM_BRIDGE_WRLATHALFMODE (1<<16)
//#define MEM_BRIDGE_WRLAT(n)      (((n)&31)<<17)
//#define MEM_BRIDGE_WRLAT_MASK    (31<<17)
//#define MEM_BRIDGE_WRLAT_SHIFT   (17)
//#define MEM_BRIDGE_WEDLYHALFMODE (1<<24)
//#define MEM_BRIDGE_WEDLY(n)      (((n)&7)<<25)
//#define MEM_BRIDGE_WEDLY_MASK    (7<<25)
//#define MEM_BRIDGE_WEDLY_SHIFT   (25)
//#define MEM_BRIDGE_RELAX(n)      (((n)&7)<<28)
//#define MEM_BRIDGE_RELAX_MASK    (7<<28)
//#define MEM_BRIDGE_RELAX_SHIFT   (28)
//#define MEM_BRIDGE_FORCERELAX    (1<<31)

//AHBM_Ctrl
//#define MEM_BRIDGE_ENABLE        (1<<0)

//AHBM_Status
//#define MEM_BRIDGE_ENABLE        (1<<0)
#define MEM_BRIDGE_UPDATE_SB       (1<<4)

//SpaceBase
#define MEM_BRIDGE_BASEADDR(n)     (((n)&0xFFFFFFF)<<4)



//==============================================================================
// mem_FIFO
//------------------------------------------------------------------------------
///
//==============================================================================

//Flush
#define MEM_FIFO_FLUSH             (1<<0)

//EBC_CRE
#define MEM_FIFO_CRE               (1<<0)

//EBC_CS4_Block_Remap
#define MEM_FIFO_BLOCK_REMAP(n)    (((n)&15)<<23)
#define MEM_FIFO_BLOCK_REMAP_MASK  (15<<23)
#define MEM_FIFO_BLOCK_REMAP_SHIFT (23)

//Side_Id
#define MEM_FIFO_SIDE_ID_SYSTEM    (0<<0)
#define MEM_FIFO_SIDE_ID_BASEBAND  (1<<0)



//==============================================================================
// ebc_cs0
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CS0_BASE               0x08000000

#define REG_CS0_BASE_HI            BASE_HI(REG_CS0_BASE)
#define REG_CS0_BASE_LO            BASE_LO(REG_CS0_BASE)




//==============================================================================
// ebc_cs1
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_CS1_BASE               0x02000000

#define REG_CS1_BASE_HI            BASE_HI(REG_CS1_BASE)
#define REG_CS1_BASE_LO            BASE_LO(REG_CS1_BASE)




//==============================================================================
// ebc_cs2
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// ebc_cs3
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// ebc_cs4
//------------------------------------------------------------------------------
///
//==============================================================================



//==============================================================================
// int_sram
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_INT_SRAM_BASE          0x01C00000

#define REG_INT_SRAM_BASE_HI       BASE_HI(REG_INT_SRAM_BASE)
#define REG_INT_SRAM_BASE_LO       BASE_LO(REG_INT_SRAM_BASE)




//==============================================================================
// bb_patch
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BB_PATCH_BASE          0x01D00000

#define REG_BB_PATCH_BASE_HI       BASE_HI(REG_BB_PATCH_BASE)
#define REG_BB_PATCH_BASE_LO       BASE_LO(REG_BB_PATCH_BASE)




//==============================================================================
// int_rom
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_INT_ROM_BASE           0x01E00000

#define REG_INT_ROM_BASE_HI        BASE_HI(REG_INT_ROM_BASE)
#define REG_INT_ROM_BASE_LO        BASE_LO(REG_INT_ROM_BASE)




//==============================================================================
// bcpu_rom
//------------------------------------------------------------------------------
///
//==============================================================================
#define REG_BCPU_ROM_BASE          0x01E80000

#define REG_BCPU_ROM_BASE_HI       BASE_HI(REG_BCPU_ROM_BASE)
#define REG_BCPU_ROM_BASE_LO       BASE_LO(REG_BCPU_ROM_BASE)





#endif
