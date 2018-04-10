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
* Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.    
*******************************************************************************/
/******************************************************************************
* File Name    : r_flash_rx110.h
* Description  : This file has specific information about the ROM and DF on 
*                RX110 MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 10.10.2012 1.00    First Release
*         : 22.07.2016 2.00    Modified for BSPless flash. Modified values for
*                              FLASH_CF_LOWEST_VALID_BLOCK and FLASH_CF_BLOCK_INVALID.
******************************************************************************/

#ifndef _FLASH_API_RX110_H
#define _FLASH_API_RX110_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>
/******************************************************************************
Macro definitions
******************************************************************************/
#define FLASH_NUM_BLOCKS_DF        (8)
#define FLASH_DF_MIN_PGM_SIZE      (1)
#define FLASH_CF_MIN_PGM_SIZE      (4)

#define FLASH_CF_BLOCK_SIZE        (1024)
#define FLASH_DF_BLOCK_SIZE        (1024)
#define FLASH_DF_FULL_SIZE         (FLASH_NUM_BLOCKS_DF*FLASH_DF_BLOCK_SIZE)
#define FLASH_DF_FULL_PGM_SIZE     (FLASH_DF_FULL_SIZE-FLASH_DF_MIN_PGM_SIZE)
#define FLASH_DF_LAST_VALID_ADDR   (FLASH_DF_BLOCK_INVALID-1)
#define FLASH_DF_HIGHEST_VALID_BLOCK  (FLASH_DF_BLOCK_INVALID-FLASH_DF_BLOCK_SIZE)


#define FLASH_NUM_BLOCKS_CF         (MCU_ROM_SIZE_BYTES) / (FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_FULL_SIZE          (FLASH_NUM_BLOCKS_CF*FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_LOWEST_VALID_BLOCK (FLASH_CF_BLOCK_INVALID + 1)
#define FLASH_CF_LAST_VALID_ADDR    (FLASH_CF_LOWEST_VALID_BLOCK)

#define FLASH_CF_256KBOUNDARY       (0xFFFC0000)

