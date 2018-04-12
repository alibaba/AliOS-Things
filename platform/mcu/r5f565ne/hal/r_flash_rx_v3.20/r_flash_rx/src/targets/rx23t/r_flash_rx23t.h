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
* File Name    : r_flash_rx23t.h
* Description  : This file has specific information about the ROM and DF on 
*                RX23T MCUs.
*******************************************************************************
* History : DD.MM.YYYY Version Description
*         : 01.09.2015 1.00    First Release
*         : 22.07.2016 2.00    Modified for BSPless flash. Fixed block range comments.
*                              Modified values for
*                              FLASH_CF_LOWEST_VALID_BLOCK and FLASH_CF_BLOCK_INVALID.
******************************************************************************/

#ifndef _FLASH_API_RX23T_H
#define _FLASH_API_RX23T_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Defines standard typedefs used in this file */
#include <stdint.h>
/******************************************************************************
Macro definitions
******************************************************************************/
#define FLASH_CF_MIN_PGM_SIZE         (8)       // 8 for ROM; 4 for extra area

#define FLASH_CF_BLOCK_SIZE           (2048)

#define FLASH_NUM_BLOCKS_CF           (MCU_ROM_SIZE_BYTES / FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_FULL_SIZE            (FLASH_NUM_BLOCKS_CF*FLASH_CF_BLOCK_SIZE)
#define FLASH_CF_LOWEST_VALID_BLOCK   (FLASH_CF_BLOCK_INVALID + 1)
#define FLASH_CF_LAST_VALID_ADDR      (FLASH_CF_LOWEST_VALID_BLOCK)

