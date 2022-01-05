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

#ifndef _BLUFI_SECURITY_H_
#define _BLUFI_SECURITY_H_

#include "esp_log.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief       BLUFI negotiate data handler
 *
 * @param[in] data          data from phone
 * @param[in] len           length of data from phone
 * @param[in] output_data   data to be sent to phone
 * @param[in] output_len    length of data to be sent to phone
 */
void blufi_dh_negotiate_data_handler(uint8_t *data, int len, uint8_t **output_data, int *output_len, bool *need_free);

/**
 * @brief       BLUFI  encrypt the data after negotiating a share key
 *
 * @param[in] iv8           initial vector(8bit), normally, blufi core will input packet sequence number
 * @param[in] crypt_data    plain text and encrypted data, the encrypt function must support autochthonous encrypt
 * @param[in] crypt_len     length of plain text
 *
 * @return  Nonnegative number is encrypted length, if error, return negative number;
 */
int blufi_aes_encrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);

/**
 * @brief       BLUFI  decrypt the data after negotiating a share key
 *
 * @param[in] iv8           initial vector(8bit), normally, blufi core will input packet sequence number
 * @param[in] crypt_data    encrypted data and plain text, the encrypt function must support autochthonous decrypt
 * @param[in] crypt_len     length of encrypted text
 *
 * @return  Nonnegative number is decrypted length, if error, return negative number;
 */
int blufi_aes_decrypt(uint8_t iv8, uint8_t *crypt_data, int crypt_len);

/**
 * @brief      BLUFI CRC check sum function
 *
 * @param[in] iv8       initial vector(8bit), normally, blufi core will input packet sequence number
 * @param[in] data      data need to checksum
 * @param[in] len       length of data
 *
 * @return  None
 */
uint16_t blufi_crc_checksum(uint8_t iv8, uint8_t *data, int len);

/**
 * @brief      Initialize and allocate the resource for BLUFI security
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t blufi_security_init(void);

/**
 * @brief      Uninitialize and free the resource for BLUFI security
 *
 * @return
 *     - ESP_OK
 *     - ESP_FAIL
 */
esp_err_t blufi_security_deinit(void);

#ifdef __cplusplus
}
#endif

#endif
