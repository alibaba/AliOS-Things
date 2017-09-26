/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#ifndef __TFS_ID2_H
#define __TFS_ID2_H

#include <stdint.h>

/* ID2 key started with letter Y */
#define TFS_ID2_LEN    17
/* for 3DES sign len is 16 + 8, for rsa sign len is 128 */
#define TFS_ID2_SIGN_SIZE   (128)

/* AES padding type*/
#define TFS_AES_PKCS7_PADDING 0X01
#define TFS_AES_ZERO_PADDING  0X02
#define TFS_AES_NO_PADDING    0X03

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief get ID2 informaiton
 *
 * @param[out] id2: the mem must >= TFS_ID2_LEN.
 * @param[inout] len: TFS_ID2_LEN.
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int tfs_get_ID2(uint8_t *id2, uint32_t *len);


/**
 * @brief sign data with ID2 key
 *
 * @param[in] in: input data.
 * @param[in] in_len: the length of intput data, which must <= 4096 bytes.
 * @param[out] sign: signature for input data.
 * @param[inout] sign_len: the length of signature, which should be set to sign buffer's len.
 * @return: 0~OK, other~ERROR
 * @note None.
 */
int tfs_id2_sign(const uint8_t *in, uint32_t in_len,
                 uint8_t *sign, uint32_t *sign_len);

/**
 * @brief decrypt cipher text with ID2 key
 *
 * @param[in] in: input data.
 * @param[in] in_len: the length of intput data, which must <= 4096 bytes.
 * @param[out] out: decrypted data.
 * @param[inout] out_len: the length of decrypted data, which should be set to out buffer's len.
 * @return: 0~OK, other~ERROR
 * @note None.
 */
int tfs_id2_decrypt(const uint8_t *in, uint32_t in_len,
                    uint8_t *out, uint32_t *out_len);

/**
 * @brief get auth code
 *
 * @param[out] auth_code: mode~sid~seed~signature, which should be more than 256 bytes.
 * @param[out] len: auth code length
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int tfs_get_auth_code(uint8_t *auth_code, uint32_t *len);

/**
 * @brief get activate status
 *
 * @return: 0~activated, other~not activated or read activate info fail.
 * @note None.
 */
int tfs_is_device_activated(void);

/**
 * @brief activate device
 *
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int tfs_activate_device(void);

/**
 * @brief get auth code with timestamp
 *
 * @param[in]  timestamp: timestamp from caller, the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 * @param[out] auth_code: model~sid~timestamp~signature, which should be more than 256 bytes.
 * @param[out] auth_len: auth code length.
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int tfs_id2_get_auth_code(uint64_t timestamp, uint8_t *auth_code,
                          uint32_t *auth_len);

/**
 * @brief get auth code with digest and timestamp
 *
 * @param[in]  timestamp: timestamp from caller, the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC).
 * @param[in]  digest: data digest
 * @param[in]  digest_len: data digest length, which must <= 256 bytes
 * @param[out] auth_code: model~timestamp~signature, which should be more than 256 bytes.
 * @param[out] auth_len: auth code length.
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int tfs_id2_get_digest_auth_code(uint64_t timestamp, uint8_t *digest,
                                 uint32_t digest_len, uint8_t *auth_code, uint32_t *auth_len);

/**
 * @brief aes128 cbc encryption
 *
 * @param[in] key: key for aes128, key length should be 16 bytes.
 * @param[in] iv: initial vector for cbc, iv length should be 16 bytes.
 * @param[in] input_len:input data length, when padding type is nopadding, it should be multiple of 16, which must <= 2MB.
 * @param[in] input: input data for encryption.
 * @param[out] out_len:output data length.
 * @param[out] output: output data for encryption .
 * @param[in] padding: padding type, support pkcs7 padding, zero padding and no padding now.
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int32_t tfs_aes128_cbc_enc(const uint8_t *key,
                           const uint8_t *iv,
                           int32_t input_len,
                           const uint8_t *input,
                           int32_t *output_len,
                           uint8_t *output,
                           uint8_t padding);

/**
 * @brief aes128 cbc decryption
 *
 * @param[in] key: key for aes128, key length should be 16 bytes.
 * @param[in] iv: initial vector for cbc, iv length should be 16 bytes.
 * @param[in] input_len:input data length, it should be multiple of 16, which must <= 2MB.
 * @param[in] input: input data for decryption.
 * @param[out] out_len:output data length.
 * @param[out] output: output data for decryption .
 * @param[in] padding: padding type, support pkcs7 padding, zero padding and no padding now.
 * @return: 0~OK, other~ERROR.
 * @note None.
 */
int32_t tfs_aes128_cbc_dec(const uint8_t *key,
                           const uint8_t *iv,
                           int32_t input_len,
                           const uint8_t *input,
                           int32_t *output_len,
                           uint8_t *output,
                           uint8_t padding);

#ifdef __cplusplus
}
#endif

#endif
