/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
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

#ifndef __ESP_SPIFFS_H__
#define __ESP_SPIFFS_H__

#include "spiffs/spiffs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** \defgroup Spiffs_APIs Spiffs APIs
  * @brief Spiffs APIs
  *
  * More details about spiffs on https://github.com/pellepl/spiffs
  *
  */

/** @addtogroup Spiffs_APIs
  * @{
  */

struct esp_spiffs_config {
    uint32 phys_size;        /**< physical size of the SPI Flash */
    uint32 phys_addr;        /**< physical offset in spi flash used for spiffs, must be on block boundary */
    uint32 phys_erase_block; /**< physical size when erasing a block */

    uint32 log_block_size;   /**< logical size of a block, must be on physical block size boundary and must never be less than a physical block */
    uint32 log_page_size;    /**< logical size of a page, at least log_block_size/8  */

    uint32 fd_buf_size;      /**< file descriptor memory area size */
    uint32 cache_buf_size;   /**< cache buffer size */
};

/**
  * @brief  Initialize spiffs
  *
  * @param  struct esp_spiffs_config *config : ESP8266 spiffs configuration
  *
  * @return 0         : succeed
  * @return otherwise : fail
  */
sint32 esp_spiffs_init(struct esp_spiffs_config *config);

/**
  * @brief  Deinitialize spiffs
  *
  * @param  uint8 format : 0, only deinit; otherwise, deinit spiffs and format.
  *
  * @return null
  */
void esp_spiffs_deinit(uint8 format);

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __ESP_SPIFFS_H__ */
