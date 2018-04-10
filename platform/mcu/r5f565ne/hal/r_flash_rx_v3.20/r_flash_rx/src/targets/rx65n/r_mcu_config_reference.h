/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
 * other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
 * THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
 * EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
 * SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
 * SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
 * this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2016 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
 * File Name    : r_mcu_config_reference.h
 * Description  : Defines the FLASH MCU configuration when not using the FIT BSP.
 ***********************************************************************************************************************/
/***********************************************************************************************************************
* History : DD.MM.YYYY Version Description
*           11.07.2016 1.00    First Release
*           24.01.2017 1.10    Added equates for RSKRX65N-2
*           02.08.2017 1.10    Moved FLASH_CFG_USE_FIT_BSP == 1 case to r_flash_targets.h.
***********************************************************************************************************************/
#ifndef R_MCU_CONFIG_HEADER_FILE
#define R_MCU_CONFIG_HEADER_FILE

#include "r_flash_rx_config.h"

#if (FLASH_CFG_USE_FIT_BSP == 0)

#include <stdint.h>     // typedefs
#include <stdbool.h>
#include <stddef.h>     // NULL
#include <machine.h>    // nop(), xchg()
#include "iodefine.h"

#define MCU_RX65N
#define MCU_RX65_ALL

/****************************************************************************************
 MCU Configuration when not using r_bsp:
 Configuration options are set here
 Change the values of the following defines as needed.
 DON'T FORGET TO SELECT CLOCK (if LOCO not desired) AND
 ENABLE GLOBAL INTERRUPTS IN YOUR MAIN APPLICATION (normally done by BSP).
****************************************************************************************/

#define MCU_CFG_ICLK_HZ             (120000000)
#define MCU_CFG_FCLK_HZ             (60000000)

/* ROM, RAM, and Data Flash Capacity.
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   4            = 0x4             = 512KB/256KB/Not equipped
   7            = 0x7             = 768KB/256KB/Not equipped
   9            = 0x9             = 1MB/256KB/Not equipped  - RSK+RX65N-1
   C            = 0xC             = 1.5MB/640KB/32KB
   E            = 0xE             = 2MB/640KB/32KB          - RSK+RX65N-2
   NOTE: When the RAM capacity is 640KB, the RAM areas are not contiguous
*/
#define MCU_CFG_PART_MEMORY_SIZE    (0x9)

/****************************************************************************************
 Do not change the following values.
****************************************************************************************/
#if (MCU_CFG_PART_MEMORY_SIZE == 0x4)
    #define MCU_ROM_SIZE_BYTES              (524288)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (0)
#elif (MCU_CFG_PART_MEMORY_SIZE == 0x7)
    #define MCU_ROM_SIZE_BYTES              (786432)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (0)
#elif (MCU_CFG_PART_MEMORY_SIZE == 0x9)
    #define MCU_ROM_SIZE_BYTES              (1048576)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (0)
#elif (MCU_CFG_PART_MEMORY_SIZE == 0xC)
    #define MCU_ROM_SIZE_BYTES              (1572864)
    #define MCU_RAM_SIZE_BYTES              (655360)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif (MCU_CFG_PART_MEMORY_SIZE == 0xE)
    #define MCU_ROM_SIZE_BYTES              (2097152)
    #define MCU_RAM_SIZE_BYTES              (655360)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#else
    #error "ERROR - MCU_CFG_PART_MEMORY_SIZE - Unknown memory size chosen in r_mcu_config.h"
#endif

#define FIT_NO_PTR      ((void *)0x10000000)    // Reserved space on RX
#define FIT_NO_FUNC     ((void *)0x10000000)    // Reserved space on RX

/* RSKRX65N-1 equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORT0.PODR.BIT.B3
#define LED1            PORT0.PODR.BIT.B5
#define LED2            PORT7.PODR.BIT.B3
#define LED3            PORTJ.PODR.BIT.B5
#define SW1             PORT0.PIDR.BIT.B0
#define SW2             PORT0.PIDR.BIT.B1
#define SW3             PORT0.PIDR.BIT.B7
#endif

/* RSKRX65N-2 equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORT7.PODR.BIT.B3
#define LED1            PORTG.PODR.BIT.B7
#define LED2            PORTG.PODR.BIT.B6
#define LED3            PORTG.PODR.BIT.B5
#define SW1             PORT0.PIDR.BIT.B3
#define SW2             PORT0.PIDR.BIT.B5
#define SW3             PORT0.PIDR.BIT.B7
#endif

#endif /* R_BSP Module not used */
#endif /* R_MCU_CONFIG_HEADER_FILE */
