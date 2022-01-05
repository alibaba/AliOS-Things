/**
 * \file
 * \brief ATCA Hardware abstraction layer for SAMB11 I2C over ASF drivers.
 *
 * Prerequisite: add SERCOM I2C Master Polled support to application in Atmel Studio
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

#ifndef HAL_SAMB11_I2C_ASF_H_
#define HAL_SAMB11_I2C_ASF_H_

#include <asf.h>

/** \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of ASF.
 *
   @{ */

#define MAX_I2C_BUSES    2 // SAMB11 has up to 2 I2C address that can be configured as I2C

/** \brief this is the hal_data for ATCA HAL for ASF
 */
typedef struct atcaI2Cmaster
{
    I2C*                     i2c_sercom;
    struct i2c_master_module i2c_master_instance;
    int                      ref_ct;
    // for conveniences during interface release phase
    int bus_index;
} ATCAI2CMaster_t;

/** @} */
#endif /* HAL_SAMB11_I2C_ASF_H_ */