/* Delay function Setting */
#define WAIT_DIV_LOOP_CYCLE           (4000) /* 4 cycle of 1 loop * 1000(us) */
#define WAIT_OVERHEAD_COUNT           (5)    /* Overhead is reduced from a loop count.
                                                overhead loop count = 20 cycle / 4 cycle of 1 loop */

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/
typedef enum _flash_block_address
{
    FLASH_CF_BLOCK_END     = 0xFFFFFFFF,    /*   Top  of the CS */
    FLASH_CF_BLOCK_0       = 0xFFFFF800,    /*   2KB: 0xFFFFF800 - 0xFFFFFFFF */
    FLASH_CF_BLOCK_1       = 0xFFFFF000,    /*   2KB: 0xFFFFF000 - 0xFFFFF7FF */
    FLASH_CF_BLOCK_2       = 0xFFFFE800,    /*   2KB: 0xFFFFE800 - 0xFFFFEFFF */
    FLASH_CF_BLOCK_3       = 0xFFFFE000,    /*   2KB: 0xFFFFE000 - 0xFFFFE7FF */
    FLASH_CF_BLOCK_4       = 0xFFFFD800,    /*   2KB: 0xFFFFD800 - 0xFFFFDFFF */
    FLASH_CF_BLOCK_5       = 0xFFFFD000,    /*   2KB: 0xFFFFD000 - 0xFFFFD7FF */
    FLASH_CF_BLOCK_6       = 0xFFFFC800,    /*   2KB: 0xFFFFC800 - 0xFFFFCFFF */
    FLASH_CF_BLOCK_7       = 0xFFFFC000,    /*   2KB: 0xFFFFC000 - 0xFFFFC7FF */
    FLASH_CF_BLOCK_8       = 0xFFFFB800,    /*   2KB: 0xFFFFB800 - 0xFFFFBFFF */
    FLASH_CF_BLOCK_9       = 0xFFFFB000,    /*   2KB: 0xFFFFB000 - 0xFFFFB7FF */
    FLASH_CF_BLOCK_10      = 0xFFFFA800,    /*   2KB: 0xFFFFA800 - 0xFFFFAFFF */
    FLASH_CF_BLOCK_11      = 0xFFFFA000,    /*   2KB: 0xFFFFA000 - 0xFFFFA7FF */
    FLASH_CF_BLOCK_12      = 0xFFFF9800,    /*   2KB: 0xFFFF9800 - 0xFFFF9FFF */
    FLASH_CF_BLOCK_13      = 0xFFFF9000,    /*   2KB: 0xFFFF9000 - 0xFFFF97FF */
    FLASH_CF_BLOCK_14      = 0xFFFF8800,    /*   2KB: 0xFFFF8800 - 0xFFFF8FFF */
    FLASH_CF_BLOCK_15      = 0xFFFF8000,    /*   2KB: 0xFFFF8000 - 0xFFFF87FF */
    FLASH_CF_BLOCK_16      = 0xFFFF7800,    /*   2KB: 0xFFFF7800 - 0xFFFF7FFF */
    FLASH_CF_BLOCK_17      = 0xFFFF7000,    /*   2KB: 0xFFFF7000 - 0xFFFF77FF */
    FLASH_CF_BLOCK_18      = 0xFFFF6800,    /*   2KB: 0xFFFF6800 - 0xFFFF6FFF */
    FLASH_CF_BLOCK_19      = 0xFFFF6000,    /*   2KB: 0xFFFF6000 - 0xFFFF67FF */
    FLASH_CF_BLOCK_20      = 0xFFFF5800,    /*   2KB: 0xFFFF5800 - 0xFFFF5FFF */
    FLASH_CF_BLOCK_21      = 0xFFFF5000,    /*   2KB: 0xFFFF5000 - 0xFFFF57FF */
    FLASH_CF_BLOCK_22      = 0xFFFF4800,    /*   2KB: 0xFFFF4800 - 0xFFFF4FFF */
    FLASH_CF_BLOCK_23      = 0xFFFF4000,    /*   2KB: 0xFFFF4000 - 0xFFFF47FF */
    FLASH_CF_BLOCK_24      = 0xFFFF3800,    /*   2KB: 0xFFFF3800 - 0xFFFF3FFF */
    FLASH_CF_BLOCK_25      = 0xFFFF3000,    /*   2KB: 0xFFFF3000 - 0xFFFF37FF */
    FLASH_CF_BLOCK_26      = 0xFFFF2800,    /*   2KB: 0xFFFF2800 - 0xFFFF2FFF */
    FLASH_CF_BLOCK_27      = 0xFFFF2000,    /*   2KB: 0xFFFF2000 - 0xFFFF27FF */
    FLASH_CF_BLOCK_28      = 0xFFFF1800,    /*   2KB: 0xFFFF1800 - 0xFFFF1FFF */
    FLASH_CF_BLOCK_29      = 0xFFFF1000,    /*   2KB: 0xFFFF1000 - 0xFFFF17FF */
    FLASH_CF_BLOCK_30      = 0xFFFF0800,    /*   2KB: 0xFFFF0800 - 0xFFFF0FFF */
    FLASH_CF_BLOCK_31      = 0xFFFF0000,    /*   2KB: 0xFFFF0000 - 0xFFFF07FF */
#if   MCU_CFG_PART_MEMORY_SIZE == 0x3   /*   '3' parts 64KB ROM */
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_31 - 1),
#else
    FLASH_CF_BLOCK_32      = 0xFFFEF800,    /*   2KB: 0xFFFEF800 - 0xFFFEFFFF */
    FLASH_CF_BLOCK_33      = 0xFFFEF000,    /*   2KB: 0xFFFEF000 - 0xFFFEF7FF */
    FLASH_CF_BLOCK_34      = 0xFFFEE800,    /*   2KB: 0xFFFEE800 - 0xFFFEEFFF */
    FLASH_CF_BLOCK_35      = 0xFFFEE000,    /*   2KB: 0xFFFEE000 - 0xFFFEE7FF */
    FLASH_CF_BLOCK_36      = 0xFFFED800,    /*   2KB: 0xFFFED800 - 0xFFFEDFFF */
    FLASH_CF_BLOCK_37      = 0xFFFED000,    /*   2KB: 0xFFFED000 - 0xFFFED7FF */
    FLASH_CF_BLOCK_38      = 0xFFFEC800,    /*   2KB: 0xFFFEC800 - 0xFFFECFFF */
    FLASH_CF_BLOCK_39      = 0xFFFEC000,    /*   2KB: 0xFFFEC000 - 0xFFFEC7FF */
    FLASH_CF_BLOCK_40      = 0xFFFEB800,    /*   2KB: 0xFFFEB800 - 0xFFFEBFFF */
    FLASH_CF_BLOCK_41      = 0xFFFEB000,    /*   2KB: 0xFFFEB000 - 0xFFFEB7FF */
    FLASH_CF_BLOCK_42      = 0xFFFEA800,    /*   2KB: 0xFFFEA800 - 0xFFFEAFFF */
    FLASH_CF_BLOCK_43      = 0xFFFEA000,    /*   2KB: 0xFFFEA000 - 0xFFFEA7FF */
    FLASH_CF_BLOCK_44      = 0xFFFE9800,    /*   2KB: 0xFFFE9800 - 0xFFFE9FFF */
    FLASH_CF_BLOCK_45      = 0xFFFE9000,    /*   2KB: 0xFFFE9000 - 0xFFFE97FF */
    FLASH_CF_BLOCK_46      = 0xFFFE8800,    /*   2KB: 0xFFFE8800 - 0xFFFE8FFF */
    FLASH_CF_BLOCK_47      = 0xFFFE8000,    /*   2KB: 0xFFFE8000 - 0xFFFE87FF */
    FLASH_CF_BLOCK_48      = 0xFFFE7800,    /*   2KB: 0xFFFE7800 - 0xFFFE7FFF */
    FLASH_CF_BLOCK_49      = 0xFFFE7000,    /*   2KB: 0xFFFE7000 - 0xFFFE77FF */
    FLASH_CF_BLOCK_50      = 0xFFFE6800,    /*   2KB: 0xFFFE6800 - 0xFFFE6FFF */
    FLASH_CF_BLOCK_51      = 0xFFFE6000,    /*   2KB: 0xFFFE6000 - 0xFFFE67FF */
    FLASH_CF_BLOCK_52      = 0xFFFE5800,    /*   2KB: 0xFFFE5800 - 0xFFFE5FFF */
    FLASH_CF_BLOCK_53      = 0xFFFE5000,    /*   2KB: 0xFFFE5000 - 0xFFFE57FF */
    FLASH_CF_BLOCK_54      = 0xFFFE4800,    /*   2KB: 0xFFFE4800 - 0xFFFE4FFF */
    FLASH_CF_BLOCK_55      = 0xFFFE4000,    /*   2KB: 0xFFFE4000 - 0xFFFE47FF */
    FLASH_CF_BLOCK_56      = 0xFFFE3800,    /*   2KB: 0xFFFE3800 - 0xFFFE3FFF */
    FLASH_CF_BLOCK_57      = 0xFFFE3000,    /*   2KB: 0xFFFE3000 - 0xFFFE37FF */
    FLASH_CF_BLOCK_58      = 0xFFFE2800,    /*   2KB: 0xFFFE2800 - 0xFFFE2FFF */
    FLASH_CF_BLOCK_59      = 0xFFFE2000,    /*   2KB: 0xFFFE2000 - 0xFFFE27FF */
    FLASH_CF_BLOCK_60      = 0xFFFE1800,    /*   2KB: 0xFFFE1800 - 0xFFFE1FFF */
    FLASH_CF_BLOCK_61      = 0xFFFE1000,    /*   2KB: 0xFFFE1000 - 0xFFFE17FF */
    FLASH_CF_BLOCK_62      = 0xFFFE0800,    /*   2KB: 0xFFFE0800 - 0xFFFE0FFF */
    FLASH_CF_BLOCK_63      = 0xFFFE0000,    /*   2KB: 0xFFFE0000 - 0xFFFE07FF */
    FLASH_CF_BLOCK_INVALID = (FLASH_CF_BLOCK_63 - 1)  /*   '5' parts 128KB ROM */
#endif
} flash_block_address_t;


/*  According to HW Manual the Max Programming Time for 8 bytes (ROM)
    is 967 us.  This is with a FCLK of 1MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 967 us number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ROM_WRITE \
        ((int32_t)(967 * (MCU_CFG_ICLK_HZ/1000000)))


/*  According to HW Manual the Max Blank Check time for 8 bytes
    is 55 usec.  This is with a FCLK of 1MHz. The calculation
    below calculates the number of ICLK ticks needed for the timeout delay.
    The 55 usec number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_BLANK_CHECK \
        ((int32_t)(55 * (MCU_CFG_ICLK_HZ/1000000)))


/*  According to HW Manual the Max Erasure Time for a 128KB block is
    around 5112ms.  This is with a FCLK of 1MHz. The calculation below
    calculates the number of ICLK ticks needed for the timeout delay.
    The 5112ms number is adjusted linearly depending on the FCLK frequency.
*/
#define WAIT_MAX_ERASE_CF_1K \
        ((int32_t)(5112000 *(MCU_CFG_ICLK_HZ/1000000)))


#endif /* _FLASH_API_RX23T_H */
