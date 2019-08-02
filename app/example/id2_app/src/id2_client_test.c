/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 */

#include "id2_test.h"

static int id2_client_test_get_id(void)
{
    irot_result_t ret;
    uint32_t version = 0;
    uint32_t id2_len = 0;
    uint8_t id2[ID2_ID_LEN + 1] = {0};

    ID2_DBG_LOG("====> ID2 Client Test Get ID Start.\n");

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    ret = id2_client_get_version(&version);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client get version fail\n");
        goto _out;
    }

    ret = id2_client_get_id(id2, &id2_len);
    if (ret != IROT_ERROR_SHORT_BUFFER ||
        id2_len != ID2_ID_LEN) {
        ID2_DBG_LOG("get client id2 fail, %d\n", ret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = id2_client_get_id(id2, &id2_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get client id2 fail, %d\n", ret);
        goto _out;
    }

    ret = id2_client_get_id(id2, &id2_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get client id2 fail, %d\n", ret);
        goto _out;
    }

    if (id2_len != ID2_ID_LEN) {
        ID2_DBG_LOG("invalid id2 length, %d\n", id2_len);
        goto _out;
    }

    ID2_DBG_LOG("ID2: %s\n", id2);

_out:
    id2_client_cleanup();

    ID2_DBG_LOG("====> ID2 Client Test Get ID End.\n");

    return ret == IROT_SUCCESS ? 0 : -1;
}

static int id2_client_test_get_challenge_auth_code(void)
{
    irot_result_t ret;
    uint32_t auth_code_len = 0;
    uint8_t auth_code[ID2_AUTH_CODE_BUF_LEN] = {0};
    char *server_random = "55B83408399FA660F05C82E4F25333DC";
    char *extra = "abcd1234";

    ID2_DBG_LOG("====> ID2 Client Test Get Challenge Auth Code Start.\n");

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    ret = id2_client_get_challenge_auth_code(
              server_random, NULL, 0, auth_code, &auth_code_len);
    if (ret != IROT_ERROR_SHORT_BUFFER ||
        auth_code_len != ID2_AUTH_CODE_BUF_LEN) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = id2_client_get_challenge_auth_code(
              server_random, NULL, 0, auth_code, &auth_code_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        goto _out;
    }

    ID2_DBG_LOG("authcode[no extra]:\n [%d] %s\n", auth_code_len, auth_code);

    auth_code_len = ID2_AUTH_CODE_BUF_LEN;
    ret = id2_client_get_challenge_auth_code(server_random,
                     (uint8_t *)extra, strlen(extra), auth_code, &auth_code_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        goto _out;
    }

    ID2_DBG_LOG("authcode[extra = %s]:\n [%d] %s\n", extra, auth_code_len, auth_code);

_out:
    id2_client_cleanup();

    ID2_DBG_LOG("====> ID2 Client Test Get Challenge Auth Code End.\n");

    return ret == IROT_SUCCESS ? 0 : -1;
}

static int id2_client_test_get_timestamp_auth_code(void)
{
    irot_result_t ret;
    uint32_t auth_code_len = 0;
    uint8_t auth_code[ID2_AUTH_CODE_BUF_LEN] = {0};
    char *timestamp = "1512022279204";
    char *extra = "abcd1234";

    ID2_DBG_LOG("====> ID2 Client Test Get Timestamp Auth Code Start.\n");

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    ret = id2_client_get_timestamp_auth_code(
              timestamp, NULL, 0, auth_code, &auth_code_len);
    if (ret != IROT_ERROR_SHORT_BUFFER ||
        auth_code_len != ID2_AUTH_CODE_BUF_LEN) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = id2_client_get_timestamp_auth_code(
              timestamp, NULL, 0, auth_code, &auth_code_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        goto _out;
    }

    ID2_DBG_LOG("authcode[no extra]:\n [%d] %s\n", auth_code_len, auth_code);

    auth_code_len = ID2_AUTH_CODE_BUF_LEN;
    ret = id2_client_get_timestamp_auth_code(timestamp,
                     (uint8_t *)extra, strlen(extra), auth_code, &auth_code_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("get challenge auth code fail, %d\n", ret);
        goto _out;
    }

    ID2_DBG_LOG("authcode[extra = %s]:\n [%d] %s\n", extra, auth_code_len, auth_code);

_out:
    id2_client_cleanup();

    ID2_DBG_LOG("====> ID2 Client Test Get Timestamp Auth Code End.\n");

    return ret == IROT_SUCCESS ? 0 : -1;
}

static int id2_client_test_get_device_challenge(void)
{
    irot_result_t ret;
    uint32_t i = 0;
    uint32_t random_len = 0;
    uint8_t random[ID2_MAX_DEVICE_RANDOM_LEN + 4] = {0};

    ID2_DBG_LOG("====> ID2 Client Test Get Device Challenge Start.\n");

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    for (i = 1; i < ID2_MAX_DEVICE_RANDOM_LEN + 4; i += 4) {
        random_len = i;
        ret = id2_client_get_device_challenge(random, &random_len);
        if (ret != IROT_SUCCESS) {
            ID2_DBG_LOG("id2 client get device challenege fail, %d\n", ret);
            goto _out;
        }

        ID2_DBG_LOG("device random: [%d] %s\n", random_len, random);
        ls_osa_msleep(10);
    }

_out:
    id2_client_cleanup();

    ID2_DBG_LOG("====> ID2 Client Test Get Device Challenege End.\n");

    return ret == IROT_SUCCESS ? 0 : -1;
}

static int id2_client_test_get_secret(void)
{
    irot_result_t ret;
    uint32_t secret_len = 0;
    uint8_t secret[ID2_DERIV_SECRET_LEN + 1] = {0};
    const char* seed = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    ID2_DBG_LOG("====> ID2 Client Test Get Secret Start.\n");

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    ret = id2_client_get_secret(seed, secret, &secret_len);
    if (ret != IROT_ERROR_SHORT_BUFFER ||
        secret_len != ID2_DERIV_SECRET_LEN) {
        ID2_DBG_LOG("get client secret fail, %d\n", ret);
        ret = IROT_ERROR_GENERIC;
        goto _out;
    }

    ret = id2_client_get_secret(seed, secret, &secret_len);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client get secret fail, %d\n", ret);
        goto _out;
    }

    ID2_DBG_LOG("device secret: [%d] %s\n", secret_len, secret);

_out:
    id2_client_cleanup();

    ID2_DBG_LOG("====> ID2 Client Test Get Secret End.\n");

    return ret == IROT_SUCCESS ? 0 : -1;
}

