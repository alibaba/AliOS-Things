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


#ifndef _MEM_BRIDGE_H_
#define _MEM_BRIDGE_H_

#ifdef CT_ASM
#error "You are trying to use in an assembly code the normal H description of 'mem_bridge'."
#endif


#include "globals.h"
#include "gallite_generic_config.h"

// =============================================================================
//  MACROS
// =============================================================================
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

// =============================================================================
//  TYPES
// =============================================================================

// ============================================================================
// MEM_BRIDGE_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_MEM_BRIDGE_BASE     0x01912000

typedef volatile struct
{
    REG32                          FIFO_Ctrl;                    //0x00000000
    REG32                          FIFO_Status;                  //0x00000004
    REG32                          Monitor_Ctrl;                 //0x00000008
    REG32                          Rom_Bist;                     //0x0000000C
    REG32                          SRam_Bist;                    //0x00000010
    REG32 Reserved_00000014[59];                //0x00000014
    REG32                          Rom_Patch[NB_ROM_PACHT];      //0x00000100
    REG32 Reserved_00000140[48];                //0x00000140
    REG32 Reserved_00000200[64];                //0x00000200
    REG32 Reserved_00000300[64];                //0x00000300
    REG32                          EBC_Ctrl;                     //0x00000400
    REG32                          EBC_Status;                   //0x00000404
    REG32 Reserved_00000408;                    //0x00000408
    /// This register is used to program the CS_Time register of One CS_Config
    /// at the same time as the CS_Mode regiter is set (so the two registers are
    /// always synchronized with each other).
    /// To update Chip Select n configuration, first write the timing value in
    /// this register, then write the CS_Mode configuration in the corresponding
    /// CS_Config[n].CS_Mode.
    REG32                          CS_Time_Write;                //0x0000040C
    struct
    {
        /// Writing this register will also copy the content of CS_Time_Write to
        /// the CS_Time register.
        REG32                      CS_Mode;                      //0x00000410
        /// This register cannot be written direclty. To update it, write to the
        /// CS_Time_write register, then to the CS_Mode register.
        REG32                      CS_Time;                      //0x00000414
    } CS_Config[EBC_NB_CS];
    REG32 Reserved_00000438[50];                //0x00000438
    REG32                          AHBM_Ctrl;                    //0x00000500
    REG32                          AHBM_Status;                  //0x00000504
    REG32 Reserved_00000508[2];                 //0x00000508
    REG32                          SpaceBase[EBC_NB_CS];         //0x00000510
} HWP_MEM_BRIDGE_T;

#define hwp_bbMemBridge            ((HWP_MEM_BRIDGE_T*) KSEG1(REG_BB_MEM_BRIDGE_BASE))


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



// ============================================================================
// MEM_FIFO_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef volatile struct
{
    /// Reading this register ensure the previous write command are all done
    /// (any read would do anyway) but this does not access external controller.
    /// When changing CS Config it will only work if the baseband is off
    /// (boot...) ! Poll CS_Update in reg CS Status instead.
    REG32                          Flush;                        //0x00000000
    REG32                          EBC_CRE;                      //0x00000004
    REG32                          EBC_CS4_Block_Remap;          //0x00000008
    REG32                          Side_Id;                      //0x0000000C
} HWP_MEM_FIFO_T;



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



// ============================================================================
// EBC_CS0_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CS0_BASE               0x08000000

typedef volatile struct
{
    UINT8 ram_array[33554432];                  //0x00000000
} HWP_EBC_CS0_T;

#define hwp_cs0                    ((HWP_EBC_CS0_T*) KSEG1(REG_CS0_BASE))




// ============================================================================
// EBC_CS1_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_CS1_BASE               0x02000000

typedef volatile struct
{
    UINT8 ram_array[16777216];                  //0x00000000
} HWP_EBC_CS1_T;

#define hwp_cs1                    ((HWP_EBC_CS1_T*) KSEG1(REG_CS1_BASE))




// ============================================================================
// EBC_CS2_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef volatile struct
{
    UINT8 ram_array[524288];                    //0x00000000
} HWP_EBC_CS2_T;





// ============================================================================
// EBC_CS3_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef volatile struct
{
    UINT8 ram_array[262144];                    //0x00000000
} HWP_EBC_CS3_T;





// ============================================================================
// EBC_CS4_T
// -----------------------------------------------------------------------------
///
// =============================================================================
typedef volatile struct
{
    UINT8 ram_array[8388608];                   //0x00000000
} HWP_EBC_CS4_T;





// ============================================================================
// INT_SRAM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_INT_SRAM_BASE          0x01C00000

typedef volatile struct
{
    UINT8 sram_array[65536];                    //0x00000000
} HWP_INT_SRAM_T;

#define hwp_intSram                ((HWP_INT_SRAM_T*) KSEG1(REG_INT_SRAM_BASE))




// ============================================================================
// BB_PATCH_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BB_PATCH_BASE          0x01D00000

typedef volatile struct
{
} HWP_BB_PATCH_T;

#define hwp_bbPatch                ((HWP_BB_PATCH_T*) KSEG1(REG_BB_PATCH_BASE))




// ============================================================================
// INT_ROM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_INT_ROM_BASE           0x01E00000

typedef volatile struct
{
    UINT8 rom_array[81920];                     //0x00000000
} HWP_INT_ROM_T;

#define hwp_intRom                 ((HWP_INT_ROM_T*) KSEG1(REG_INT_ROM_BASE))




// ============================================================================
// BCPU_ROM_T
// -----------------------------------------------------------------------------
///
// =============================================================================
#define REG_BCPU_ROM_BASE          0x01E80000

typedef volatile struct
{
    UINT8 rom_array[98304];                     //0x00000000
} HWP_BCPU_ROM_T;

#define hwp_bcpuRom                ((HWP_BCPU_ROM_T*) KSEG1(REG_BCPU_ROM_BASE))






#endif

