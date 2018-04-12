/*******************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized. This 
* software is owned by Renesas Electronics Corporation and is protected under
* all applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT
* LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE 
* AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.
* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR
* ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE
* BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software
* and to discontinue the availability of this software. By using this software,
* you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer 
*
* Copyright (C) 2012 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_rx610.h
* Description  : This file has specific information about the ROM and DF on 
*                RX610 Group MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 11.09.2012 1.00    First Release
*         : 10.10.2012 1.10    Added FCU_RAM_INIT_REQUIRED macro.
*         : 09.08.2016 2.00    Modified for BSPless support.
*         : 17.11.2016 2.10    Added FLASH_xxx size and num block defines
*         : 02.08.2017 2.20    Replaced #include "r_mcu_config.h" with "r_flash_targets.h"
******************************************************************************/

#ifndef _FLASH_API_RX610_H
#define _FLASH_API_RX610_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include <stdint.h>
#include "r_flash_targets.h"


/******************************************************************************
Macro definitions
******************************************************************************/
/* Defines the number of flash areas */
#define NUM_ROM_AREAS           2    
/* Defines the start program/erase address for the different flash areas */
#define ROM_AREA_0              (0x00F00000)
#define ROM_AREA_1              (0x00E00000)

/* Defines whether this MCU requires the FCU RAM be enabled and initialized.
   If uncommented, then MCU is required to init FCU RAM.
   If commented out, then MCU is not required. */
#define FCU_RAM_INIT_REQUIRED   (1)

/*  Bottom of DF Area */
#define DF_ADDRESS              0x00100000
/* Used for getting DF block */
#define DF_MASK                 0xFFFFE000
/* Used for getting erase boundary in DF block when doing blank checking */
#define DF_ERASE_BLOCK_SIZE     0x00002000
/* Defines how many DF blocks are on this part */
#define DF_NUM_BLOCKS           4
/* THis is the last address in the last block, not the starting address */
#define FLASH_DF_LAST_BLOCK        ((FLASH_DF_BLOCK_INVALID) - (DF_ERASE_BLOCK_SIZE))

/* Defines how many ROM blocks are on this part */
#if MCU_ROM_SIZE_BYTES == 2097152
    #define ROM_NUM_BLOCKS      28    //2MB part
#elif MCU_ROM_SIZE_BYTES == 1572864
    #define ROM_NUM_BLOCKS      24    //1.5MB part
#elif MCU_ROM_SIZE_BYTES == 1048576
    #define ROM_NUM_BLOCKS      20    //1MB part
#elif MCU_ROM_SIZE_BYTES == 786432
    #define ROM_NUM_BLOCKS      18    //768KB part
#endif

/* Programming size for ROM in bytes */
#define ROM_PROGRAM_SIZE        256
/* Programming sizes for data flash in bytes. Some MCUs have two sizes
   (e.g. 8-bytes or 128-bytes) that's why there is a LARGE and SMALL */
#define DF_PROGRAM_SIZE_LARGE   128
#define DF_PROGRAM_SIZE_SMALL   8

#define FLASH_CF_SMALL_BLOCK_SIZE       (8192)
#define FLASH_CF_MEDIUM_BLOCK_SIZE      (65536)
#define FLASH_CF_LARGE_BLOCK_SIZE       (131072)
#define FLASH_NUM_BLOCKS_CF             ROM_NUM_BLOCKS
#define FLASH_CF_MIN_PGM_SIZE           ROM_PROGRAM_SIZE
#define FLASH_DF_BLOCK_SIZE             DF_ERASE_BLOCK_SIZE
#define FLASH_NUM_BLOCKS_DF             DF_NUM_BLOCKS
#define FLASH_DF_MIN_PGM_SIZE           DF_PROGRAM_SIZE_SMALL
#define FLASH_DF_HIGHEST_VALID_BLOCK    FLASH_DF_LAST_BLOCK


