/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include <yunit.h>
#include <yts.h>
#include <tfs.h>
#include <stdio.h>
#include "aos/aos.h"

// device info
struct device_info {
    const char *product_name;
    const char *imei;
    const char *hardware_id;
    const char *mac;
    const char *bt_mac;
    const char *build_time;
    const char *os_version;
    const char *dm_pixels;
    const char *dm_dpi;
    const char *cup_info;
    const char *storage_total;
    const char *camera_resolution;
};

extern int tfs_id2_encrypt(const uint8_t *in, uint32_t in_len,uint8_t *out, uint32_t *out_len);
extern int tfs_id2_verify(const uint8_t *in, uint32_t in_len, uint8_t *sign, uint32_t sign_len);
extern int pal_get_info(const char *key, char *value);
extern int pal_save_info(const char *key, char *value);
extern int pal_json_get_string_value(char *json_str, const char **tokens, int tokens_size, char *value);
extern int pal_json_get_number_value(char *json_str, const char **tokens, int tokens_size, int *value);
extern int pal_network_create(const char *server, int port);
extern int pal_network_send(int sockfd, const char *buf, int len);
extern int pal_network_recv(int sockfd, char *buf, int *len);
extern int pal_collect_device_info(struct device_info *pInfo);
extern int http_get_seed(const char *func, const char *arg, char *seed);
extern int http_activate_dev(const char *func, const char *arg);

#define SIGN_IN_DATA_SIZE 4096
static uint8_t sign_in_data[SIGN_IN_DATA_SIZE + 1];
#define SIGN_OUT_BUF_MAX 256
static uint8_t sign_out_data[SIGN_OUT_BUF_MAX + 1] = {0};

#define ENCRYPT_IN_DATA_SIZE 3744
#define ENCRYPT_OUT_DATA_SIZE 4096
static uint8_t enc_in_data[ENCRYPT_OUT_DATA_SIZE + 1];
static uint8_t enc_out_data[ENCRYPT_OUT_DATA_SIZE + 256] = {0};
static uint8_t dec_out_data[ENCRYPT_OUT_DATA_SIZE + 1] = {0};

#define AUTH_CODE_BUF_MAX 256
static uint8_t auth_code[AUTH_CODE_BUF_MAX] = {0};

static void prepare_sign_test_data(int size) {
    int i = 0;
    for (i = 0;i < size;i ++) {
        sign_in_data[i] = i%10 + '0';
    }
    sign_in_data[size] = '\0';
}

static void prepare_decrypt_test_data(int size) {
    int i = 0;
    for (i = 0;i < size;i ++) {
        enc_in_data[i] = i%10 + '0';
    }
    enc_in_data[size] = '\0';
}

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

