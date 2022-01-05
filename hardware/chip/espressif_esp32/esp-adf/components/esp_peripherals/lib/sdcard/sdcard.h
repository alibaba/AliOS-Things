/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2018 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
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

#ifndef _ESP_SDCARD_H_
#define _ESP_SDCARD_H_

#include "audio_error.h"
#include "periph_sdcard.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief      { function_description }
 *
 * @param[in]  gpio                 The gpio
 * @param[in]  detect_intr_handler  The detect intr handler
 * @param      isr_context          The isr context
 *
 * @return     { description_of_the_return_value }
 */
esp_err_t sdcard_init(int gpio, void (*detect_intr_handler)(void *), void *isr_context);

/**
  * @brief  mount sdcard to FAT filesystem
  *
  * @param  base_path path where partition should be registered (e.g. "/sdcard")
  *
  * @return
  *      - ESP_OK on success
  *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
  *      - ESP_ERR_NO_MEM if memory can not be allocated
  *      - ESP_FAIL if partition can not be mounted
  *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
  */
esp_err_t sdcard_mount(const char* base_path, periph_sdcard_mode_t mode);

/**
 * @brief Unmount FAT filesystem and release resources acquired using esp_vfs_fat_sdmmc_mount
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if sd_card_mount hasn't been called
 */
esp_err_t sdcard_unmount(void);

/**
 * @brief remove the sdcard device GPIO interruption in Audio board
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_FAIL destory sdcard gpio handle failed
 */
esp_err_t sdcard_destroy(void);

/**
 * @brief get the status of sdcard is insert or not
 *
 * @return
 *      - true sdcard is insert
 *      - false sdcard is unplug
 */
bool sdcard_is_exist();

/**
 * @brief      Read value of CARD DETECT Pin
 *
 * @return     value of CARD DETECT PIN
 */
int sdcard_read_detect_pin(void);

#ifdef __cplusplus
}
#endif

#endif
