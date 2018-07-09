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
*           10.08.2016 1.00    First Release
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

#define MCU_RX63T
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
   E            = 0xE             = 512KB/48KB/32KB
   C            = 0xC             = 384KB/32KB/32KB
   B            = 0xB             = 256KB/24KB/32KB
   6            = 0x6             = 64KB/8KB/8KB
   5            = 0x5             = 48KB/8KB/8KB
   4            = 0x4             = 32KB/8KB/8KB
*/
#define MCU_CFG_PART_MEMORY_SIZE    (0x6)


/****************************************************************************************
 Do not change the following values.
****************************************************************************************/

#if MCU_CFG_PART_MEMORY_SIZE == 0x4
    #define MCU_ROM_SIZE_BYTES          (32768)
    #define MCU_RAM_SIZE_BYTES          (8192)
    #define MCU_DATA_FLASH_SIZE_BYTES   (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x5
    #define MCU_ROM_SIZE_BYTES          (49152)
    #define MCU_RAM_SIZE_BYTES          (8192)
    #define MCU_DATA_FLASH_SIZE_BYTES   (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x6
    #define MCU_ROM_SIZE_BYTES          (65536)
    #define MCU_RAM_SIZE_BYTES          (8192)
    #define MCU_DATA_FLASH_SIZE_BYTES   (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xB
    #define MCU_ROM_SIZE_BYTES          (262144)
    #define MCU_RAM_SIZE_BYTES          (24576)
    #define MCU_DATA_FLASH_SIZE_BYTES   (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xC
    #define MCU_ROM_SIZE_BYTES          (393216)
    #define MCU_RAM_SIZE_BYTES          (32768)
    #define MCU_DATA_FLASH_SIZE_BYTES   (32768)
#elif MCU_CFG_PART_MEMORY_SIZE == 0xE
    #define MCU_ROM_SIZE_BYTES          (524288)
    #define MCU_RAM_SIZE_BYTES          (49152)
    #define MCU_DATA_FLASH_SIZE_BYTES   (32768)
#else
    #error "ERROR - MCU_CFG_PART_MEMORY_SIZE - Unknown memory size chosen in r_mcu_config.h"
#endif

#define FIT_NO_PTR      ((void *)0x10000000)    // Reserved space on RX
#define FIT_NO_FUNC     ((void *)0x10000000)    // Reserved space on RX

/* RSKRX63T equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORT7.PODR.BIT.B1
#define LED1            PORT7.PODR.BIT.B2
#define LED2            PORT7.PODR.BIT.B3
#define LED3            PORT3.PODR.BIT.B3
#define SW1             PORT1.PIDR.BIT.B0
#define SW2             PORT1.PIDR.BIT.B1
#define SW3             PORT0.PIDR.BIT.B0
#endif

#endif /* R_BSP Module not used */
#endif /* R_MCU_CONFIG_HEADER_FILE */
