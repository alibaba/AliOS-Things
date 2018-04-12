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
*           12.07.2016 1.00    First Release
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

#define MCU_RX71M
#define MCU_RX71_ALL

/****************************************************************************************
 MCU Configuration when not using r_bsp:
 Configuration options are set here
 Change the values of the following defines as needed.
 DON'T FORGET TO SELECT CLOCK (if LOCO not desired) AND
 ENABLE GLOBAL INTERRUPTS IN YOUR MAIN APPLICATION (normally done by BSP).
****************************************************************************************/

#define MCU_CFG_ICLK_HZ             (240000000)
#define MCU_CFG_FCLK_HZ             (60000000)

/* ROM, RAM, and Data Flash Capacity.
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   L            = 0x15            = 4MB/512KB/64KB
   J            = 0x13            = 3MB/512KB/64KB
   G            = 0x10            = 2.5MB/512KB/64KB
   F            = 0xF             = 2MB/512KB/64KB
*/
#define MCU_CFG_PART_MEMORY_SIZE    (0x15)


/****************************************************************************************
 Do not change the following values.
****************************************************************************************/

#if MCU_CFG_PART_MEMORY_SIZE == 0xF
    #define MCU_ROM_SIZE_BYTES              (2097152)
    #define MCU_RAM_SIZE_BYTES              (524288)
    #define MCU_DATA_FLASH_SIZE_BYTES       (65536)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x10
    #define MCU_ROM_SIZE_BYTES              (2621440)
    #define MCU_RAM_SIZE_BYTES              (524288)
    #define MCU_DATA_FLASH_SIZE_BYTES       (65536)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x13
    #define MCU_ROM_SIZE_BYTES              (3145728)
    #define MCU_RAM_SIZE_BYTES              (524288)
    #define MCU_DATA_FLASH_SIZE_BYTES       (65536)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x15
    #define MCU_ROM_SIZE_BYTES              (4194304)
    #define MCU_RAM_SIZE_BYTES              (524288)
    #define MCU_DATA_FLASH_SIZE_BYTES       (65536)
#else
    #error "ERROR - MCU_CFG_PART_MEMORY_SIZE - Unknown memory size chosen in r_mcu_config.h"
#endif

#define FIT_NO_PTR      ((void *)0x10000000)    // Reserved space on RX
#define FIT_NO_FUNC     ((void *)0x10000000)    // Reserved space on RX

/* RSKRX71M equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORT0.PODR.BIT.B3
#define LED1            PORT0.PODR.BIT.B5
#define LED2            PORT2.PODR.BIT.B6
#define LED3            PORT2.PODR.BIT.B7
#define SW1             PORT1.PIDR.BIT.B5
#define SW2             PORT1.PIDR.BIT.B2
#define SW3             PORT0.PIDR.BIT.B7
#endif

#endif /* R_BSP Module not used */
#endif /* R_MCU_CONFIG_HEADER_FILE */
