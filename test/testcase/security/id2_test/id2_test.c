/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <id2_client.h>
#include <stdio.h>

#include "yts/yunit.h"
#include "yts/yts.h"
#include "aos/kernel.h"

#define MAX_AUTH_LEN   256
#define MAX_CRYPTO_LEN 4096

static void test_id2_client_get_id(void)
{
    int ret = 0;
    uint32_t len_1 = ID2_ID_LEN + 1;
    uint8_t id2_1[ID2_ID_LEN + 1] = {0};
    uint32_t len_2 = ID2_ID_LEN - 1;
    uint8_t id2_2[ID2_ID_LEN - 1] = {0};

    //ret = id2_client_get_id(id2_1, &len_1);
    //YUNIT_ASSERT(ret == IROT_SUCCESS);

    ret = id2_client_get_id(id2_2, &len_2);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_id(NULL, &len_1);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_id(id2_1, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_id(NULL, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);
}

static void test_id2_client_decrypt(void)
{
    int ret = 0;
    uint8_t enc_out_data[] = { 0x03, 0x4b, 0x1e, 0x52, 0x33, 0x5f, 0x32, 0x31,
                               0x26, 0x70, 0x91, 0xab, 0x4a, 0x8b, 0x80, 0xdb
                             };
    uint8_t dec_out_data[sizeof(enc_out_data)] = {0};
    uint32_t enc_len = sizeof(enc_out_data);
    uint32_t dec_len = sizeof(dec_out_data);

    ret = id2_client_decrypt(NULL, enc_len, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_decrypt(enc_out_data, enc_len, NULL, &dec_len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_decrypt(enc_out_data, enc_len, dec_out_data, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_decrypt(NULL, 0, NULL, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);
}

static void test_id2_client_get_challenge_auth_code(void)
{
    int ret = -1;
    uint32_t len = AUTH_CODE_BUF_LEN;
    uint8_t auth_code[AUTH_CODE_BUF_LEN] = {0};
    const char *extra = "abcd1234";
    const char *challenge = "55B83408399FA660F05C82E4F25333DC";
    const char *challenge_s = "";
    const char *challenge_l = "55B83408399FA660F05C82E4F25333DC1";

    ret = id2_client_get_challenge_auth_code(NULL, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, NULL, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, auth_code, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    //ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, auth_code, &len);
    //YUNIT_ASSERT(ret == IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code(NULL, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), NULL, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), auth_code, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    //len = AUTH_CODE_BUF_LEN;
    //ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), auth_code, &len);
    //YUNIT_ASSERT(ret == IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge_s, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge_l, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge_s, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_challenge_auth_code((const uint8_t *)challenge_l, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);
}

static void test_id2_client_get_timestamp_auth_code(void)
{
    int ret = -1;
    uint32_t len = AUTH_CODE_BUF_LEN;
    uint8_t auth_code[AUTH_CODE_BUF_LEN] = {0};
    const char *extra = "abcd1234";
    const char *timestamp = "1509070455668";
    const char *timestamp_s = "";
    const char *timestamp_l = "55B83408399FA660F05C82E4F25333DC1";

    ret = id2_client_get_timestamp_auth_code(NULL, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, NULL, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, auth_code, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    //ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, auth_code, &len);
    //YUNIT_ASSERT(ret == IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code(NULL, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), NULL, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), auth_code, NULL);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    //len = AUTH_CODE_BUF_LEN;
    //ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), auth_code, &len);
    //YUNIT_ASSERT(ret == IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp_s, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp_l, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp_s, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);

    ret = id2_client_get_timestamp_auth_code((const uint8_t *)timestamp_l, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != IROT_SUCCESS);
}

static int init(void)
{
    id2_client_init();
    return 0;
}

static int cleanup(void)
{
    id2_client_cleanup();
    return 0;
}

static void setup(void)
{
}

static void teardown(void)
{
}

static yunit_test_case_t aos_id2_client_testcases[] = {
    {"id2_client_get_id", test_id2_client_get_id},
    {"id2_client_decrypt", test_id2_client_decrypt},
    {"id2_client_get_challenge_auth_code", test_id2_client_get_challenge_auth_code},
    {"id2_client_get_timestamp_auth_code", test_id2_client_get_timestamp_auth_code},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "id2", init, cleanup, setup, teardown, aos_id2_client_testcases},
    YUNIT_TEST_SUITE_NULL
};

void test_id2(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_id2);
