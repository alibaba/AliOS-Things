/**
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 * 
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 */
#ifndef IOT_DFU_H__
#define IOT_DFU_H__

#include <stdint.h>
#include "dfu_types.h"
#include "iot_file.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @file iot_dfu.h
 *
 * @defgroup iot_dfu_app Application IoT DFU
 * @ingroup iot_sdk_dfu
 * @{
 * @brief API for upgrading firmware in user application.
 *
 */

/**@brief IoT DFU module's events. */
typedef enum
{
    IOT_DFU_WRITE_COMPLETE,                  /**< Write operation finishes with success. */
    IOT_DFU_ERROR                            /**< DFU event indicating that an error occurs during update. */
} iot_dfu_evt_t;


/**@brief Description of single block of firmware. */
typedef struct
{
    uint32_t size;                           /**< Size of firmware block. */
    uint16_t crc;                            /**< Checksum of firmware block. Set to 0 if no checksum checking is needed. */
} iot_dfu_firmware_block_t;


/**@brief Description of the new firmware that has been written into flash memory. */
typedef struct
{
    iot_dfu_firmware_block_t application;    /**< Description of Application block in firmware image. */
    iot_dfu_firmware_block_t softdevice;     /**< Description of SoftDevice block in firmware image. */
    iot_dfu_firmware_block_t bootloader;     /**< Description of Bootloader block in firmware image. */
} iot_dfu_firmware_desc_t;


/**@brief IOT DFU module's application callback.
 *
 * @param[in] result Result of the received event.
 * @param[in] event  Event from IoT DFU module.
 *
 * @retval None.
 */
typedef void (*iot_dfu_callback_t)(uint32_t result, iot_dfu_evt_t evt);


 /**@brief Function for initializing IOT DFU module.
 *
 * @note The iot_dfu does not hold any state. The application must ensure that only one firmware image is written at once.
 *       Remember to register pstorage event handler to allow IoT File module to work.
 *
 *       err_code = softdevice_sys_evt_handler_set(sys_evt_dispatch);
 *       APP_ERROR_CHECK(err_code);
 *
 *       static void sys_evt_dispatch(uint32_t sys_evt)
 *       {
 *           pstorage_sys_event_handler(sys_evt);
 *       }
 *
 *       err_code = pstorage_init();
 *       APP_ERROR_CHECK(err_code);
 *
 * @param[in] cb  Callback to be called upon IoT file write success and failure, or any IoT DFU error event.
 *
 * @retval NRF_SUCCESS on success, an error_code otherwise.
 */
uint32_t iot_dfu_init(iot_dfu_callback_t cb);


/**@brief Function for creating a PStorage IoT File reference for flash operations and setting
 *        start address of PStorage File.
 *
 * @note Application should ensure that correct word sized alignment is given at the beginning and between
 *       Application image and Softdevice image while updating them at the same time.
 *
 * @param[out] pp_file  Reference to PStorage File.
 *
 * @retval NRF_SUCCESS on success, an error_code otherwise.
 */
uint32_t iot_dfu_file_create(iot_file_t ** pp_file);


/**@brief Function for validating the CRC of the received image.
 *
 * @param[in]  p_firmware_desc  Description of the firmware. Must have been set to zeros on all unused fields.
 *
 * @retval NRF_SUCCESS on success, an error_code otherwise.
 */
uint32_t iot_dfu_firmware_validate(iot_dfu_firmware_desc_t * p_firmware_desc);


/**@brief Function for rebooting device and trigger bootloader to swap the new firmware.
 *
 * @note Currently the following combinations are allowed:
 *       - Application
 *       - Bootloader
 *       - SoftDevice with Application
 *       - SoftDevice with Bootloader
 *
 * @param[in]  p_firmware_desc  Description of the firmware. Must have been set to zeros on all unused fields.
 *
 * @return    NRF_SUCCESS on success, an error_code otherwise.
 */
uint32_t iot_dfu_firmware_apply(iot_dfu_firmware_desc_t * p_firmware_desc);

/**@} */

#ifdef __cplusplus
}
#endif

#endif // IOT_DFU_H__
