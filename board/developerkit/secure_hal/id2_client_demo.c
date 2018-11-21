/*
 * Copyright (C) 2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include "error_code.h"
#include "id2_client.h"

#define MAX_DEC_BUF 0x10

extern void irot_pal_log(const char* fmt, ...);
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
		0xA0, 0xFA, 0x13, 0x44, 0x96, 0x0D, 0x4E, 0xBA, //CONST_3DES_112
		//0x84, 0xAC, 0xA5, 0x93, 0x71, 0xAE, 0x5F, 0x36, //CONST_3DES_168
		//0xEC, 0xE1, 0x8C, 0xE9, 0xB9, 0x61, 0xAE, 0xD7,   0x50, 0x02, 0xA4, 0x8E, 0xB9, 0x95, 0x5E, 0x44, //CONST_AES_128
		//0x8D, 0x07, 0xF6, 0x38, 0x52, 0x28, 0x25, 0xBD,   0x28, 0xD3, 0x1D, 0x8D, 0xCA, 0x21, 0xB6, 0xE9, //CONST_AES_192
		//0x72, 0x8D, 0x1F, 0xDB, 0x02, 0x3B, 0x7D, 0x37,   0xAA, 0x47, 0xDA, 0x0F, 0x19, 0x6A, 0x37, 0x13, //CONST_AES_256
	};

    dec_len = sizeof(buf);
    ret = id2_client_decrypt(ciphertext, sizeof(ciphertext), buf, &dec_len);
    if (ret != IROT_SUCCESS)
    {
        id2_log_debug("id2_client_decrypt return %d error!\n", ret);
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

/* you must call it in your code to run the test! */
void id2_client_test(void)
{
    irot_result_t ret;
    uint32_t version;
    int test_count = 10;
    int i;

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

    //test
    for (i = 0; i < test_count; ++i)
    {	
				id2_log_debug("test: %d time\n", i);
        id2_log_debug("test: id2_client_get_id\n");
        test_id2_client_get_id();

        id2_log_debug("test: id2_client_get_challenge_auth_code\n");
        test_id2_client_get_challenge_auth_code();

        id2_log_debug("test: id2_client_get_timestamp_auth_code\n");
        test_id2_client_get_timestamp_auth_code();

        id2_log_debug("test: id2_client_decrypt\n");
        test_id2_client_decrypt();
    }
}
