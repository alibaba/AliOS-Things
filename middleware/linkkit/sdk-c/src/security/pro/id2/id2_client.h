/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */



#ifndef __ID2_CLIENT_H__
#define __ID2_CLIENT_H__

#include <stdint.h>
#include <stdbool.h>
#include "error_code.h"

#ifdef __cplusplus
extern "C"
#endif

#define ID2_ID_LEN                          24
#define ID2_MAX_SERVER_RANDOM_LEN           32
#define ID2_MAX_DEVICE_RANDOM_LEN           16
#define ID2_MAX_EXTRA_LEN                   512
#define ID2_MAX_CRYPTO_LEN                  4096
#define AUTH_CODE_BUF_LEN                   256


/**
 * @brief initialize the id2 client resource, must be called first before other ID2 client API.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_init(void);

/**
 * @brief get the id2-client-sdk version number
 * @param[out] pversion     the version number for output
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_version(uint32_t* pversion);

/**
 * @brief get ID2 information
 *
 * @param[out] id   the ID2 buffer, buffer size must >= ID2_ID_LEN.
 * @param[inout]   len input with the ID2 buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_id(uint8_t* id, uint32_t* len);

/**
 * @brief get the authentication code with the challenge mode.
 *
 * @param[in]  server_random     random data from ID2 server.
 * @param[in]  extra             extra data, optional data, no more than 512 bytes.
 * @param[in]  extra_len         length of extra data.
 * @param[out] auth_code         the auth code output buffer.
 * @param[inout] auth_code_len   input with the output buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_challenge_auth_code(const char* server_random, const uint8_t* extra, uint32_t extra_len, uint8_t* auth_code, uint32_t* auth_code_len);

/**
 * @brief get the authentication code with timestamp mode.
 *
 * @param[in]  timestamp         the number of milliseconds since the Epoch, 1970-01-01 00:00:00 +0000 (UTC)
 * @param[in]  extra             extra data, optional data, no more than 512 bytes.
 * @param[in]  extra_len         length of extra data.
 * @param[out] auth_code         the auth code output buffer.
 * @param[inout] auth_code_len   input with the output buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_timestamp_auth_code(const char* timestamp, const uint8_t* extra, uint32_t extra_len, uint8_t* auth_code, uint32_t* auth_code_len);

/**
 * @brief decrypt the input data with ID2 key.
 *
 * @param[in]  in               input data.
 * @param[in]  in_len           lenth of the input data, which must <= 4096 bytes.
 * @param[out] out              output buffer for decrypt data.
 * @param[inout] out_len        input with the output buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_decrypt(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len);


/**
 * @brief get the challenge form device.
 *
 * @param[out] device_random_buf        output buffer for device challenge.
 * @param[inout] device_random_len      input with the output buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_device_challenge(uint8_t* device_random_buf, uint32_t* device_random_len);

/**
 * @brief   verify the auth code from server.
 *
 * @param[in] server_auth_code       auth code of server.
 * @param[in] server_auth_code_len   auth code length.
 * @param[in] device_random          device challenge, may be NULL if the get_device_challenge has been called.
 * @param[in] device_random_len      the length of device challenge, must set to 0 if device_random is null.
 * @param[in] server_extra           extra data of server.
 * @param[in] server_extra_len       extra data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_verify_server(const uint8_t* server_auth_code, uint32_t server_auth_code_len, const uint8_t* device_random, uint32_t device_random_len, const uint8_t* server_extra, uint32_t server_extra_len);


/* ================================ ID2 OTP Functions ===================================== */

/**
 * @brief get the device id2 provisiong status.
 * @param[out] is_prov   the provisioning status.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_prov_stat(bool *is_prov);

/**
 * @brief get the otp authentication code.
 *
 * @param[in]  token             the provisioning token buffer.
 * @param[in]  token_len         length of provisioning token.
 * @param[out] auth_code         the auth code output buffer.
 * @param[inout] len   input with the output buffer size, ouput the real data length.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_otp_auth_code(const uint8_t* token, uint32_t token_len,
                                           uint8_t* auth_code, uint32_t* len);
/**
 * @brief load the id2 otp data into device.
 *
 * @param[in]  otp_data    the otp data, which is got from id2 server.
 * @param[in]  len         length of otp data.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_load_otp_data(const uint8_t *otp_data, uint32_t len);


/**
 * @brief derive the id2 secret data
 *
 * @param[in]       seed	
 * @param[out]      secret
 * @param[inout]    len
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_get_secret(const char*seed, uint8_t* secret, uint32_t* len);


/**
 * @brief set ID2 and key only for debug.
 *
 * @param[in] id2:			the id2 vlaue, (24 character)
 * @param[in] key_type:		3DES = 0x01, AES = 0x02
 * @param[in] key:			the key value
 *
 * for example: id2_client_set_id2_and_key("0102030405060708090A0B0C", 0x01, "0102030405060708090A0B0C0D0E0F101112131415161718");
 *
 * @return @see irot_result_t
 */

irot_result_t id2_client_set_id2_and_key(const char* id2, int key_type, const char* key);


/**
 * @brief resource release.
 *
 * @return @see irot_result_t
 */
irot_result_t id2_client_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif
