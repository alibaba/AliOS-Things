/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sha256.h"

#include "auth.h"
#include "core.h"
#include "utils.h"
#include "breeze_hal_ble.h"

static uint8_t device_secret[MAX_SECRET_LEN] = { 0 };
static uint8_t product_secret[PRODUCT_SECRET_LEN]  = { 0 };
static uint16_t device_secret_len = 0;
static uint16_t product_secret_len = 0;

#define SEQUENCE_STR "sequence"
#define DEVICE_NAME_STR "deviceName"
#define PRODUCT_KEY_STR "productKey"
#define DEVICE_SECRET_STR "deviceSecret"

#define HI_SERVER_STR "Hi,Server"
#define HI_CLIENT_STR "Hi,Client"
#define OK_STR "OK"

bool g_dn_complete = false;
static void on_timeout(void *arg1, void *arg2)
{
    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_2, BZ_ETIMEOUT);
}

static void get_rand(auth_t *p_auth)
{
    uint8_t  bytes_available = 0;
    uint32_t seed = os_now_ms();
    uint8_t  byte[4 + 1];
    uint32_t result;
    uint16_t bytes_copy;

    srand((unsigned int)seed);
    result = rand();

    while (bytes_available < RANDOM_SEQ_LEN) {
        seed += result;
        seed = seed % 9999;
        snprintf((char *)byte, sizeof(byte), "%04d", seed);
        bytes_copy = RANDOM_SEQ_LEN - bytes_available;
        bytes_copy = (bytes_copy > 4) ? 4 : bytes_copy;
        memcpy(p_auth->ikm + p_auth->ikm_len + bytes_available, byte,
               bytes_copy);
        bytes_available += bytes_copy;
    }
}

static void generate_key(auth_t *p_auth)
{
    SHA256_CTX context;
    uint8_t okm[SHA256_BLOCK_SIZE];

    sha256_init(&context);
    sha256_update(&context, p_auth->ikm, p_auth->ikm_len + RANDOM_SEQ_LEN);
    sha256_final(&context, okm);
    memcpy(p_auth->okm, okm, MAX_OKM_LEN);
}

static void ikm_init(auth_t *p_auth, ali_init_t const *p_init)
{
    device_secret_len = p_init->secret.length;
    memcpy(device_secret, p_init->secret.p_data, device_secret_len);
    product_secret_len = p_init->product_secret.length;
    memcpy(product_secret, p_init->product_secret.p_data, product_secret_len);

    memcpy(p_auth->ikm + p_auth->ikm_len, p_init->product_secret.p_data, p_init->product_secret.length);
    p_auth->ikm_len += p_init->product_secret.length;

    p_auth->ikm[p_auth->ikm_len++] = ',';
}

ret_code_t auth_init(auth_t *p_auth, ali_init_t const *p_init, tx_func_t tx_func)
{
    ret_code_t ret = BZ_SUCCESS;

    memset(p_auth, 0, sizeof(auth_t));
    p_auth->state = AUTH_STATE_IDLE;
    p_auth->tx_func = tx_func;

    if (p_init->product_key.length == PRODUCT_KEY_LEN &&
        p_init->device_key.length != 0 &&
        p_init->secret.length == DEVICE_SECRET_LEN) {
        memcpy(p_auth->product_key, p_init->product_key.p_data, PRODUCT_KEY_LEN);
        memcpy(p_auth->secret, p_init->secret.p_data, DEVICE_SECRET_LEN);
        memcpy(p_auth->device_name, p_init->device_key.p_data, p_init->device_key.length);
        p_auth->device_name_len = p_init->device_key.length;
    }

    p_auth->key_len = p_init->device_key.length;
    memcpy(p_auth->key, p_init->device_key.p_data, p_auth->key_len);

    ikm_init(p_auth, p_init);

    ret = os_timer_new(&p_auth->timer, on_timeout, p_auth, BZ_AUTH_TIMEOUT);
    return ret;
}

void auth_reset(auth_t *p_auth)
{
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);
    p_auth->state = AUTH_STATE_IDLE;
    os_timer_stop(&p_auth->timer);
}

void auth_rx_command(auth_t *p_auth, uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0 || (cmd & BZ_CMD_TYPE_MASK) != BZ_CMD_AUTH) {
        return;
    }

    switch (p_auth->state) {
        case AUTH_STATE_RAND_SENT: {
            g_dn_complete = true;
            if (cmd == BZ_CMD_AUTH_REQ &&
                memcmp(HI_SERVER_STR, p_data, MIN(length, strlen(HI_SERVER_STR))) == 0) {
                p_auth->state = AUTH_STATE_REQ_RECVD;
                err_code = p_auth->tx_func(BZ_CMD_AUTH_RSP, HI_CLIENT_STR, strlen(HI_CLIENT_STR));
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                err_code = os_timer_start(&p_auth->timer);
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                    return;
                }
            } else {
                p_auth->state = AUTH_STATE_FAILED;
            }
        } break;

        case AUTH_STATE_REQ_RECVD:
            if (cmd == BZ_CMD_AUTH_CFM &&
                memcmp(OK_STR, p_data, MIN(length, strlen(OK_STR))) == 0) {
                p_auth->state = AUTH_STATE_DONE;
            } else {
                p_auth->state = AUTH_STATE_FAILED;
            }
            break;

        default:
            err_code = p_auth->tx_func(BZ_CMD_ERR, NULL, 0);
            if (err_code != BZ_SUCCESS) {
                core_handle_err(ALI_ERROR_SRC_AUTH_SEND_ERROR, err_code);
                return;
            }
            break;
    }

    if (p_auth->state == AUTH_STATE_DONE) {
        event_notify(BZ_EVENT_AUTHENTICATED);
        os_timer_stop(&p_auth->timer);
    } else if (p_auth->state == AUTH_STATE_FAILED) {
        os_timer_stop(&p_auth->timer);
        ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
    }
}

