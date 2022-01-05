/**
 * \file
 * \brief ATCA Hardware abstraction layer for I2C bit banging.
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

#ifndef HAL_I2C_BITBANG_H_
#define HAL_I2C_BITBANG_H_





#if     BOARD == AT88CK9000
#       include "i2c_bitbang_at88ck9000.h"
#endif

#if    (SAMD21 == 1)
#include "i2c_bitbang_samd21.h"
#endif


/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief These methods define the hardware abstraction layer for
 *        communicating with a CryptoAuth device using I2C bit banging.
   @{ */

/**
 * \brief This enumeration lists flags for I2C read or write addressing.
 */
enum i2c_read_write_flag
{
    I2C_WRITE = (uint8_t)0x00,  //!< write command flag
    I2C_READ  = (uint8_t)0x01   //!< read command flag
};

/**
 * \brief This is the hal_data for ATCA HAL.
 */
typedef struct atcaI2Cmaster
{
    uint32_t pin_sda;
    uint32_t pin_scl;
    int      ref_ct;
    //! for conveniences during interface release phase
    int bus_index;
} ATCAI2CMaster_t;

/** @} */

#endif /* HAL_AT88CK900X_I2C_H_ */