static void test_tfs_id2_sign(void)
{
    int ret = 0;
    uint32_t len = SIGN_OUT_BUF_MAX;
    uint32_t in_len = 0;

    prepare_sign_test_data(SIGN_IN_DATA_SIZE);
    in_len = SIGN_IN_DATA_SIZE;
    memset(sign_out_data, 0, SIGN_OUT_BUF_MAX + 1);

    ret = tfs_id2_sign(NULL, in_len, sign_out_data, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_sign((const uint8_t *)sign_in_data, 0, sign_out_data, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_sign((const uint8_t *)sign_in_data, in_len, NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_sign((const uint8_t *)sign_in_data, in_len, sign_out_data, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_sign(NULL, 0, NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_sign((const uint8_t *)sign_in_data, in_len, sign_out_data, &len);
    YUNIT_ASSERT(ret == 0);

    ret = tfs_id2_verify((const uint8_t *)sign_in_data, in_len, sign_out_data, len);
    YUNIT_ASSERT(ret == 0);
}

static void test_tfs_id2_decrypt(void)
{
    int ret = 0;
    uint32_t enc_len = ENCRYPT_OUT_DATA_SIZE;
    uint32_t dec_len = ENCRYPT_OUT_DATA_SIZE;
    uint32_t in_len = 0;

    prepare_decrypt_test_data(ENCRYPT_IN_DATA_SIZE);
    in_len = ENCRYPT_IN_DATA_SIZE;

    memset(enc_out_data, 0, ENCRYPT_OUT_DATA_SIZE + 256);
    memset(dec_out_data, 0, ENCRYPT_OUT_DATA_SIZE + 1);
    ret = tfs_id2_encrypt((uint8_t *)enc_in_data, in_len, enc_out_data, &enc_len);
    YUNIT_ASSERT(ret == 0);

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
}

static void test_tfs_get_auth_code(void)
{
    int ret = 0;
    uint32_t len = AUTH_CODE_BUF_MAX;

    memset(auth_code, 0, AUTH_CODE_BUF_MAX);

    ret = tfs_get_auth_code(auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_auth_code(NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_auth_code(NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_get_auth_code(auth_code, &len);
    YUNIT_ASSERT(ret == 0);
}

static void test_tfs_activate_device(void)
{
    int ret = -1;
    const char *key = "activate";

    aos_kv_del(key);

    ret = tfs_is_device_activated();
    YUNIT_ASSERT(ret != 0);

    ret = tfs_activate_device();
    YUNIT_ASSERT(ret == 0);

    ret = tfs_is_device_activated();
    YUNIT_ASSERT(ret == 0);
}

static void test_tfs_id2_get_auth_code(void)
{
    int ret = -1;
    uint32_t len = AUTH_CODE_BUF_MAX;
    uint64_t timestamp = 0;

    timestamp = 1487922356796ULL;
    memset(auth_code, 0, AUTH_CODE_BUF_MAX);

    ret = tfs_id2_get_auth_code(timestamp, NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_auth_code(timestamp, auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_auth_code(timestamp, NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_auth_code(timestamp, auth_code, &len);
    YUNIT_ASSERT(ret == 0);
}

static void test_tfs_id2_get_digest_auth_code(void)
{
    int ret = -1;
    uint32_t len = AUTH_CODE_BUF_MAX;
    uint64_t timestamp = 0; // in ms
    char *digest = "abcd";
    char digest1[258];
    int i = 0;

    timestamp = 1487922356796ULL;
    memset(auth_code, 0, AUTH_CODE_BUF_MAX);

    ret = tfs_id2_get_digest_auth_code(timestamp, NULL, strlen(digest), auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_digest_auth_code(timestamp, (uint8_t *)digest, 0, auth_code, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_digest_auth_code(timestamp, (uint8_t *)digest, strlen(digest), NULL, &len);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_digest_auth_code(timestamp, (uint8_t *)digest, strlen(digest), auth_code, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_digest_auth_code(timestamp, NULL, 0, NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = tfs_id2_get_digest_auth_code(timestamp, (uint8_t *)digest, strlen(digest), auth_code, &len);
    YUNIT_ASSERT(ret == 0);

    for (i = 0;i < 257; i ++) {
        digest1[i] = i%10 + '0';
    }
    digest1[257] = '\0';
    len = AUTH_CODE_BUF_MAX;
    ret = tfs_id2_get_digest_auth_code(timestamp, (uint8_t *)digest1, strlen(digest1), auth_code, &len);
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

static void test_tfs_pal_storage(void)
{
    int ret = -1;
    ret = pal_save_info(NULL, "ok");
    YUNIT_ASSERT(ret != 0);

    ret = pal_save_info("key", NULL);
    YUNIT_ASSERT(ret != 0);

    ret = pal_save_info(NULL, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = pal_get_info("key", NULL);
    YUNIT_ASSERT(ret != 0);

    ret = pal_get_info(NULL, "ok");
    YUNIT_ASSERT(ret != 0);

    ret = pal_get_info(NULL, NULL);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_pal_json(void)
{
    int ret = -1;
    char *json_str = "{\"code\":18, \"msg\":\"id2 not exist\", \"value\":false}";
    char *json_str1 = "{code:18, msg:\"id2 not exist\", value:false}";
    const char *tokens[1];
    int tokens_size = 1;
    char code[3] = {0};
    int msg;

    tokens[0] = "code";
    ret = pal_json_get_string_value(NULL, tokens, tokens_size, code);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_string_value(json_str, NULL, tokens_size, code);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_string_value(json_str, tokens, 0, code);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_string_value(json_str, tokens, tokens_size, NULL);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_string_value(NULL, NULL, 0, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = pal_json_get_string_value(json_str1, tokens, tokens_size, code);
    YUNIT_ASSERT(ret != 0);

    tokens[0] = "code2";
    ret = pal_json_get_string_value(json_str, tokens, tokens_size, code);
    YUNIT_ASSERT(ret != 0);

    tokens[0] = "code";
    ret = pal_json_get_string_value(json_str, tokens, tokens_size, code);
    YUNIT_ASSERT(ret != 0);

    tokens[0] = "msg";
    ret = pal_json_get_number_value(NULL, tokens, tokens_size, &msg);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_number_value(json_str, NULL, tokens_size, &msg);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_number_value(json_str, tokens, 0, &msg);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_number_value(json_str, tokens, tokens_size, NULL);
    YUNIT_ASSERT(ret != 0);
    ret = pal_json_get_number_value(NULL, NULL, 0, NULL);
    YUNIT_ASSERT(ret != 0);

    ret = pal_json_get_number_value(json_str1, tokens, tokens_size, &msg);
    YUNIT_ASSERT(ret != 0);

    tokens[0] = "msg2";
    ret = pal_json_get_number_value(json_str, tokens, tokens_size, &msg);
    YUNIT_ASSERT(ret != 0);

    tokens[0] = "msg";
    ret = pal_json_get_number_value(json_str, tokens, tokens_size, &msg);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_pal_network(void)
{
    int ret = 0;
    const char *hostname = "id2server.yunos.com";
    const char *hostname2 = "hello.yunos.com";
    int port = 300;
    char buf[20] = {"helloworld!"};
    int buf_len = 10;

    ret = pal_network_create(hostname2, port);
    YUNIT_ASSERT(ret != 0);

    ret = pal_network_create(hostname, port);
    YUNIT_ASSERT(ret != 0);

    ret = pal_network_send(200, buf, buf_len);
    YUNIT_ASSERT(ret != 0);

    ret = pal_network_recv(200, buf, &buf_len);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_pal_device(void)
{
    int ret = 0;
    ret = pal_collect_device_info(NULL);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_id2_verify(void)
{
    int ret = 0;
    uint32_t len = 128;
    uint32_t in_len = 0;

    prepare_sign_test_data(SIGN_IN_DATA_SIZE);
    in_len = strlen(sign_in_data);
    memset(sign_out_data, 0, SIGN_OUT_BUF_MAX);

    ret = tfs_id2_verify(NULL, in_len, sign_out_data, len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_verify(sign_in_data, 0, sign_out_data, len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_verify(sign_in_data, in_len, NULL, len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_verify(sign_in_data, in_len, sign_out_data, 0);
    YUNIT_ASSERT(ret != 0);

    prepare_sign_test_data(SIGN_IN_DATA_SIZE);
    in_len = strlen(sign_in_data);
    ret = tfs_id2_verify(sign_in_data, in_len, sign_out_data, len);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_id2_encrypt(void)
{
    int ret = 0;
    uint32_t enc_len = ENCRYPT_OUT_DATA_SIZE + 256;
    uint32_t dec_len = ENCRYPT_OUT_DATA_SIZE;
    uint32_t in_len = 0;

    prepare_decrypt_test_data(ENCRYPT_IN_DATA_SIZE);
    in_len = strlen(enc_in_data);
    memset(enc_out_data, 0, ENCRYPT_OUT_DATA_SIZE);

    ret = tfs_id2_encrypt(NULL, in_len, enc_out_data, &enc_len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_encrypt(enc_in_data, 0, enc_out_data, &enc_len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_encrypt(enc_in_data, in_len, NULL, &enc_len);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_encrypt(enc_in_data, in_len, enc_out_data, NULL);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_encrypt(NULL, 0, NULL, NULL);
    YUNIT_ASSERT(ret != 0);
    ret = tfs_id2_encrypt(enc_in_data, in_len, enc_out_data, &enc_len);
    YUNIT_ASSERT(ret == 0);

    prepare_decrypt_test_data(ENCRYPT_OUT_DATA_SIZE + 1);
    in_len = strlen(enc_in_data);

    ret = tfs_id2_encrypt(enc_in_data, in_len, enc_out_data, &enc_len);
    YUNIT_ASSERT(ret != 0);
}

static void test_tfs_http(void)
{
    char fun[41] = {0};
    char argu[1026] = {0};
    char seed[100] = {0};
    int ret = 0;
    int i = 0;

    for (i = 0; i < 40; i ++) {
        fun[i] = i%10 + '0';
    }
    fun[40] = '\0';
    for (i = 0; i < 1025; i ++) {
        argu[i] = i%10 + '0';
    }
    argu[1025] = '\0';
    ret = http_get_seed(fun, argu, seed);
    YUNIT_ASSERT(ret != 0);

    ret = http_activate_dev(fun, argu);
    YUNIT_ASSERT(ret != 0);
}

static yunit_test_case_t krhino_tfs_testcases[] = {
    { "tfs_get_ID2", test_tfs_get_ID2},
    { "tfs_id2_sign", test_tfs_id2_sign},
    { "tfs_id2_decrypt", test_tfs_id2_decrypt},
    { "tfs_activate_device", test_tfs_activate_device},
    { "tfs_get_auth_code", test_tfs_get_auth_code},
    { "tfs_id2_get_auth_code", test_tfs_id2_get_auth_code},
    { "tfs_id2_get_digest_auth_code", test_tfs_id2_get_digest_auth_code},
    { "tfs_pal_storage", test_tfs_pal_storage},
    { "tfs_pal_json", test_tfs_pal_json},
    { "tfs_pal_network", test_tfs_pal_network},
    { "tfs_pal_device", test_tfs_pal_device},
    { "tfs_id2_verify", test_tfs_id2_verify},
    { "tfs_id2_encrypt", test_tfs_id2_encrypt},
    { "tfs_http", test_tfs_http},
    YUNIT_TEST_CASE_NULL
};

static yunit_test_suite_t suites[] = {
    { "tfs", init, cleanup, setup, teardown, krhino_tfs_testcases },
    YUNIT_TEST_SUITE_NULL
};

void test_tfs(void)
{
    yunit_add_test_suites(suites);
}

