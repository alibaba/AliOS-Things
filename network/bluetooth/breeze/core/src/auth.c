/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "auth.h"
#include "core.h"
#include "utils.h"
#include "breeze_hal_ble.h"
#include "breeze_hal_sec.h"


static uint8_t device_secret[MAX_SECRET_LEN] = { 0 };
uint8_t product_secret[PRODUCT_SECRET_LEN]  = { 0 };
static uint16_t device_secret_len = 0;
static uint16_t product_secret_len = 0;

#define SEQUENCE_STR "sequence"
#define DEVICE_NAME_STR "deviceName"
#define DEVICE_SECRET_STR "deviceSecret"
#define PRODUCT_KEY_STR "productKey"
#define PRODUCT_SECRET_STR "productSecret"

#define HI_SERVER_STR "Hi,Server"
#define HI_CLIENT_STR "Hi,Client"
#define OK_STR "OK"

bool g_dn_complete = false;

auth_t g_auth;

static void on_timeout(void *arg1, void *arg2)
{
    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_2, BZ_ETIMEOUT);
}

static void ikm_init(ali_init_t const *p_init)
{
#ifndef CONFIG_MODEL_SECURITY
    device_secret_len = p_init->secret.length;
    memcpy(device_secret, p_init->secret.p_data, device_secret_len);
#endif
    product_secret_len = p_init->product_secret.length;
    memcpy(product_secret, p_init->product_secret.p_data, product_secret_len);

    memcpy(g_auth.ikm + g_auth.ikm_len, p_init->product_secret.p_data, p_init->product_secret.length);
    g_auth.ikm_len += p_init->product_secret.length;

    g_auth.ikm[g_auth.ikm_len++] = ',';
}

ret_code_t auth_init(ali_init_t const *p_init, tx_func_t tx_func)
{
    ret_code_t ret = BZ_SUCCESS;

    memset(&g_auth, 0, sizeof(auth_t));
    g_auth.state = AUTH_STATE_IDLE;
    g_auth.tx_func = tx_func;

    if(p_init->product_key.length == PRODUCT_KEY_LEN &&
         p_init->device_key.length != 0){
        memcpy(g_auth.product_key, p_init->product_key.p_data, PRODUCT_KEY_LEN);
        memcpy(g_auth.device_name, p_init->device_key.p_data, p_init->device_key.length);
        g_auth.device_name_len = p_init->device_key.length;
    }
    if(p_init->secret.length == DEVICE_SECRET_LEN){
        memcpy(g_auth.secret, p_init->secret.p_data, DEVICE_SECRET_LEN);
    } 
    g_auth.key_len = p_init->device_key.length;
    if(g_auth.key_len > 0){
        memcpy(g_auth.key, p_init->device_key.p_data, g_auth.key_len);
    }
    ikm_init(p_init);
    ret = os_timer_new(&g_auth.timer, on_timeout, &g_auth, BZ_AUTH_TIMEOUT);
    return ret;
}

void auth_reset(void)
{
    g_auth.state = AUTH_STATE_IDLE;
    os_timer_stop(&g_auth.timer);
}

void auth_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    uint32_t err_code;

    if (length == 0 || (cmd & BZ_CMD_TYPE_MASK) != BZ_CMD_AUTH) {
        return;
    }

    switch (g_auth.state) {
        case AUTH_STATE_RAND_SENT:
            g_dn_complete = true;
            if (cmd == BZ_CMD_AUTH_REQ &&
                memcmp(HI_SERVER_STR, p_data, MIN(length, strlen(HI_SERVER_STR))) == 0) {
                g_auth.state = AUTH_STATE_REQ_RECVD;
                err_code = g_auth.tx_func(BZ_CMD_AUTH_RSP, HI_CLIENT_STR, strlen(HI_CLIENT_STR));
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                err_code = os_timer_start(&g_auth.timer);
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                    return;
                }
            } else {
                g_auth.state = AUTH_STATE_FAILED;
            }
            break;

        case AUTH_STATE_REQ_RECVD:
            if (cmd == BZ_CMD_AUTH_CFM && memcmp(OK_STR, p_data, MIN(length, strlen(OK_STR))) == 0) {
                g_auth.state = AUTH_STATE_DONE;
            } else {
                g_auth.state = AUTH_STATE_FAILED;
            }
            break;

        default:
            err_code = g_auth.tx_func(BZ_CMD_ERR, NULL, 0);
            if (err_code != BZ_SUCCESS) {
                core_handle_err(ALI_ERROR_SRC_AUTH_SEND_ERROR, err_code);
                return;
            }
            break;
    }

    if (g_auth.state == AUTH_STATE_DONE) {
        event_notify(BZ_EVENT_AUTHENTICATED, NULL, 0);
        os_timer_stop(&g_auth.timer);
    } else if (g_auth.state == AUTH_STATE_FAILED) {
        os_timer_stop(&g_auth.timer);
        ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
    }
}

