/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "error_code.h"
#include "config.h"
#include "id2_client.h"

#define MAX_DEC_BUF 0x10

extern void irot_pal_log(const char* fmt, ...);
extern irot_result_t basex64_encode(const uint8_t* in, uint32_t in_len, uint8_t* out, uint32_t* out_len);
#define id2_log_debug irot_pal_log

static void test_id2_client_get_id(void)
{
    irot_result_t ret;
    uint32_t len = ID2_ID_LEN;
    uint8_t id2[ID2_ID_LEN + 1] = {0};

    ret = id2_client_get_id(id2, &len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_get_id return %d error!\n", ret);
        return;
    }
    if (len != ID2_ID_LEN)
    {
        id2_log_debug("id2 length error!\n");
        return;
    }
    id2_log_debug("ID: %s\n\n", id2);
}

static void test_id2_client_get_challenge_auth_code(void)
{
    irot_result_t ret;
    uint32_t len =  AUTH_CODE_BUF_LEN;
    uint8_t auth_code[AUTH_CODE_BUF_LEN] = {0};

    const char* extra = "abcd1234";
    const char* challenge = "55B83408399FA660F05C82E4F25333DC";

    ret = id2_client_get_challenge_auth_code(challenge, NULL, 0, auth_code, &len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_get_challenge_auth_code return %d error!\n", ret);
    }
    id2_log_debug("AUTHCODE:|%d|%s|%s\n", len, "", auth_code);

    len = AUTH_CODE_BUF_LEN;
    memset(auth_code, 0x00, AUTH_CODE_BUF_LEN);
    ret = id2_client_get_challenge_auth_code(challenge, (const uint8_t*)extra, strlen(extra), auth_code, &len);
    if (ret != 0)
    {
        id2_log_debug("id2_client_get_challenge_auth_code return %d error!\n", ret);
    }
    id2_log_debug("AUTHCODE:|%d|%s|%s\n", len, extra, auth_code);
}

static void test_id2_client_get_timestamp_auth_code(void)
{
    irot_result_t ret;
    uint32_t len = AUTH_CODE_BUF_LEN;
    uint8_t auth_code[AUTH_CODE_BUF_LEN] = {0};

    // just an example
    const char* extra = "abcd1234";
    const char* timestamp_str = "1512022279204";

    // just an example in, you must get the real epoch time, in ms!!!
    ret = id2_client_get_timestamp_auth_code(timestamp_str, NULL, 0, auth_code, &len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_get_timestamp_auth_code return %d error!\n", ret);
        return;
    }
    id2_log_debug("AUTHCODE:|%d|%s|%s\n", len, "", auth_code);

    /* test with extra data */
    len = AUTH_CODE_BUF_LEN;
    memset(auth_code, 0, AUTH_CODE_BUF_LEN);
    ret = id2_client_get_timestamp_auth_code(timestamp_str, (uint8_t*)extra, strlen(extra), auth_code, &len);
    if (ret != 0)
    {
        id2_log_debug("id2_client_get_timestamp_auth_code return %d error!\n", ret);
        return;
    }
    id2_log_debug("AUTHCODE:|%d|%s|%s\n", len, extra, auth_code);
}

static void test_id2_client_decrypt(void)
{
    irot_result_t ret;
    uint8_t buf[MAX_DEC_BUF] = {0};
    uint32_t dec_len;

    const char* plaintext = "1234";
	const uint8_t ciphertext[] =
	{
#if (ID2_OTP_SUPPORTED)

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
     0xDE, 0xCF, 0xB0, 0x4D, 0xE5, 0x25, 0x63, 0x1E, //CONST_3DES_112
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
     0x1C, 0xC5, 0x65, 0xA1, 0x91, 0xCF, 0xCE, 0xA5,   0x9F, 0x99, 0x7D, 0xC9, 0x19, 0x5C, 0x15, 0x4D, //CONST_AES_128
#endif

#else  /* ID2_OTP_SUPPORTED */

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
	0xA0, 0xFA, 0x13, 0x44, 0x96, 0x0D, 0x4E, 0xBA, //CONST_3DES_112
	//0x84, 0xAC, 0xA5, 0x93, 0x71, 0xAE, 0x5F, 0x36, //CONST_3DES_168
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
	0xEC, 0xE1, 0x8C, 0xE9, 0xB9, 0x61, 0xAE, 0xD7,   0x50, 0x02, 0xA4, 0x8E, 0xB9, 0x95, 0x5E, 0x44, //CONST_AES_128
	//0x8D, 0x07, 0xF6, 0x38, 0x52, 0x28, 0x25, 0xBD,   0x28, 0xD3, 0x1D, 0x8D, 0xCA, 0x21, 0xB6, 0xE9, //CONST_AES_192
	//0x72, 0x8D, 0x1F, 0xDB, 0x02, 0x3B, 0x7D, 0x37,   0xAA, 0x47, 0xDA, 0x0F, 0x19, 0x6A, 0x37, 0x13, //CONST_AES_256
#else
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
#endif

#endif /* ID2_OTP_SUPPORTED */
	};

    dec_len = sizeof(buf);
    ret = id2_client_decrypt(ciphertext, sizeof(ciphertext), buf, &dec_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_decrypt return %d error!\n", ret);
        id2_log_debug("1. please get the id2 server SDK.\n");
        id2_log_debug("2. connect to id2 server, verify authcode and get the encrypted data.\n");
        id2_log_debug("3. decrypt the encrypted data use id2_client_decrypt.\n");
        return;
    }

    if (dec_len != strlen(plaintext))
    {
        id2_log_debug("decrypt output length error %d!\n", dec_len);
        return;
    }

    if (memcmp((void*)plaintext, buf, dec_len) != 0)
    {
        id2_log_debug("dec_out != plaintext, check your ciphertext!\n");
    }
    else
    {
        id2_log_debug("dec_out = plaintext = %s\n", plaintext);
    }
}

