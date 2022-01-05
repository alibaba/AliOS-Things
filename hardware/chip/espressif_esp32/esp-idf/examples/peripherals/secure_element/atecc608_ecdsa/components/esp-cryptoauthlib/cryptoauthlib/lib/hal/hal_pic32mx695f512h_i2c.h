/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MX695F512H I2C over xxx drivers.
 *
 * This code is structured in two parts.  Part 1 is the connection of the ATCA HAL API to the physical I2C
 * implementation. Part 2 is the xxx I2C primitives to set up the interface.
 *
 * Prerequisite:
 *
 * \copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip software
 * and any derivatives exclusively with Microchip products. It is your
 * responsibility to comply with third party license terms applicable to your
 * use of third party software (including open source software) that may
 * accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
 * PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT,
 * SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE
 * OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
 * MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
 * FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL
 * LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED
 * THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR
 * THIS SOFTWARE.
 */

#ifndef HAL_PIC32MX695F512H_I2C_ASF_H_
#define HAL_PIC32MX695F512H_I2C_ASF_H_


/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 *
   @{ */

// Clock Constants
#define GetSystemClock()        (80000000ul)
#define GetPeripheralClock()    (GetSystemClock() / (1 << OSCCONbits.PBDIV))
#define GetInstructionClock()   (GetSystemClock())

#define MAX_I2C_BUSES           4   // PIC32MX695F512H has 4 TWI

/**
 * \brief this is the hal_data for ATCA HAL
 */
typedef struct atcaI2Cmaster
{
    I2C_MODULE id;
    int        ref_ct;
    // for conveniences during interface release phase
    int bus_index;
} ATCAI2CMaster_t;

void i2c_write(I2C_MODULE i2c_id, uint8_t address, uint8_t *data, int len);
ATCA_STATUS i2c_read(I2C_MODULE i2c_id, uint8_t address, uint8_t *data, uint16_t len);

void change_i2c_speed(ATCAIface iface, uint32_t speed);

/** @} */

#endif  /* HAL_PIC32MX695F512H_I2C_ASF_H_ */