static void update_aes_key(auth_t *p_auth, bool use_device_key)
{
    uint8_t rand_backup[RANDOM_SEQ_LEN];

    memcpy(rand_backup, p_auth->ikm + p_auth->ikm_len, RANDOM_SEQ_LEN);
    p_auth->ikm_len = 0;

    if (use_device_key) {
        memcpy(p_auth->ikm + p_auth->ikm_len, device_secret, device_secret_len);
        p_auth->ikm_len += device_secret_len;
    } else {
        memcpy(p_auth->ikm + p_auth->ikm_len, product_secret, product_secret_len);
        p_auth->ikm_len += product_secret_len;
    }

    p_auth->ikm[p_auth->ikm_len++] = ',';
    memcpy(p_auth->ikm + p_auth->ikm_len, rand_backup, sizeof(rand_backup));

    generate_key(p_auth);
    // notify key updated
    transport_update_key(p_auth->okm);
}

void auth_connected(auth_t *p_auth)
{
    uint32_t err_code;

    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    err_code = os_timer_start(&p_auth->timer);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_0, err_code);
        return;
    }

    p_auth->state = AUTH_STATE_CONNECTED;
    get_rand(p_auth);
    update_aes_key(p_auth, false);
}

void auth_service_enabled(auth_t *p_auth)
{
    uint32_t err_code;

    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    p_auth->state = AUTH_STATE_SVC_ENABLED;

    err_code = p_auth->tx_func(BZ_CMD_AUTH_RAND, p_auth->ikm + p_auth->ikm_len, RANDOM_SEQ_LEN);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }
}

void auth_tx_done(auth_t *p_auth)
{
    uint32_t err_code;

    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    if (p_auth->state == AUTH_STATE_SVC_ENABLED) {
#ifdef CONFIG_MODEL_SECURITY
        p_auth->state = AUTH_STATE_RAND_SENT;
        return;
#else
        p_auth->state = AUTH_STATE_RAND_SENT;
        err_code = p_auth->tx_func(BZ_CMD_AUTH_KEY, p_auth->key, p_auth->key_len);
        if (err_code != BZ_SUCCESS) {
            core_handle_err(ALI_ERROR_SRC_AUTH_SEND_KEY, err_code);
            return;
        }
        return;
#endif
    } else if (p_auth->state == AUTH_STATE_RAND_SENT) {
#ifdef CONFIG_MODEL_SECURITY
        return;
#else
        update_aes_key(p_auth, true);
#endif
    }
}

bool auth_is_authdone(void)
{
    auth_t *p_auth = &g_core->auth;
    return (bool)(p_auth->state == AUTH_STATE_DONE);
}

ret_code_t auth_get_device_name(uint8_t **pp_device_name, uint8_t *p_length)
{
    auth_t *p_auth = &g_core->auth;

    *pp_device_name = p_auth->device_name;
    *p_length = p_auth->device_name_len;
    return BZ_SUCCESS;
}

ret_code_t auth_get_product_key(uint8_t **pp_prod_key, uint8_t *p_length)
{
    auth_t *p_auth = &g_core->auth;

    *pp_prod_key = p_auth->product_key;
    *p_length = PRODUCT_KEY_LEN;
    return BZ_SUCCESS;
}

ret_code_t auth_get_secret(uint8_t **pp_secret, uint8_t *p_length)
{
    auth_t *p_auth = &g_core->auth;

    *pp_secret = p_auth->secret;
    *p_length  = DEVICE_SECRET_LEN;
    return BZ_SUCCESS;
}

#ifdef CONFIG_AIS_SECURE_ADV
static void make_seq_le(uint32_t *seq)
{
    uint32_t test_num = 0x01020304;
    uint8_t *byte     = (uint8_t *)(&test_num);

    if (*byte == 0x04)
        return;
    uint32_t tmp = *seq;
    SET_U32_LE(seq, tmp);
}

int auth_calc_adv_sign(auth_t *p_auth, uint32_t seq, uint8_t *sign)
{
    SHA256_CTX context;
    uint8_t full_sign[32], i, *p;

    make_seq_le(&seq);
    sha256_init(&context);

    sha256_update(&context, DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
    sha256_update(&context, p_auth->device_name, p_auth->device_name_len);

    sha256_update(&context, DEVICE_SECRET_STR, strlen(DEVICE_SECRET_STR));
    sha256_update(&context, p_auth->secret, DEVICE_SECRET_LEN);

    sha256_update(&context, PRODUCT_KEY_STR, strlen(PRODUCT_KEY_STR));
    sha256_update(&context, p_auth->product_key, PRODUCT_KEY_LEN);

    sha256_update(&context, SEQUENCE_STR, strlen(SEQUENCE_STR));
    sha256_update(&context, &seq, sizeof(seq));

    sha256_final(&context, full_sign);

    memcpy(sign, full_sign, 4);

    return 0;
}
#endif
