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
 * @defgroup iot_dfu_bootloader_image Bootloader images
 * @ingroup iot_sdk_dfu
 * @{
 *
 * @brief Internal Device Firmware Update module interface.
 */

#ifndef APP_DFU_BOOT_H__
#define APP_DFU_BOOT_H__

#include "dfu_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**@brief Function for loading the bootloader settings.
 *
 * @details This function is defined as an interface function. The implementation of this is system
 *          specific and needs to be implemented by a user module.
 *
 * @param[out] p_bootloader_settings Current bootloader settings.
 */
void bootloader_settings_load(bootloader_settings_t * p_bootloader_settings);

/**@brief Function for saving the bootloader settings.
 *
 * @details This function is defined as an interface function. The implementation of this is system
 *          specific and needs to be implemented by a user module.
 * @param[in]  p_bootloader_settings New bootloader settings.
 */
void bootloader_settings_save(bootloader_settings_t * p_bootloader_settings);

/**@brief Function for checking if new bootloader will be installed.
 *
 * @details The function will also compare the copied version against the firmware source,
 *          and mark it as installed if comparisson is a success before returning.
 *
 * @return NRF_SUCCESS if install was successful. NRF_ERROR_NULL if the images differs.
 */
uint32_t dfu_bl_image_validate(void);

/**@brief Function for validating that new application has been correctly installed.
 *
 * @details The function will also compare the copied version against the firmware source,
 *          and mark it as installed if comparisson is a success before returning.
 *
 * @return NRF_SUCCESS if install was successful. NRF_ERROR_NULL if the images differs.
 */
uint32_t dfu_app_image_validate(void);

/**@brief Function for validating that new SoftDevice has been correctly installed.
 *
 * @details The function will also compare the copied version against the firmware source,
 *          and mark it as installed if comparisson is a success before returning.
 *
 * @return NRF_SUCCESS if install was successful. NRF_ERROR_NULL if the images differs.
 */
uint32_t dfu_sd_image_validate(void);

/**@brief Function for swapping existing bootloader with newly received.
 *
 * @note This function will not return any error code but rather reset the chip after.
 *       If an error occurs during copy of bootloader, this will be resumed by the system on
 *       power-on again.
 *
 * @return NRF_SUCCESS on succesfull swapping. For error code please refer to
 *         sd_mbr_command_copy_bl_t.
 */
uint32_t dfu_bl_image_swap(void);

/**@brief Function for swapping existing SoftDevice with newly received.
 *
 * @return NRF_SUCCESS on succesfull swapping. For error code please refer to
 *         sd_mbr_command_copy_sd_t.
 */
uint32_t dfu_sd_image_swap(void);

/**@brief Function for swapping existing application with newly received.
 *
 * @return NRF_SUCCESS on succesfull swapping. For error code please refer to
 *         sd_mbr_command_copy_sd_t.
 */
uint32_t dfu_app_image_swap(void);

#ifdef __cplusplus
}
#endif

#endif // APP_DFU_BOOT_H__

/** @} */
