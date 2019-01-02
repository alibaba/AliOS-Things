/*
 * Copyright (c) 2012, Freescale Semiconductor, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of Freescale Semiconductor, Inc. nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#if !defined(__OBDS_HARDWARE_H__)
#define __OBDS_HARDWARE_H__

#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////

//! @name Android_Buttons test defines
//@{
#define HOME_BUTTON_GPIO_INST     GPIO_PORT1
#define HOME_BUTTON_GPIO_NUM      11
#define BACK_BUTTON_GPIO_INST     GPIO_PORT1
#define BACK_BUTTON_GPIO_NUM      12
#define PROG_BUTTON_GPIO_INST     GPIO_PORT2
#define PROG_BUTTON_GPIO_NUM      12
#define VOLPLUS_BUTTON_GPIO_INST  GPIO_PORT2
#define VOLPLUS_BUTTON_GPIO_NUM   15
#define VOLMINUS_BUTTON_GPIO_INST GPIO_PORT5
#define VOLMINUS_BUTTON_GPIO_NUM  14
//@}

//! @brief I2C addresses for devices tested by OBDS.
enum
{
    MMA8450_I2C_ID = 0x1C,
    MMA8451_I2C_ID = 0x1C,
    MAG3112_I2C_ID = 0x1D,
    ISL29023_I2C_ID = 0x44,
    MAG3110_I2C_ID = 0x0E,
    PPL3115_I2C_ID = 0x60,
    EMC1046_I2C_ID = 0x4C,
    MC1323_I2C_ID = 0x4C,
    MAX11801_I2C_ID = (0x90 >> 1),
    PMIC_MC13892_I2C_ADDR = 0x8
};

//! @brief SPI-NOR device types
enum
{
    AT45DB321D = 1,
    M25P32 = 2
};

//! @brief Flag decides the SPI-NOR device type
extern uint32_t spi_nor_flash_type;

//! @brief List of displays supported by the board.
extern int32_t ipu_display_panel[];

#endif // __OBDS_HARDWARE_H__
////////////////////////////////////////////////////////////////////////////////
// EOF
////////////////////////////////////////////////////////////////////////////////
