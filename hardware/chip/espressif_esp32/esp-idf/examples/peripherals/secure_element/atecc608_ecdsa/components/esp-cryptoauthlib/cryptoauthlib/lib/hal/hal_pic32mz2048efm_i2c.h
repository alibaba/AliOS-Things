/**
 * \file
 * \brief ATCA Hardware abstraction layer for PIC32MZ2048
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

#ifndef HAL_PIC32MZ2048EFM_I2C_H_
#define HAL_PIC32MZ2048EFM_I2C_H_


/**
 * \defgroup hal_ Hardware abstraction layer (hal_)
 *
 * \brief
 * These methods define the hardware abstraction layer for communicating with a CryptoAuth device
 * using I2C driver of Harmony.
 *
   @{ */

// Wherever needed to indicate that it is a Harmony driver hal implementation
#define HARMONY_I2C_DRIVER      1

// Clock Constants
#define MAX_I2C_BUSES           3   //

/**
 * \brief this is the hal_data for ATCA HAL
 */
typedef struct atcaI2Cmaster
{
    int id;
    int ref_ct;
    int bus_index;
} ATCAI2CMaster_t;

typedef struct
{
    volatile uintptr_t i2cDriverInstance;
    uint32_t           i2cDriverInstanceIndex;
    void *             i2cDriverInit;
} DRV_I2C_Object;
/** @} */

#endif  /* HAL_PIC32MX695F512H_I2C_ASF_H_ */