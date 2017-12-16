/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yunit.h>
#include <yts.h>
#include <tfs.h>
#include <stdio.h>
#include "aos/aos.h"

#define MAX_AUTH_LEN   256
#define MAX_CRYPTO_LEN 4096

static void test_tfs_get_ID2(void)
{
    int ret = 0;
    uint32_t len_1 = TFS_ID2_LEN + 1;
    uint8_t id2_1[TFS_ID2_LEN + 1] = {0};
    uint32_t len_2 = TFS_ID2_LEN - 1;
    uint8_t id2_2[TFS_ID2_LEN - 1] = {0};

    ret = tfs_get_ID2(id2_1, &len_1);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_get_ID2(id2_2, &len_2);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_ID2(NULL, &len_1);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_ID2(id2_1, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_ID2(NULL, NULL);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_id2_decrypt(void)
{
    int ret = 0;
    uint8_t enc_out_data[] = { 0x03, 0x4b, 0x1e, 0x52, 0x33, 0x5f, 0x32, 0x31,
                               0x26, 0x70, 0x91, 0xab, 0x4a, 0x8b, 0x80, 0xdb };
    uint8_t dec_out_data[MAX_CRYPTO_LEN] = {0};
    uint32_t enc_len = sizeof(enc_out_data);
    uint32_t dec_len = sizeof(dec_out_data);

    ret = tfs_id2_decrypt(NULL, enc_len, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_decrypt(enc_out_data, 0, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_decrypt(enc_out_data, enc_len, NULL, &dec_len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_decrypt(enc_out_data, enc_len, dec_out_data, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_decrypt(NULL, 0, NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_decrypt(enc_out_data, enc_len, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret == 0);

    dec_len = sizeof(dec_out_data);
    ret = tfs_id2_decrypt(enc_out_data, enc_len + 1, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret != 0);

    dec_len = enc_len - 1;
    ret = tfs_id2_decrypt(enc_out_data, enc_len, dec_out_data, &dec_len);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_id2_get_challenge_auth_code(void)
{
    int ret = -1;
    uint32_t len = MAX_AUTH_LEN;
    uint8_t auth_code[MAX_AUTH_LEN] = {0};
    const char *extra = "abcd1234";
    const char *challenge = "55B83408399FA660F05C82E4F25333DC";
    const char *challenge_s = "";
    const char *challenge_l = "55B83408399FA660F05C82E4F25333DC1";

    ret = tfs_id2_get_challenge_auth_code(NULL, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_id2_get_challenge_auth_code(NULL, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    len = MAX_AUTH_LEN;
    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge_s, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge_l, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge_s, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_challenge_auth_code((const uint8_t *)challenge_l, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_id2_get_timestamp_auth_code(void)
{
    int ret = -1;
    uint32_t len = MAX_AUTH_LEN;
    uint8_t auth_code[MAX_AUTH_LEN] = {0};
    const char *extra = "abcd1234";
    const char *timestamp = "1509070455668";
    const char *timestamp_s = "";
    const char *timestamp_l = "55B83408399FA660F05C82E4F25333DC1";

    ret = tfs_id2_get_timestamp_auth_code(NULL, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_id2_get_timestamp_auth_code(NULL, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    len = MAX_AUTH_LEN;
    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp_s, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp_l, NULL, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp_s, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_timestamp_auth_code((const uint8_t *)timestamp_l, extra, strlen(extra), auth_code, &len);
    YUNIT_ASSERT(ret != 0);
}

static int init(void)
{
    return 0;
}

static int cleanup(void)
{
    return 0;
}

static void setup(void)
{

}

static void teardown(void)
{

}

static yunit_test_case_t aos_tfs_testcases[] = {
    { "tfs_get_ID2", test_tfs_get_ID2},
    { "tfs_id2_decrypt", test_tfs_id2_decrypt},
    { "tfs_id2_get_challenge_auth_code", test_tfs_id2_get_challenge_auth_code},
    { "tfs_id2_get_timestamp_auth_code", test_tfs_id2_get_timestamp_auth_code},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "tfs", init, cleanup, setup, teardown, aos_tfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_tfs(void)
{
    yunit_add_test_suites(suites);
}
AOS_TESTCASE(test_tfs);