/* Delay function Setting */
#define WAIT_DIV_LOOP_CYCLE         (5000) /* 4 cycle of 1 loop * 1000(us) */
#define WAIT_OVERHEAD_COUNT         (4)    /* Overhead is reduced from a loop count.
                                              overhead loop count = 20 cycle / 4 cycle of 1 loop */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum _flash_block_address
{
    FLASH_CF_BLOCK_END     = 0xFFFFFFFF,    /*   Top of the CS */
    FLASH_CF_BLOCK_0       = 0xFFFFFC00,    /*   1KB: 0xFFFFFC00 - 0xFFFFFFFF */
    FLASH_CF_BLOCK_1       = 0xFFFFF800,    /*   1KB: 0xFFFFF800 - 0xFFFFFBFF */
    FLASH_CF_BLOCK_2       = 0xFFFFF400,    /*   1KB: 0xFFFFF400 - 0xFFFFF7FF */
    FLASH_CF_BLOCK_3       = 0xFFFFF000,    /*   1KB: 0xFFFFF000 - 0xFFFFF3FF */
    FLASH_CF_BLOCK_4       = 0xFFFFEC00,    /*   1KB: 0xFFFFEC00 - 0xFFFFEFFF */
    FLASH_CF_BLOCK_5       = 0xFFFFE800,    /*   1KB: 0xFFFFE800 - 0xFFFFEBFF */
    FLASH_CF_BLOCK_6       = 0xFFFFE400,    /*   1KB: 0xFFFFE400 - 0xFFFFE7FF */
    FLASH_CF_BLOCK_7       = 0xFFFFE000,    /*   1KB: 0xFFFFE000 - 0xFFFFE3FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x6   // rx110 'H' part 8KB ROM
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_7 - 1),
#else
    FLASH_CF_BLOCK_8       = 0xFFFFDC00,    /*   1KB: 0xFFFFDC00 - 0xFFFFDFFF */
    FLASH_CF_BLOCK_9       = 0xFFFFD800,    /*   1KB: 0xFFFFD800 - 0xFFFFDBFF */
    FLASH_CF_BLOCK_10      = 0xFFFFD400,    /*   1KB: 0xFFFFD400 - 0xFFFFD7FF */
    FLASH_CF_BLOCK_11      = 0xFFFFD000,    /*   1KB: 0xFFFFD000 - 0xFFFFD3FF */
    FLASH_CF_BLOCK_12      = 0xFFFFCC00,    /*   1KB: 0xFFFFCC00 - 0xFFFFCFFF */
    FLASH_CF_BLOCK_13      = 0xFFFFC800,    /*   1KB: 0xFFFFC800 - 0xFFFFCBFF */
    FLASH_CF_BLOCK_14      = 0xFFFFC400,    /*   1KB: 0xFFFFC400 - 0xFFFFC7FF */
    FLASH_CF_BLOCK_15      = 0xFFFFC000,    /*   1KB: 0xFFFFC000 - 0xFFFFC3FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x0   //  'J' parts 16KB ROM
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_15 - 1),
#else
    FLASH_CF_BLOCK_16      = 0xFFFFBC00,    /*   1KB: 0xFFFFBC00 - 0xFFFFBFFF */
    FLASH_CF_BLOCK_17      = 0xFFFFB800,    /*   1KB: 0xFFFFB800 - 0xFFFFBBFF */
    FLASH_CF_BLOCK_18      = 0xFFFFB400,    /*   1KB: 0xFFFFB400 - 0xFFFFB7FF */
    FLASH_CF_BLOCK_19      = 0xFFFFB000,    /*   1KB: 0xFFFFB000 - 0xFFFFB3FF */
    FLASH_CF_BLOCK_20      = 0xFFFFAC00,    /*   1KB: 0xFFFFAC00 - 0xFFFFAFFF */
    FLASH_CF_BLOCK_21      = 0xFFFFA800,    /*   1KB: 0xFFFFA800 - 0xFFFFABFF */
    FLASH_CF_BLOCK_22      = 0xFFFFA400,    /*   1KB: 0xFFFFA400 - 0xFFFFA7FF */
    FLASH_CF_BLOCK_23      = 0xFFFFA000,    /*   1KB: 0xFFFFA000 - 0xFFFFA3FF */
    FLASH_CF_BLOCK_24      = 0xFFFF9C00,    /*   1KB: 0xFFFF9C00 - 0xFFFF9FFF */
    FLASH_CF_BLOCK_25      = 0xFFFF9800,    /*   1KB: 0xFFFF9800 - 0xFFFF9BFF */
    FLASH_CF_BLOCK_26      = 0xFFFF9400,    /*   1KB: 0xFFFF9400 - 0xFFFF97FF */
    FLASH_CF_BLOCK_27      = 0xFFFF9000,    /*   1KB: 0xFFFF9000 - 0xFFFF93FF */
    FLASH_CF_BLOCK_28      = 0xFFFF8C00,    /*   1KB: 0xFFFF8C00 - 0xFFFF8FFF */
    FLASH_CF_BLOCK_29      = 0xFFFF8800,    /*   1KB: 0xFFFF8800 - 0xFFFF8BFF */
    FLASH_CF_BLOCK_30      = 0xFFFF8400,    /*   1KB: 0xFFFF8400 - 0xFFFF87FF */
    FLASH_CF_BLOCK_31      = 0xFFFF8000,    /*   1KB: 0xFFFF8000 - 0xFFFF83FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x1   // '1' parts 32KB ROM
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_31 - 1),
#else
    FLASH_CF_BLOCK_32      = 0xFFFF7C00,    /*   1KB: 0xFFFF7C00 - 0xFFFF7FFF */
    FLASH_CF_BLOCK_33      = 0xFFFF7800,    /*   1KB: 0xFFFF7800 - 0xFFFF7BFF */
    FLASH_CF_BLOCK_34      = 0xFFFF7400,    /*   1KB: 0xFFFF7400 - 0xFFFF77FF */
    FLASH_CF_BLOCK_35      = 0xFFFF7000,    /*   1KB: 0xFFFF7000 - 0xFFFF73FF */
    FLASH_CF_BLOCK_36      = 0xFFFF6C00,    /*   1KB: 0xFFFF6C00 - 0xFFFF6FFF */
    FLASH_CF_BLOCK_37      = 0xFFFF6800,    /*   1KB: 0xFFFF6800 - 0xFFFF6BFF */
    FLASH_CF_BLOCK_38      = 0xFFFF6400,    /*   1KB: 0xFFFF6400 - 0xFFFF67FF */
    FLASH_CF_BLOCK_39      = 0xFFFF6000,    /*   1KB: 0xFFFF6000 - 0xFFFF63FF */
    FLASH_CF_BLOCK_40      = 0xFFFF5C00,    /*   1KB: 0xFFFF5C00 - 0xFFFF5FFF */
    FLASH_CF_BLOCK_41      = 0xFFFF5800,    /*   1KB: 0xFFFF5800 - 0xFFFF5BFF */
    FLASH_CF_BLOCK_42      = 0xFFFF5400,    /*   1KB: 0xFFFF5400 - 0xFFFF57FF */
    FLASH_CF_BLOCK_43      = 0xFFFF5000,    /*   1KB: 0xFFFF5000 - 0xFFFF53FF */
    FLASH_CF_BLOCK_44      = 0xFFFF4C00,    /*   1KB: 0xFFFF4C00 - 0xFFFF4FFF */
    FLASH_CF_BLOCK_45      = 0xFFFF4800,    /*   1KB: 0xFFFF4800 - 0xFFFF4BFF */
    FLASH_CF_BLOCK_46      = 0xFFFF4400,    /*   1KB: 0xFFFF4400 - 0xFFFF47FF */
    FLASH_CF_BLOCK_47      = 0xFFFF4000,    /*   1KB: 0xFFFF4000 - 0xFFFF43FF */
    FLASH_CF_BLOCK_48      = 0xFFFF3C00,    /*   1KB: 0xFFFF3C00 - 0xFFFF3FFF */
    FLASH_CF_BLOCK_49      = 0xFFFF3800,    /*   1KB: 0xFFFF3800 - 0xFFFF3BFF */
    FLASH_CF_BLOCK_50      = 0xFFFF3400,    /*   1KB: 0xFFFF3400 - 0xFFFF37FF */
    FLASH_CF_BLOCK_51      = 0xFFFF3000,    /*   1KB: 0xFFFF3000 - 0xFFFF33FF */
    FLASH_CF_BLOCK_52      = 0xFFFF2C00,    /*   1KB: 0xFFFF2C00 - 0xFFFF2FFF */
    FLASH_CF_BLOCK_53      = 0xFFFF2800,    /*   1KB: 0xFFFF2800 - 0xFFFF2BFF */
    FLASH_CF_BLOCK_54      = 0xFFFF2400,    /*   1KB: 0xFFFF2400 - 0xFFFF27FF */
    FLASH_CF_BLOCK_55      = 0xFFFF2000,    /*   1KB: 0xFFFF2000 - 0xFFFF23FF */
    FLASH_CF_BLOCK_56      = 0xFFFF1C00,    /*   1KB: 0xFFFF1C00 - 0xFFFF1FFF */
    FLASH_CF_BLOCK_57      = 0xFFFF1800,    /*   1KB: 0xFFFF1800 - 0xFFFF1BFF */
    FLASH_CF_BLOCK_58      = 0xFFFF1400,    /*   1KB: 0xFFFF1400 - 0xFFFF17FF */
    FLASH_CF_BLOCK_59      = 0xFFFF1000,    /*   1KB: 0xFFFF1000 - 0xFFFF13FF */
    FLASH_CF_BLOCK_60      = 0xFFFF0C00,    /*   1KB: 0xFFFF0C00 - 0xFFFF0FFF */
    FLASH_CF_BLOCK_61      = 0xFFFF0800,    /*   1KB: 0xFFFF0800 - 0xFFFF0BFF */
    FLASH_CF_BLOCK_62      = 0xFFFF0400,    /*   1KB: 0xFFFF0400 - 0xFFFF07FF */
    FLASH_CF_BLOCK_63      = 0xFFFF0000,    /*   1KB: 0xFFFF0000 - 0xFFFF03FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x3   //   '3' parts 64KB ROM
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_63 - 1)
#else
    FLASH_CF_BLOCK_64      = 0xFFFEFC00,    /*   1KB: 0xFFFEFC00 - 0xFFFEFFFF */
    FLASH_CF_BLOCK_65      = 0xFFFEF800,    /*   1KB: 0xFFFEF800 - 0xFFFEFBFF */
    FLASH_CF_BLOCK_66      = 0xFFFEF400,    /*   1KB: 0xFFFEF400 - 0xFFFEF7FF */
    FLASH_CF_BLOCK_67      = 0xFFFEF000,    /*   1KB: 0xFFFEF000 - 0xFFFEF3FF */
    FLASH_CF_BLOCK_68      = 0xFFFEEC00,    /*   1KB: 0xFFFEEC00 - 0xFFFEEFFF */
    FLASH_CF_BLOCK_69      = 0xFFFEE800,    /*   1KB: 0xFFFEE800 - 0xFFFEEBFF */
    FLASH_CF_BLOCK_70      = 0xFFFEE400,    /*   1KB: 0xFFFEE400 - 0xFFFEE7FF */
    FLASH_CF_BLOCK_71      = 0xFFFEE000,    /*   1KB: 0xFFFEE000 - 0xFFFEE3FF */
    FLASH_CF_BLOCK_72      = 0xFFFEDC00,    /*   1KB: 0xFFFEDC00 - 0xFFFEDFFF */
    FLASH_CF_BLOCK_73      = 0xFFFED800,    /*   1KB: 0xFFFED800 - 0xFFFEDBFF */
    FLASH_CF_BLOCK_74      = 0xFFFED400,    /*   1KB: 0xFFFED400 - 0xFFFED7FF */
    FLASH_CF_BLOCK_75      = 0xFFFED000,    /*   1KB: 0xFFFED000 - 0xFFFED3FF */
    FLASH_CF_BLOCK_76      = 0xFFFECC00,    /*   1KB: 0xFFFECC00 - 0xFFFECFFF */
    FLASH_CF_BLOCK_77      = 0xFFFEC800,    /*   1KB: 0xFFFEC800 - 0xFFFECBFF */
    FLASH_CF_BLOCK_78      = 0xFFFEC400,    /*   1KB: 0xFFFEC400 - 0xFFFEC7FF */
    FLASH_CF_BLOCK_79      = 0xFFFEC000,    /*   1KB: 0xFFFEC000 - 0xFFFEC3FF */
    FLASH_CF_BLOCK_80      = 0xFFFEBC00,    /*   1KB: 0xFFFEBC00 - 0xFFFEBFFF */
    FLASH_CF_BLOCK_81      = 0xFFFEB800,    /*   1KB: 0xFFFEB800 - 0xFFFEBBFF */
    FLASH_CF_BLOCK_82      = 0xFFFEB400,    /*   1KB: 0xFFFEB400 - 0xFFFEB7FF */
    FLASH_CF_BLOCK_83      = 0xFFFEB000,    /*   1KB: 0xFFFEB000 - 0xFFFEB3FF */
    FLASH_CF_BLOCK_84      = 0xFFFEAC00,    /*   1KB: 0xFFFEAC00 - 0xFFFEAFFF */
    FLASH_CF_BLOCK_85      = 0xFFFEA800,    /*   1KB: 0xFFFEA800 - 0xFFFEABFF */
    FLASH_CF_BLOCK_86      = 0xFFFEA400,    /*   1KB: 0xFFFEA400 - 0xFFFEA7FF */
    FLASH_CF_BLOCK_87      = 0xFFFEA000,    /*   1KB: 0xFFFEA000 - 0xFFFEA3FF */
    FLASH_CF_BLOCK_88      = 0xFFFE9C00,    /*   1KB: 0xFFFE9C00 - 0xFFFE9FFF */
    FLASH_CF_BLOCK_89      = 0xFFFE9800,    /*   1KB: 0xFFFE9800 - 0xFFFE9BFF */
    FLASH_CF_BLOCK_90      = 0xFFFE9400,    /*   1KB: 0xFFFE9400 - 0xFFFE97FF */
    FLASH_CF_BLOCK_91      = 0xFFFE9000,    /*   1KB: 0xFFFE9000 - 0xFFFE93FF */
    FLASH_CF_BLOCK_92      = 0xFFFE8C00,    /*   1KB: 0xFFFE8C00 - 0xFFFE8FFF */
    FLASH_CF_BLOCK_93      = 0xFFFE8800,    /*   1KB: 0xFFFE8800 - 0xFFFE8BFF */
    FLASH_CF_BLOCK_94      = 0xFFFE8400,    /*   1KB: 0xFFFE8400 - 0xFFFE87FF */
    FLASH_CF_BLOCK_95      = 0xFFFE8000,    /*   1KB: 0xFFFE8000 - 0xFFFE83FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x4   //   '4' parts 96KB ROM
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_95 - 1),
#else
    FLASH_CF_BLOCK_96      = 0xFFFE7C00,    /*   1KB: 0xFFFE7C00 - 0xFFFE7FFF */
    FLASH_CF_BLOCK_97      = 0xFFFE7800,    /*   1KB: 0xFFFE7800 - 0xFFFE7BFF */
    FLASH_CF_BLOCK_98      = 0xFFFE7400,    /*   1KB: 0xFFFE7400 - 0xFFFE77FF */
    FLASH_CF_BLOCK_99      = 0xFFFE7000,    /*   1KB: 0xFFFE7000 - 0xFFFE73FF */
    FLASH_CF_BLOCK_100     = 0xFFFE6C00,    /*   1KB: 0xFFFE6C00 - 0xFFFE6FFF */
    FLASH_CF_BLOCK_101     = 0xFFFE6800,    /*   1KB: 0xFFFE6800 - 0xFFFE6BFF */
    FLASH_CF_BLOCK_102     = 0xFFFE6400,    /*   1KB: 0xFFFE6400 - 0xFFFE67FF */
    FLASH_CF_BLOCK_103     = 0xFFFE6000,    /*   1KB: 0xFFFE6000 - 0xFFFE63FF */
    FLASH_CF_BLOCK_104     = 0xFFFE5C00,    /*   1KB: 0xFFFE5C00 - 0xFFFE5FFF */
    FLASH_CF_BLOCK_105     = 0xFFFE5800,    /*   1KB: 0xFFFE5800 - 0xFFFE5BFF */
    FLASH_CF_BLOCK_106     = 0xFFFE5400,    /*   1KB: 0xFFFE5400 - 0xFFFE57FF */
    FLASH_CF_BLOCK_107     = 0xFFFE5000,    /*   1KB: 0xFFFE5000 - 0xFFFE53FF */
    FLASH_CF_BLOCK_108     = 0xFFFE4C00,    /*   1KB: 0xFFFE4C00 - 0xFFFE4FFF */
    FLASH_CF_BLOCK_109     = 0xFFFE4800,    /*   1KB: 0xFFFE4800 - 0xFFFE4BFF */
    FLASH_CF_BLOCK_110     = 0xFFFE4400,    /*   1KB: 0xFFFE4400 - 0xFFFE47FF */
    FLASH_CF_BLOCK_111     = 0xFFFE4000,    /*   1KB: 0xFFFE4000 - 0xFFFE43FF */
    FLASH_CF_BLOCK_112     = 0xFFFE3C00,    /*   1KB: 0xFFFE3C00 - 0xFFFE3FFF */
    FLASH_CF_BLOCK_113     = 0xFFFE3800,    /*   1KB: 0xFFFE3800 - 0xFFFE3BFF */
    FLASH_CF_BLOCK_114     = 0xFFFE3400,    /*   1KB: 0xFFFE3400 - 0xFFFE37FF */
    FLASH_CF_BLOCK_115     = 0xFFFE3000,    /*   1KB: 0xFFFE3000 - 0xFFFE33FF */
    FLASH_CF_BLOCK_116     = 0xFFFE2C00,    /*   1KB: 0xFFFE2C00 - 0xFFFE2FFF */
    FLASH_CF_BLOCK_117     = 0xFFFE2800,    /*   1KB: 0xFFFE2800 - 0xFFFE2BFF */
    FLASH_CF_BLOCK_118     = 0xFFFE2400,    /*   1KB: 0xFFFE2400 - 0xFFFE27FF */
    FLASH_CF_BLOCK_119     = 0xFFFE2000,    /*   1KB: 0xFFFE2000 - 0xFFFE23FF */
    FLASH_CF_BLOCK_120     = 0xFFFE1C00,    /*   1KB: 0xFFFE1C00 - 0xFFFE1FFF */
    FLASH_CF_BLOCK_121     = 0xFFFE1800,    /*   1KB: 0xFFFE1800 - 0xFFFE1BFF */
    FLASH_CF_BLOCK_122     = 0xFFFE1400,    /*   1KB: 0xFFFE1400 - 0xFFFE17FF */
    FLASH_CF_BLOCK_123     = 0xFFFE1000,    /*   1KB: 0xFFFE1000 - 0xFFFE13FF */
    FLASH_CF_BLOCK_124     = 0xFFFE0C00,    /*   1KB: 0xFFFE0C00 - 0xFFFE0FFF */
    FLASH_CF_BLOCK_125     = 0xFFFE0800,    /*   1KB: 0xFFFE0800 - 0xFFFE0BFF */
    FLASH_CF_BLOCK_126     = 0xFFFE0400,    /*   1KB: 0xFFFE0400 - 0xFFFE07FF */
    FLASH_CF_BLOCK_127     = 0xFFFE0000,    /*   1KB: 0xFFFE0000 - 0xFFFE03FF */
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_127 - 1)
#endif
#endif
#endif
#endif
#endif

} flash_block_address_t;


/*  According to HW Manual the Max Programming Time for 4 bytes (ROM)
    is 490 us.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 490us number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(490 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Programming Time for 1 bytes
    (Data Flash) is 374 us.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 374 us number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_DF_WRITE \
        ((int32_t)(374 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))

/*  According to HW Manual the Max Blank Check time for 1 bytes
    (Data Flash) is 16 usec.  This is with a FCLK of 32MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 16 usec number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(16 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))


/*  According to HW Manual the Max Erasure Time for a 1KB block is
    around 214ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 214ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE_CF_1K \
        ((int32_t)(214000 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))


/*  According to HW Manual the Max Erasure Time for a 1KB Data Flash block is
    around 228ms.  This is with a FCLK of 32MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 228ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE_DF_1K \
        ((int32_t)(228000 * (32.0/(MCU_CFG_FCLK_HZ/1000000)))*(MCU_CFG_ICLK_HZ/1000000))
        
#define WAIT_MAX_ERASE_DF   WAIT_MAX_ERASE_DF_1K



#endif /* _FLASH_API_RX110_H */
