/**
 * \file
 * \brief Unity tests for the cryptoauthlib AES GCM functions.
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
#ifndef ATCA_BASIC_AES_GCM_H_
#define ATCA_BASIC_AES_GCM_H_

#include "cryptoauthlib.h"

/** \ingroup atcab_
 * @{
 */

extern const char* atca_basic_aes_gcm_version;

/** Context structure for AES GCM operations.
 */
typedef struct atca_aes_gcm_ctx
{
    uint16_t key_id;                           //!< Key location. Can either be a slot number or ATCA_TEMPKEY_KEYID for TempKey.
    uint8_t  key_block;                        //!< Index of the 16-byte block to use within the key location for the actual key.
    uint8_t  cb[AES_DATA_SIZE];                //!< Counter block, comprises of nonce + count value (16 bytes).
    uint32_t data_size;                        //!< Size of the data being encrypted/decrypted in bytes.
    uint32_t aad_size;                         //!< Size of the additional authenticated data in bytes.
    uint8_t  h[AES_DATA_SIZE];                 //!< Subkey for ghash functions in GCM.
    uint8_t  j0[AES_DATA_SIZE];                //!< Precounter block generated from IV.
    uint8_t  y[AES_DATA_SIZE];                 //!< Current GHASH output
    uint8_t  partial_aad[AES_DATA_SIZE];       //!< Partial blocks of data waiting to be processed
    uint32_t partial_aad_size;                 //!< Amount of data in the partial block buffer
    uint8_t  enc_cb[AES_DATA_SIZE];            //!< Last encrypted counter block
    uint8_t  ciphertext_block[AES_DATA_SIZE];  //!< Last ciphertext block
} atca_aes_gcm_ctx_t;

ATCA_STATUS atcab_aes_gcm_init(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, const uint8_t* iv, size_t iv_size);
ATCA_STATUS atcab_aes_gcm_init_rand(atca_aes_gcm_ctx_t* ctx, uint16_t key_id, uint8_t key_block, size_t rand_size,
                                    const uint8_t* free_field, size_t free_field_size, uint8_t* iv);
ATCA_STATUS atcab_aes_gcm_aad_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* aad, uint32_t aad_size);
ATCA_STATUS atcab_aes_gcm_encrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* plaintext, uint32_t plaintext_size, uint8_t* ciphertext);
ATCA_STATUS atcab_aes_gcm_encrypt_finish(atca_aes_gcm_ctx_t* ctx, uint8_t* tag, size_t tag_size);
ATCA_STATUS atcab_aes_gcm_decrypt_update(atca_aes_gcm_ctx_t* ctx, const uint8_t* ciphertext, uint32_t ciphertext_size, uint8_t* plaintext);
ATCA_STATUS atcab_aes_gcm_decrypt_finish(atca_aes_gcm_ctx_t* ctx, const uint8_t* tag, size_t tag_size, bool* is_verified);

/** @} */

#endif