typedef enum _flash_block_address
{
    FLASH_CF_BLOCK_0       = 0x00FFE000,    /*   8KB: 0xFFFFE000 - 0xFFFFFFFF */
    FLASH_CF_BLOCK_1       = 0x00FFC000,    /*   8KB: 0xFFFFC000 - 0xFFFFDFFF */
    FLASH_CF_BLOCK_2       = 0x00FFA000,    /*   8KB: 0xFFFFA000 - 0xFFFFBFFF */
    FLASH_CF_BLOCK_3       = 0x00FF8000,    /*   8KB: 0xFFFF8000 - 0xFFFF9FFF */
    FLASH_CF_BLOCK_4       = 0x00FF6000,    /*   8KB: 0xFFFF6000 - 0xFFFF7FFF */
    FLASH_CF_BLOCK_5       = 0x00FF4000,    /*   8KB: 0xFFFF4000 - 0xFFFF5FFF */
    FLASH_CF_BLOCK_6       = 0x00FF2000,    /*   8KB: 0xFFFF2000 - 0xFFFF3FFF */
    FLASH_CF_BLOCK_7       = 0x00FF0000,    /*   8KB: 0xFFFF0000 - 0xFFFF1FFF */
    FLASH_CF_BLOCK_8       = 0x00FE0000,    /*  64KB: 0xFFFE0000 - 0xFFFEFFFF */
    FLASH_CF_BLOCK_9       = 0x00FD0000,    /*  64KB: 0xFFFD0000 - 0xFFFDFFFF */
    FLASH_CF_BLOCK_10      = 0x00FC0000,    /*  64KB: 0xFFFC0000 - 0xFFFCFFFF */
    FLASH_CF_BLOCK_11      = 0x00FB0000,    /*  64KB: 0xFFFB0000 - 0xFFFBFFFF */
    FLASH_CF_BLOCK_12      = 0x00FA0000,    /*  64KB: 0xFFFA0000 - 0xFFFAFFFF */
    FLASH_CF_BLOCK_13      = 0x00F90000,    /*  64KB: 0xFFF90000 - 0xFFF9FFFF */
    FLASH_CF_BLOCK_14      = 0x00F80000,    /*  64KB: 0xFFF80000 - 0xFFF8FFFF */
    FLASH_CF_BLOCK_15      = 0x00F70000,    /*  64KB: 0xFFF70000 - 0xFFF7FFFF */
    FLASH_CF_BLOCK_16      = 0x00F60000,    /*  64KB: 0xFFF60000 - 0xFFF6FFFF */
    FLASH_CF_BLOCK_17      = 0x00F40000,    /* 128KB: 0xFFF40000 - 0xFFF5FFFF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x4   //   768KB ROM
    FLASH_CF_BLOCK_INVALID = 0x00F3FFFF,
#else
    FLASH_CF_BLOCK_18      = 0x00F20000,    /* 128KB: 0xFFF20000 - 0xFFF3FFFF */
    FLASH_CF_BLOCK_19      = 0x00F00000,    /* 128KB: 0xFFF00000 - 0xFFF1FFFF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x6   //   1 MB ROM
    FLASH_CF_BLOCK_INVALID = 0x00EFFFFF,
#else
    FLASH_CF_BLOCK_20      = 0x00EE0000,    /* 128KB: 0xFFEE0000 - 0xFFEFFFFF */
    FLASH_CF_BLOCK_21      = 0x00EC0000,    /* 128KB: 0xFFEC0000 - 0xFFEDFFFF */
    FLASH_CF_BLOCK_22      = 0x00EA0000,    /* 128KB: 0xFFEA0000 - 0xFFEBFFFF */
    FLASH_CF_BLOCK_23      = 0x00E80000,    /* 128KB: 0xFFE80000 - 0xFFE9FFFF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x7   //   1.5 MB ROM
    FLASH_CF_BLOCK_INVALID = 0x00E7FFFF,
#else
    FLASH_CF_BLOCK_24      = 0x00E60000,    /* 128KB: 0xFFE60000 - 0xFFE7FFFF */
    FLASH_CF_BLOCK_25      = 0x00E40000,    /* 128KB: 0xFFE40000 - 0xFFE5FFFF */
    FLASH_CF_BLOCK_26      = 0x00E20000,    /* 128KB: 0xFFE20000 - 0xFFE3FFFF */
    FLASH_CF_BLOCK_27      = 0x00E00000,    /* 128KB: 0xFFE00000 - 0xFFE1FFFF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x8   //   2.0 MB ROM
    FLASH_CF_BLOCK_INVALID = 0x00DFFFFF,
#endif
#endif
#endif
#endif

    FLASH_DF_BLOCK_0       = 0x00100000,    /*    8K: 0x00100000 - 0x00101FFF */
    FLASH_DF_BLOCK_1       = 0x00102000,    /*    8K: 0x00102000 - 0x00103FFF */
    FLASH_DF_BLOCK_2       = 0x00104000,    /*    8K: 0x00104000 - 0x00105FFF */
    FLASH_DF_BLOCK_3       = 0x00106000,    /*    8K: 0x00106000 - 0x00107FFF */
    FLASH_DF_BLOCK_INVALID = 0x00108000     /*    32K: Can't write beyond 0x00107FFF */

} flash_block_address_t;

typedef struct _rom_block_sizes
{
 uint32_t num_blocks;                     // number of blocks at this size
 uint32_t block_size;                     // Size of each block
}rom_block_sizes_t;

typedef struct _rom_block_info
{
 uint32_t start_addr;                   // starting address for this block section
 uint32_t end_addr;                     // ending (up to and including this) address
 uint16_t block_number;                 // the rom block number for this address queried
 uint32_t thisblock_stAddr;             // the starting address for the above block #
 uint32_t block_size;                   // Size of this block

}rom_block_info_t;

#define NUM_BLOCK_TABLE_ENTRIES 4
static rom_block_sizes_t g_flash_RomBlockSizes[NUM_BLOCK_TABLE_ENTRIES] = {
         8,  8193,    /* 8 blocks of 8K   */
        9, 65536,     /* 9 blocks of 64K */
        11, 131072,    /* 11 blocks of 128K */
        0,  0
};


/*  According to HW Manual the Max Programming Time for 256 bytes (ROM)
    is 12ms.  This is with a PCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 12ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(12000 * (50.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 128 bytes
    (Data Flash) is 5ms.  This is with a PCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 5ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(5000 * (50.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 2k bytes
    (Data Flash) is 0.7ms.  This is with a PCLK of 50MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 0.7ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(700 * (50.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))
        
/*  According to HW Manual the max timeout value when using the peripheral
    clock notification command is 60us. This is with a PCLK of 50MHz. The 
    calculation below calculates the number of ICLK ticks needed for the 
    timeout delay. The 10us number is adjusted linearly depending on 
    the PCLK frequency.
*/
#define WAIT_MAX_NOTIFY_FCU_CLOCK \
        ((int32_t)(60 * (50.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Erasure Time for a 128kB block
    is 1750ms.  This is with a PCLK of 50MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 1750ms number is adjusted linearly depending on the PCLK frequency.
*/
#define WAIT_MAX_ERASE \
        ((int32_t)(1750000 * (50.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/******************************************************************************
Error checking
******************************************************************************/
/* FCLK must be between 8MHz and 50MHz. */
#if (MCU_CFG_FCLK_HZ > 50000000) || (MCU_CFG_FCLK_HZ < 8000000)
    #error "ERROR - Flash API - FCLK on RX610 must be between 8MHz and 50MHz!"
#endif

#endif /* _FLASH_API_RX610_H */