#if (ID2_OTP_SUPPORTED)
int test_id2_client_get_otp_auth_code(int type)
{
    irot_result_t ret;
    uint8_t auth_code[256];
    uint8_t temp_data[256];
    uint32_t auth_len = 256;
    uint32_t temp_len = 256;
    char *token = NULL;
    char *otp_auth = NULL;

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (type == 0x01) {
        token = "i11iQLFQgzT2BRTWTVQ9B0BE0549A601";
        otp_auth = "AAABACQOSl8KZrYD5hPmp3Ln4ssZ4KHv9vIDYdq3264F3kLH6Ma6B6L/RmkocBtMR+0KRiSRRDhBIuvP0RNFxNtMO3sEGDJO7knIDlWKQ7wQoNR/xFW3Tpd2W8WQTa+YrMYtFqAne+PU9hZPmL7gH69HmrY=";
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (type == 0x04) {
        token = "i14ejGyODBUwsjKTWri789E01AAD601E";
        otp_auth = "AAABAGtpp3AyqRqSbOTyqBeM4G2SHUqk7eo35w5CrwAUX032NTAh8KIhX1ZenldVnU9fFkPeQTyl5CJAPF90wZzKbLMjsY0t+9TTMUuunbErnsvH19aIS+7JnB8FP/Dy5OEDxq84kz6fNm0j";
    }
#endif
    else
    {
        id2_log_debug("invalid type: %d\n", type);
        return -1;
    }

    ret = id2_client_get_otp_auth_code(
              (uint8_t *)token, strlen(token), auth_code, &auth_len);
    if (ret != IROT_SUCCESS) {
        id2_log_debug("get otp auth code fail, %d\n", ret);
        return -1;
    } else {
        memset(temp_data, 0, temp_len);
        basex64_encode(auth_code, auth_len, temp_data, &temp_len);

        if (temp_len != strlen(otp_auth)) {
            id2_log_debug("auth code len not match\n");
            return -1;
        }

        if (memcmp(otp_auth, temp_data, temp_len)) {
            id2_log_debug("otp auth code is not correct\n");
            return -1;
        }
    }

    return 0;
}

int test_id2_client_load_otp_data(int type)
{
    irot_result_t ret;
    uint32_t len = 0;
    uint8_t otp_data[256];

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    if (type != 0x01 && type != 0x02) {
        id2_log_debug("invalid type: %d\n", type);
        return -1;
    }

    if (type == 0x01) {
	const uint8_t tst_data[] = {
                    0x01, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
                    0x31, 0x41, 0x45, 0x35, 0x45, 0x41, 0x36, 0x31,
                    0x39, 0x30, 0x42, 0x37, 0x38, 0x35, 0x42, 0x30,
                    0x30, 0xcc, 0x38, 0xa3, 0x03, 0x99, 0xe0, 0xfe,
                    0x73, 0xb8, 0x57, 0xb8, 0xeb, 0x08, 0x9d, 0x9f,
                    0x33, 0x34, 0x6b, 0xce, 0x0b, 0x8e, 0xed, 0x34,
                    0xda, 0x10, 0xf6, 0xa8, 0xfa, 0xbb, 0x84, 0x44,
                    0x94, 0x82, 0x59, 0x6a, 0x9e, 0x2e, 0x5a, 0x5a,
                    0xa8, 0x55, 0x13, 0xa4, 0x7b, 0xad, 0xde, 0xc4,
                    0xdc, 0x90, 0x1e, 0x3b, 0x10, 0x87, 0x36, 0xdb,
                    0x75, 0x1b, 0x71, 0xda, 0xce, 0x12, 0x9b, 0x66, 0xc3,
	        };

        len = sizeof(tst_data);
        memcpy(otp_data, tst_data, len);
    }
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    if (type != 0x04 && type != 0x05) {
        id2_log_debug("invalid type: %d\n", type);
        return -1;
    }

    if (type == 0x04) {
	const uint8_t tst_data[] = {
                    0x04, 0x30, 0x30, 0x41, 0x41, 0x41, 0x41, 0x41,
                    0x41, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x34,
                    0x38, 0x42, 0x36, 0x39, 0x43, 0x44, 0x32, 0x30,
                    0x30, 0x4b, 0xcd, 0x91, 0x45, 0x7a, 0xe9, 0xb0,
                    0x67, 0xda, 0x42, 0xd4, 0x44, 0x54, 0x76, 0x06,
                    0xf0, 0xad, 0x6a, 0x88, 0xb4, 0xfa, 0x37, 0x83,
                    0x3d, 0x49, 0x4d, 0x87, 0x69, 0xae, 0x17, 0xcf,
                    0x0a, 0xc4, 0x62, 0xd5, 0x19, 0x67, 0xaf, 0x52,
                    0x49, 0x60, 0xa1, 0x86, 0x44, 0x14, 0xbb, 0x67,
                    0xf8, 0xdd, 0x14, 0xd0, 0x74, 0x1e, 0x00, 0x19, 0xe5,
	        };

        len = sizeof(tst_data);
        memcpy(otp_data, tst_data, len);
    }
#endif

    ret = id2_client_load_otp_data(otp_data, len);
    if (ret != IROT_SUCCESS) {
        id2_log_debug("id2 load otp data fail, %d\n", ret);
        return -1;
    }

    return 0;
}
#endif

void test_id2_client_get_secret(void)
{
	irot_result_t ret;
	const char* seed = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	uint8_t secret[64];
	uint32_t out_len;

	out_len = sizeof(secret);
	ret = id2_client_get_secret(seed, secret, &out_len);
	if (ret != IROT_SUCCESS)
	{
		id2_log_debug("id2_client_get_secret error: %d\n", ret);
	}
    else
    {
		id2_log_debug("id2_client_get_secret success\n");
    }
}

#if (ID2_SET_ID_KEY_SUPPORTED)
int test_id2_client_set_id_and_key(void)
{
	irot_result_t ret;
	char* id2 = "0102030405060708090A0B0C";
	const char* keydata = "0102030405060708090A0B0C0D0E0F101112131415161718";
	int key_type;

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
	key_type = 1;
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
	key_type = 2;
#else
#endif

	ret = id2_client_set_id2_and_key(id2, key_type, keydata);
	if (ret != IROT_SUCCESS)
	{
		id2_log_debug("id2_client_set_id2_and_key return %d error!\n", ret);
	}
	return 0;
}
#endif


/* you must call it in your code to run the test! */
void id2_client_test(void)
{
    irot_result_t ret;
    uint32_t version;
    int test_count = 1;
    int i;
#if (ID2_OTP_SUPPORTED)
    int key_type = 0x00;
    bool is_prov = false;
#endif

    //init: you must call init first before other function.
    ret = id2_client_init();
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_init error return %d error!\n", ret);
        return;
    }
    //get version
    ret = id2_client_get_version(&version);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_get_version error return %d\n", ret);
        return;
    }
    id2_log_debug("id2_client_version: 0x%08X\n", version);

