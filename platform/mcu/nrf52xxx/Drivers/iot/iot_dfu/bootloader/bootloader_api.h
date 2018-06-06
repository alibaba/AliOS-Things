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
/**@file
 *
 * @defgroup iot_dfu_bootloader_api Bootloader API
 * @ingroup iot_sdk_dfu
 * @{
 *
 * @brief Bootloader module interface.
 */

#ifndef BOOTLOADER_API_H__
#define BOOTLOADER_API_H__

#include "nordic_common.h"
#include "dfu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Function for initializing the Bootloader.
 *
 * @retval  NRF_SUCCESS If bootloader was succesfully initialized, else any error code recieved
 *          when initializing underlying flash storage module.
 */
uint32_t bootloader_init(void);

/**@brief Function for validating application region in flash.
 *
 * @param[in]  app_addr  Address to the region in flash where the application is stored.
 *
 * @retval True  If Application region is valid.
 * @retval False If Application region is not valid.
 */
bool bootloader_app_is_valid(uint32_t app_addr);

/**@brief Function for exiting bootloader and booting into application.
 *
 * @details This function will disable SoftDevice and all interrupts before jumping to application.
 *          The SoftDevice vector table base for interrupt forwarding will be set the application
 *          address.
 *
 * @param[in] app_addr Address to the region where the application is stored.
 */
uint32_t bootloader_app_start(uint32_t app_addr);

/**@brief Function getting state of DFU update in progress.
 *        After a successful firmware transfer to the SWAP area, the system restarts in order to
 *        copy data and check if they are valid.
 *
 * @retval  True   A firmware update is in progress. This indicates that SWAP area
 *                 is filled with new firmware.
 * @retval  False  No firmware update is in progress.
 */
bool bootloader_dfu_update_in_progress(void);

/**@brief Function for continuing the Device Firmware Update.
 *
 * @retval     NRF_SUCCESS If the final stage of SoftDevice update was successful, else an error
 *             indicating reason of failure.
 */
uint32_t bootloader_dfu_update_continue(void);

#ifdef __cplusplus
}
#endif

#endif // BOOTLOADER_API_H__

/**@} */
