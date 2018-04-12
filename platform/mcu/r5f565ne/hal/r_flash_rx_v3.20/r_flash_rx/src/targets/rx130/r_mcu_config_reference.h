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
*           22.07.2016 1.00    First Release
*           22.05.2017 1.10    Added memory sizes 0x6, 0x7, and 0x8 (512K support)
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

#define MCU_RX130
#define MCU_RX13_ALL

/****************************************************************************************
 MCU Configuration when not using r_bsp:
 Configuration options are set here
 Change the values of the following defines as needed.
 DON'T FORGET TO SELECT CLOCK (if LOCO not desired) AND
 ENABLE GLOBAL INTERRUPTS IN YOUR MAIN APPLICATION (normally done by BSP).
****************************************************************************************/

#define MCU_CFG_ICLK_HZ             (32000000)
#define MCU_CFG_FCLK_HZ             (32000000)

/* ROM, RAM, and Data Flash Capacity. 
   Character(s) = Value for macro = ROM Size/Ram Size/Data Flash Size
   8            = 0x8             = 512KB/48KB/8KB
   7            = 0x7             = 384KB/48KB/8KB
   6            = 0x6             = 256KB/32KB/8KB
   5            = 0x5             = 128KB/16KB/8KB
   3            = 0x3             = 64KB/10KB/8KB
*/
#define MCU_CFG_PART_MEMORY_SIZE    (0x5)

/****************************************************************************************
 Do not change the following values.
****************************************************************************************/

#if MCU_CFG_PART_MEMORY_SIZE == 0x3
    #define MCU_ROM_SIZE_BYTES              (65536L)
    #define MCU_RAM_SIZE_BYTES              (10240)
    #define MCU_DATA_FLASH_SIZE_BYTES       (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x5
    #define MCU_ROM_SIZE_BYTES              (131072L)
    #define MCU_RAM_SIZE_BYTES              (16384)
    #define MCU_DATA_FLASH_SIZE_BYTES       (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x6
    #define MCU_ROM_SIZE_BYTES              (262144L)
    #define MCU_RAM_SIZE_BYTES              (32768)
    #define MCU_DATA_FLASH_SIZE_BYTES       (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x7
    #define MCU_ROM_SIZE_BYTES              (393216L)
    #define MCU_RAM_SIZE_BYTES              (49152)
    #define MCU_DATA_FLASH_SIZE_BYTES       (8192)
#elif MCU_CFG_PART_MEMORY_SIZE == 0x8
    #define MCU_ROM_SIZE_BYTES              (524288L)
    #define MCU_RAM_SIZE_BYTES              (49152)
    #define MCU_DATA_FLASH_SIZE_BYTES       (8192)
#else
    #error "ERROR - MCU_CFG_PART_MEMORY_SIZE - Unknown memory size chosen in r_mcu_config.h"
#endif


/* FIT equates */
#define FIT_NO_PTR      ((void *)0x10000000)    // Reserved space on RX
#define FIT_NO_FUNC     ((void *)0x10000000)    // Reserved space on RX

/* RSKRX130 equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORT2.PODR.BIT.B1
#define LED1            PORT0.PODR.BIT.B4
#define LED2            PORT0.PODR.BIT.B6
#define LED3            PORT0.PODR.BIT.B7
#define SW1             PORT3.PIDR.BIT.B1
#define SW2             PORT3.PIDR.BIT.B2
#define SW3             PORT1.PIDR.BIT.B6
#endif

/* RSKRX130-512KB equates for reference. Change to #if 1 if want to use. */
#if 0
#define LED_ON          (0)
#define LED_OFF         (1)
#define LED0            PORTD.PODR.BIT.B3
#define LED1            PORTD.PODR.BIT.B4
#define LED2            PORTE.PODR.BIT.B6
#define LED3            PORTE.PODR.BIT.B7
#define SW1             PORT3.PIDR.BIT.B1
#define SW2             PORT3.PIDR.BIT.B2
#define SW3             PORT1.PIDR.BIT.B6
#endif

#endif /* R_BSP Module not used */
#endif /* R_MCU_CONFIG_HEADER_FILE */