#if (ID2_OTP_SUPPORTED)
    id2_log_debug("\n");
    id2_log_debug("==========================> id2 client otp test start.\n");

#if (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_AES)
    key_type = 0x01;
#elif (ID2_CRYPTO_TYPE_CONFIG == ID2_CRYPTO_TYPE_3DES)
    key_type = 0x04;
#endif

    id2_log_debug("test: id2_client_get_prov_stat\n");
    ret = id2_client_get_prov_stat(&is_prov);
    if (ret != IROT_SUCCESS) {
        id2_log_debug("id2 get prov stat fail, %d\n", ret);
        return;
    }

    if (is_prov) {
        id2_log_debug("id2 has been provisioned.\n");
    } else {
        id2_log_debug("\n");
        id2_log_debug("test: id2_client_get_otp_auth_code - key_type: 0x%02x\n", key_type);
        ret = test_id2_client_get_otp_auth_code(key_type);
        if (ret != 0) {
            id2_log_debug("id2 client get otp auth code test fail\n");
            return;
        }

        id2_log_debug("\n");
        id2_log_debug("test: id2_client_load_otp_data - key_type: 0x%02x\n", key_type);
        ret = test_id2_client_load_otp_data(key_type);
        if (ret != 0) {
            id2_log_debug("id2 client load otp data test fail\n");
            return;
        }
    }
    id2_log_debug("<========================== id2 client otp test end.\n\n");
#endif

    //test
    for (i = 0; i < test_count; ++i)
    {
        id2_log_debug("test: id2_client_get_id\n");
        test_id2_client_get_id();

        id2_log_debug("test: id2_client_get_challenge_auth_code\n");
        test_id2_client_get_challenge_auth_code();

        id2_log_debug("test: id2_client_get_timestamp_auth_code\n");
        test_id2_client_get_timestamp_auth_code();

        id2_log_debug("test: id2_client_decrypt\n");
        test_id2_client_decrypt();

        id2_log_debug("test: id2_client_get_secret\n");
		test_id2_client_get_secret();
    }

	id2_client_cleanup();
    id2_log_debug("<========================== id2 client test end.\n\n");
}
