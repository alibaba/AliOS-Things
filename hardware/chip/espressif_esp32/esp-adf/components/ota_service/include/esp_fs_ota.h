/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2020 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#pragma once

#include <bootloader_common.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void *esp_fs_ota_handle_t;

#define ESP_ERR_FS_OTA_BASE        (0x10000)
#define ESP_ERR_FS_OTA_IN_PROGRESS (ESP_ERR_FS_OTA_BASE + 1) /* OTA operation in progress */

/**
 * @brief ESP FS OTA configuration
 */
typedef struct {
    const char *path; /*!< file path */
    const int buffer_size;
} esp_fs_ota_config_t;

/**
 * @brief    Upgrade the firmware from filesystem.
 *
 * @param[in]  ota_config       pointer to esp_fs_ota_config_t structure.
 *
 * @note     This API handles the entire OTA operation, so if this API is being used
 *           then no other APIs from `esp_fs_ota` component should be called.
 *           If more information and control is needed during the FS OTA process,
 *           then one can use `esp_fs_ota_begin` and subsequent APIs. If this API returns
 *           successfully, esp_restart() must be called to boot from the new firmware image.
 *
 * @return
 *    - ESP_OK: OTA data updated, next reboot will use specified partition.
 *    - ESP_FAIL: For generic failure.
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - For other return codes, refer OTA documentation in esp-idf's app_update component.
 */
esp_err_t esp_fs_ota(esp_fs_ota_config_t *ota_config);

/**
 * @brief    Start FS OTA Firmware upgrade
 *
 * This function initializes ESP FS OTA context and open the firmware file.
 * This function must be invoked first. If this function returns successfully, then `esp_fs_ota_perform` should be
 * called to continue with the OTA process and there should be a call to `esp_fs_ota_finish` on
 * completion of OTA operation or on failure in subsequent operations.
 *
 * @param[in]   ota_config       pointer to esp_fs_ota_config_t structure
 * @param[out]  handle           pointer to an allocated data of type `esp_fs_ota_handle_t`
 *                               which will be initialised in this function
 *
 * @return
 *    - ESP_OK: FS OTA Firmware upgrade context initialised and file opened successful
 *    - ESP_FAIL: For generic failure.
 *    - ESP_ERR_INVALID_ARG: Invalid argument (missing/incorrect config, etc.)
 *    - For other return codes, refer documentation in app_update component and esp_http_client
 *      component in esp-idf.
 */
esp_err_t esp_fs_ota_begin(esp_fs_ota_config_t *ota_config, esp_fs_ota_handle_t *handle);

/**
 * @brief    Read image data from file stream and write it to OTA partition
 *
 * This function reads image data from file stream and writes it to OTA partition. This function
 * must be called only if esp_fs_ota_begin() returns successfully.
 * This function must be called in a loop since it returns after every file read operation thus
 * giving you the flexibility to stop OTA operation midway.
 *
 * @param[in]  fs_ota_handle  pointer to esp_fs_ota_handle_t structure
 *
 * @return
 *    - ESP_ERR_FS_OTA_IN_PROGRESS: OTA update is in progress, call this API again to continue.
 *    - ESP_OK: OTA update was successful
 *    - ESP_FAIL: OTA update failed
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 *    - ESP_ERR_NO_MEM: Cannot allocate memory for OTA operation.
 *    - ESP_ERR_FLASH_OP_TIMEOUT or ESP_ERR_FLASH_OP_FAIL: Flash write failed.
 *    - For other return codes, refer OTA documentation in esp-idf's app_update component.
 */
esp_err_t esp_fs_ota_perform(esp_fs_ota_handle_t fs_ota_handle);

/**
 * @brief    Clean-up FS OTA Firmware upgrade and close the file stream.
 *
 * This function closes the file stream and frees the ESP FS OTA context.
 * This function switches the boot partition to the OTA partition containing the
 * new firmware image.
 *
 * @note     If this API returns successfully, esp_restart() must be called to
 *           boot from the new firmware image
 *
 * @param[in]  fs_ota_handle   pointer to esp_fs_ota_handle_t structure
 *
 * @return
 *    - ESP_OK: Clean-up successful
 *    - ESP_ERR_INVALID_STATE
 *    - ESP_ERR_INVALID_ARG: Invalid argument
 *    - ESP_ERR_OTA_VALIDATE_FAILED: Invalid app image
 */
esp_err_t esp_fs_ota_finish(esp_fs_ota_handle_t fs_ota_handle);

/**
 * @brief   Reads app description from image header. The app description provides information
 *          like the "Firmware version" of the image.
 *
 * @note    This API can be called only after esp_fs_ota_begin() and before esp_fs_ota_perform().
 *          Calling this API is not mandatory.
 *
 * @param[in]   fs_ota_handle      pointer to esp_fs_ota_handle_t structure
 * @param[out]  new_app_info       pointer to an allocated esp_app_desc_t structure
 *
 * @return
 *    - ESP_ERR_INVALID_ARG: Invalid arguments
 *    - ESP_FAIL: Failed to read image descriptor
 *    - ESP_OK: Successfully read image descriptor
 */
esp_err_t esp_fs_ota_get_img_desc(esp_fs_ota_handle_t fs_ota_handle, esp_app_desc_t *new_app_info);

/*
* @brief  This function returns OTA image data read so far.
*
* @note   This API should be called only if `esp_fs_ota_perform()` has been called atleast once or
*         if `esp_fs_ota_get_img_desc` has been called before.
*
* @param[in]   fs_ota_handle   pointer to esp_https_ota_handle_t structure
*
* @return
*    - -1    On failure
*    - total bytes read so far
*/
int esp_fs_ota_get_image_len_read(esp_fs_ota_handle_t fs_ota_handle);

#ifdef __cplusplus
}
#endif
