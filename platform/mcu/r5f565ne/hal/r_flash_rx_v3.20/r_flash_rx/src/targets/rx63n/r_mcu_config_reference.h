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
*           08.08.2016 1.00    First Release
*           25.05.2017 2.20    Added equates for several new MCU_CFG_PART_MEMORY_SIZEs.
*           02.08.2017 2.10    Moved FLASH_CFG_USE_FIT_BSP == 1 case to r_flash_targets.h.
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

#define MCU_RX63N
#define MCU_RX631
#define MCU_RX63_ALL

/****************************************************************************************
 MCU Configuration when not using r_bsp:
 Configuration options are set here
 Change the values of the following defines as needed.
 DON'T FORGET TO SELECT CLOCK (if LOCO not desired) AND
 ENABLE GLOBAL INTERRUPTS IN YOUR MAIN APPLICATION (normally done by BSP).
****************************************************************************************/

#define MCU_CFG_ICLK_HZ             (96000000)
#define MCU_CFG_FCLK_HZ             (48000000)

/* ROM, RAM, and Data Flash Capacity.
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   F            = 0xF             = 2MB/256KB/32KB
   K            = 0x14            = 2MB/192KB/32KB
   E            = 0xE             = 2MB/128KB/32KB
   J            = 0x13            = 1.5MB/256KB/32KB
   G            = 0x10            = 1.5MB/192KB/32KB
   D            = 0xD             = 1.5MB/128KB/32KB
   Y            = 0x22            = 1MB/256KB/32KB
   W            = 0x20            = 1MB/192KB/32KB
   B            = 0xB             = 1MB/128KB/32KB
   A            = 0xA             = 768KB/128KB/32KB
   8            = 0x8             = 512KB/128KB/32KB
   7            = 0x7             = 384KB/128KB/32KB
   6            = 0x6             = 256KB/128KB/32KB
   P            = 0x19            = 512KB/64KB/32KB
   N            = 0x17            = 384KB/64KB/32KB
   M            = 0x16            = 256KB/64KB/32KB
   0            = 0x0             = 0/128KB/0
*/
#define MCU_CFG_PART_MEMORY_SIZE    (0xB)


/****************************************************************************************
 Do not change the following values.
****************************************************************************************/

#if   MCU_CFG_PART_MEMORY_SIZE == 0x0                /* 0/128KB/0 */
    #define MCU_ROM_SIZE_BYTES              (0)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (0)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x6                /* 256KB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (262144)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x7                /* 384KB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (393216)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x8                /* 512KB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (524288)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xA                /* 768KB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (786432)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xB                /* 1MB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1048576)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xD                /* 1.5MB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1572864)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xE                /* 2MB/128KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (2097152)
    #define MCU_RAM_SIZE_BYTES              (131072)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xF                /* 2MB/256KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (2097152)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x10               /* 1.5MB/192KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1572864)
    #define MCU_RAM_SIZE_BYTES              (196608)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x13               /* 1.5MB/256KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1572864)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x14               /* 2MB/192KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (2097152)
    #define MCU_RAM_SIZE_BYTES              (196608)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x16               /* 256KB/64KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (262144)
    #define MCU_RAM_SIZE_BYTES              (65536)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x17               /* 384KB/64KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (393216)
    #define MCU_RAM_SIZE_BYTES              (65536)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x19               /* 512KB/64KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (524288)
    #define MCU_RAM_SIZE_BYTES              (65536)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x20               /* 1MB/192KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1048576)
    #define MCU_RAM_SIZE_BYTES              (196608)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x22               /* 1MB/256KB/32KB */
    #define MCU_ROM_SIZE_BYTES              (1048576)
    #define MCU_RAM_SIZE_BYTES              (262144)
    #define MCU_DATA_FLASH_SIZE_BYTES       (32768)
#else
    #error "ERROR - MCU_CFG_PART_MEMORY_SIZE - Unknown memory size chosen in r_mcu_config.h"
#endif

#define FIT_NO_PTR      ((void *)0x10000000)    // Reserved space on RX
#define FIT_NO_FUNC     ((void *)0x10000000)    // Reserved space on RX

/* RDKRX63N equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED4            PORTD.PODR.BIT.B5
#define LED5            PORTE.PODR.BIT.B3
#define LED6            PORTD.PODR.BIT.B2
#define LED7            PORTE.PODR.BIT.B0
#define LED8            PORTD.PODR.BIT.B4
#define LED9            PORTE.PODR.BIT.B2
#define LED10           PORTD.PODR.BIT.B1
#define LED11           PORTD.PODR.BIT.B7
#define LED12           PORTD.PODR.BIT.B3
#define LED13           PORTE.PODR.BIT.B1
#define LED14           PORTD.PODR.BIT.B0
#define LED15           PORTD.PODR.BIT.B6
#define SW1             PORT4.PIDR.BIT.B0
#define SW2             PORT4.PIDR.BIT.B1
#define SW3             PORT4.PIDR.BIT.B4
#endif

#endif /* R_BSP Module not used */
#endif /* R_MCU_CONFIG_HEADER_FILE */
