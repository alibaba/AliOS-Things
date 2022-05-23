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

#ifndef __TONE_PARTITION_H__
#define __TONE_PARTITION_H__

#include "esp_err.h"
#include "esp_image_format.h"

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_TONE_HEADER          (0x2053)
#define FLASH_TONE_TAIL            (0xDFAC)
#define FLASH_TONE_MAGIC_WORD      (0xF55F9876)
#define FLASH_TONE_PROJECT_NAME    "ESP_TONE_BIN"

/**
 * @brief The operation handle for tone partition
 */
typedef struct tone_partition_s * tone_partition_handle_t;

/**
 * @brief  The fone bin head
 */
#pragma pack(1)
typedef struct flash_tone_header
{
    uint16_t header_tag;   /*!< File header tag is 0x2053 */
    uint16_t total_num;    /*!< Number of all tones */
    uint32_t format;       /*!< The version of the bin file */
} flash_tone_header_t;
#pragma pack()

/**
 * @brief The file information structure in tone partition
 */
#pragma pack(1)
typedef struct tone_file_info {
    uint8_t file_tag;   /*!< File tag is 0x28 */
    uint8_t song_index; /*!< Song index represents the type of tone  */
    uint8_t file_type;  /*!< The file type of the tone bin, usually the mp3 type. */
    uint8_t song_ver;   /*!< Song version, default is 0 */
    uint32_t song_adr;  /*!< The address of the bin file corresponding to each tone */
    uint32_t song_len;  /*!< The length of current tone   */
    uint32_t RFU[12];   /*!< Default 0  */
    uint32_t info_crc;  /*!< The crc value of current tone */
} tone_file_info_t;
#pragma pack()

typedef enum tone_format {
    TONE_VERSION_0,        // header + file table + files
    TONE_VERSION_1,        // header + desc + file table + files + crc + tail
} tone_format_t;

/**
 * @brief      Initial the tone partition.
 *
 * @param[in]  handle           Pointer to 'tone_partition_handle_t' structure
 * @param[in]  use_delegate     Whether to use esp delegate to read flash
 *
 * @return
 *      - 'tone_partition_handle_t': Success
 *      - NULL: Fail
 */
tone_partition_handle_t tone_partition_init(const char *partition_label, bool use_delegate);

/**
 * @brief      Destroy the tone partition handle.
 *
 * @param[in]  handle   Pointer to 'tone_partition_handle_t' structure
 *
 * @return
 *      - ESP_OK: Success
 *      - others: Failed
 */
esp_err_t tone_partition_deinit(tone_partition_handle_t handle);

/**
 * @brief      Get the 'esp_app_desc_t' structure in 'flash_tone' partition.
 *
 * @param[in]  handle   Pointer to 'tone_partition_handle_t' structure
 * @param[in]  desc     Pointer to 'esp_app_desc_t' structure
 *
 * @return
 *      - ESP_OK: Success
 *      - others: Failed
 */
esp_err_t tone_partition_get_app_desc(tone_partition_handle_t handle, esp_app_desc_t *desc);

/**
 * @brief      Get the 'tone_file_info_t' structure of the given file index.
 *
 * @param[in]  handle   Pointer to 'tone_partition_handle_t' structure
 * @param[in]  index    Index of the file stored in tone partition
 * @param[out] info     File information read from tone partition
 *
 * @return
 *      - ESP_OK: Success
 *      - others: Failed
 */
esp_err_t tone_partition_get_file_info(tone_partition_handle_t handle, uint16_t index, tone_file_info_t *info);

/**
 * @brief      Get the 'tone_file_info_t' structure of the given file index.
 *
 * @param[in]  handle   Pointer to 'tone_partition_handle_t' structure
 * @param[in]  file     File to read
 * @param[in]  offset   Read offset from the beginning of the file.
 * @param[in]  dst      Buffer to read
 * @param[in]  read_len Wanted length
 *
 * @return
 *      - ESP_OK: Success
 *      - others: Failed
 */
esp_err_t tone_partition_file_read(tone_partition_handle_t handle, tone_file_info_t *file, uint32_t offset, char *dst, int read_len);

#ifdef __cplusplus
}
#endif

#endif /* __TONE_PARTITION_H__ */