int id2_client_unit_test(void)
{
    int ret;
    bool is_prov;
#if defined(CONFIG_ID2_DEBUG)
    char *id2_id = "0102030405060708090A0B0C";
    char *id2_key = "0102030405060708090A0B0C0D0E0F101112131415161718";
#endif

    ret = id2_client_init();
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client init fail, %d\n", ret);
        return -1;
    }

    ret = id2_client_get_prov_stat(&is_prov);
    if (ret != IROT_SUCCESS) {
        ID2_DBG_LOG("id2 client get prov stat fail, %d\r\n", ret);
        return -1;
    }

    if (is_prov == false) {
#if defined(CONFIG_ID2_DEBUG)
        ret = id2_client_set_id2_and_key(id2_id, 0x02, id2_key);
        if (ret != IROT_SUCCESS) {
            ID2_DBG_LOG("set id2 and key fail, %d\n", ret);
            return -1;
        }
#else
        ID2_DBG_LOG("no id2 in device, need to prov first!!\n");
        return -1;
#endif
    }

    ret = id2_client_test_get_id();
    if (ret < 0) {
        ID2_DBG_LOG("=================>ID2 Client Test Get ID Fail.\n\n");
        return -1;
    } else {
        ID2_DBG_LOG("=================>ID2 Client Test Get ID Pass.\n\n");
    }

    ret = id2_client_test_get_challenge_auth_code();
    if (ret < 0) {
        ID2_DBG_LOG("=================>ID2 Client Test Get Challenge Auth Code Fail.\n\n");
        return -1;
    } else {
        ID2_DBG_LOG("=================>ID2 Client Test Get Challenge Auth Code Pass.\n\n");
    }

    ret = id2_client_test_get_timestamp_auth_code();
    if (ret < 0) {
        ID2_DBG_LOG("=================>ID2 Client Test Get Timestamp Auth Code Fail.\n\n");
        return -1;
    } else {
        ID2_DBG_LOG("=================>ID2 Client Test Get Timestamp Auth Code Pass.\n\n");
    }

    ret = id2_client_test_get_device_challenge();
    if (ret < 0) {
        ID2_DBG_LOG("=================>ID2 Client Test Get Device Challenge Fail.\n\n");
        return -1;
    } else {
        ID2_DBG_LOG("=================>ID2 Client Test Get Device Challenge Pass.\n\n");
    }

    ret = id2_client_test_get_secret();
    if (ret < 0) {
        ID2_DBG_LOG("=================>ID2 Client Test Get Secret Fail.\n\n");
        return -1;
    } else {
        ID2_DBG_LOG("=================>ID2 Client Test Get Secret Pass.\n\n");
    }

    return 0;
}