static void update_aes_key(bool use_device_key)
{
    uint8_t rand_backup[RANDOM_SEQ_LEN];
    SHA256_CTX context;
    uint8_t okm[SHA256_BLOCK_SIZE];

    memcpy(rand_backup, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    g_auth.ikm_len = 0;

    if (use_device_key) {
        memcpy(g_auth.ikm + g_auth.ikm_len, device_secret, device_secret_len);
        g_auth.ikm_len += device_secret_len;
    } else {
        memcpy(g_auth.ikm + g_auth.ikm_len, product_secret, product_secret_len);
        g_auth.ikm_len += product_secret_len;
    }

    g_auth.ikm[g_auth.ikm_len++] = ',';
    memcpy(g_auth.ikm + g_auth.ikm_len, rand_backup, sizeof(rand_backup));

    sec_sha256_init(&context);
    sec_sha256_update(&context, g_auth.ikm, g_auth.ikm_len + RANDOM_SEQ_LEN);
    sec_sha256_final(&context, okm);
    memcpy(g_auth.okm, okm, MAX_OKM_LEN);

    // notify key updated
    transport_update_key(g_auth.okm);
}

void auth_connected(void)
{
    uint32_t err_code;

    err_code = os_timer_start(&g_auth.timer);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_0, err_code);
        return;
    }

    g_auth.state = AUTH_STATE_CONNECTED;
    get_random(g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    update_aes_key(false);
}

void auth_service_enabled(void)
{
    uint32_t err_code;

    g_auth.state = AUTH_STATE_SVC_ENABLED;
    err_code = g_auth.tx_func(BZ_CMD_AUTH_RAND, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }
}

void auth_tx_done(void)
{
    uint32_t err_code;

    if (g_auth.state == AUTH_STATE_SVC_ENABLED) {
        g_auth.state = AUTH_STATE_RAND_SENT;
#ifdef CONFIG_MODEL_SECURITY
        return;
#else
        err_code = g_auth.tx_func(BZ_CMD_AUTH_KEY, g_auth.key, g_auth.key_len);
        if (err_code != BZ_SUCCESS) {
            core_handle_err(ALI_ERROR_SRC_AUTH_SEND_KEY, err_code);
            return;
        }
        return;
#endif
    } else if (g_auth.state == AUTH_STATE_RAND_SENT) {
#ifdef CONFIG_MODEL_SECURITY
        return;
#else
        update_aes_key(true);
#endif
    }
}

bool auth_is_authdone(void)
{
    return (bool)(g_auth.state == AUTH_STATE_DONE);
}

ret_code_t auth_get_device_name(uint8_t **pp_device_name, uint8_t *p_length)
{
    if(g_auth.device_name_len){
        *pp_device_name = g_auth.device_name;
        *p_length = g_auth.device_name_len;
        return BZ_SUCCESS;
    } else{
        return BZ_EDATASIZE;
    }
}

ret_code_t auth_get_product_key(uint8_t **pp_prod_key, uint8_t *p_length)
{
    *pp_prod_key = g_auth.product_key;
    *p_length = PRODUCT_KEY_LEN;
    return BZ_SUCCESS;
}

ret_code_t auth_get_secret(uint8_t **pp_secret, uint8_t *p_length)
{
    *pp_secret = g_auth.secret;
    *p_length = DEVICE_SECRET_LEN;
    return BZ_SUCCESS;
}

#ifdef CONFIG_AIS_SECURE_ADV
static void make_seq_le(uint32_t *seq)
{
    uint32_t test_num = 0x01020304;
    uint8_t *byte = (uint8_t *)(&test_num);

    if (*byte == 0x04)
        return;
    uint32_t tmp = *seq;
    SET_U32_LE(seq, tmp);
}

int auth_calc_adv_sign(uint32_t seq, uint8_t *sign)
{
    SHA256_CTX context;
    uint8_t full_sign[32], i, *p;

    make_seq_le(&seq);
    sec_sha256_init(&context);

    sec_sha256_update(&context, DEVICE_NAME_STR, strlen(DEVICE_NAME_STR));
    sec_sha256_update(&context, g_auth.device_name, g_auth.device_name_len);

#ifndef CONFIG_MODEL_SECURITY
    sec_sha256_update(&context, DEVICE_SECRET_STR, strlen(DEVICE_SECRET_STR));
    sec_sha256_update(&context, g_auth.secret, DEVICE_SECRET_LEN);
#else
    sec_sha256_update(&context, PRODUCT_SECRET_STR, strlen(PRODUCT_SECRET_STR));
    sec_sha256_update(&context, product_secret, PRODUCT_SECRET_LEN);
#endif

    sec_sha256_update(&context, PRODUCT_KEY_STR, strlen(PRODUCT_KEY_STR));
    sec_sha256_update(&context, g_auth.product_key, PRODUCT_KEY_LEN);

    sec_sha256_update(&context, SEQUENCE_STR, strlen(SEQUENCE_STR));
    sec_sha256_update(&context, &seq, sizeof(seq));

    sec_sha256_final(&context, full_sign);

    memcpy(sign, full_sign, 4);

    return 0;
}
#endif
