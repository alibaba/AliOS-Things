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
#include "breeze_hal_os.h"


static uint8_t device_secret[MAX_SECRET_LEN] = { 0 };
uint8_t product_secret[PRODUCT_SECRET_LEN]  = { 0 };
static uint16_t device_secret_len = 0;
uint16_t product_secret_len = 0;

#define SEQUENCE_STR "sequence"
#define DEVICE_NAME_STR "deviceName"
#define DEVICE_SECRET_STR "deviceSecret"
#define PRODUCT_KEY_STR "productKey"
#define PRODUCT_SECRET_STR "productSecret"

#define HI_SERVER_STR "Hi,Server"
#define HI_CLIENT_STR "Hi,Client"
#define OK_STR "OK"

#ifdef EN_AUTH_OFFLINE
#define AUTH_KEY_KV_PREFIX "auth_key_pairs"
#endif

bool g_dn_complete = false;
auth_t g_auth;

static void on_timeout(void *arg1, void *arg2)
{
    core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_2, BZ_ETIMEOUT);
}

static void ikm_init(ali_init_t const *p_init)
{
    if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == DEVICE_SECRET_LEN){
        device_secret_len = g_auth.device_secret_len;
        memcpy(device_secret, g_auth.secret, device_secret_len);
    }

    product_secret_len = p_init->product_secret.length;
    memcpy(product_secret, p_init->product_secret.p_data, product_secret_len);

    memcpy(g_auth.ikm + g_auth.ikm_len, p_init->product_secret.p_data, p_init->product_secret.length);
    g_auth.ikm_len += p_init->product_secret.length;

    g_auth.ikm[g_auth.ikm_len++] = ',';
}

ret_code_t auth_init(ali_init_t const *p_init, tx_func_t tx_func)
{
    int len;
    char tmp_secret[DEVICE_SECRET_LEN];
    ret_code_t ret = BZ_SUCCESS;

    len = sizeof(tmp_secret);
    memset(tmp_secret, 0, len);
    memset(&g_auth, 0, sizeof(auth_t));
    g_auth.state = AUTH_STATE_IDLE;
    g_auth.tx_func = tx_func;
    g_auth.dyn_update_device_secret = false;

    if(p_init->product_key.length == PRODUCT_KEY_LEN &&
         p_init->device_key.length != 0){
        memcpy(g_auth.product_key, p_init->product_key.p_data, PRODUCT_KEY_LEN);
        memcpy(g_auth.device_name, p_init->device_key.p_data, p_init->device_key.length);
        g_auth.device_name_len = p_init->device_key.length;
    }

    /*
     * Secret have 2 resources: internal KV storge and external initialization. Consider below conditions:
     * 1.secret per device, no DS in KV: need 1. update DS from external initialization.
     * 2.secret per device, have DS in KV:if 1.DS is the same choose either one. 2.DS is not the same, report err or choose one.
     * 3.secret per product, no DS in KV, : Doing nothing.
     * 4.secret per producet, have DS in KV:need 1. update dynamic secret flag, update from interval KV storage.
     */
    g_auth.device_secret_len = p_init->secret.length;
    memset(tmp_secret, 0, sizeof(tmp_secret));
    if(g_auth.device_secret_len == DEVICE_SECRET_LEN){
        if(auth_get_device_secret(&tmp_secret, &len) != 0 ){ //case 1.
            memcpy(g_auth.secret, p_init->secret.p_data, DEVICE_SECRET_LEN);
        } else if(memcmp(tmp_secret, g_auth.secret, g_auth.device_secret_len) == 0){//case 2.
                memcpy(g_auth.secret, p_init->secret.p_data, DEVICE_SECRET_LEN);
            } else{
                BREEZE_LOG_ERR("DS from KV not match user's input  %s\n", __func__);
                return BZ_EINVALIDPARAM;
            }
    } else if(g_auth.device_secret_len == 0){
        if(auth_get_device_secret(&tmp_secret, &len) == 0){ //case 4.
            g_auth.device_secret_len = len;
            memcpy(g_auth.secret, tmp_secret, len);
            g_auth.dyn_update_device_secret = true;
        } //case 3
    } else{
        BREEZE_LOG_ERR("Auth type not sec per product or per device.%s\n", __func__);
        return BZ_EINVALIDPARAM;//err case when DS length is not DEVICE_SECRET_LEN or 0
    }

    g_auth.key_len = p_init->device_key.length;
    if(g_auth.key_len > 0){
        memcpy(g_auth.key, p_init->device_key.p_data, g_auth.key_len);
    }
    ikm_init(p_init);
#ifdef EN_AUTH_OFFLINE
    auth_keys_init();
#endif
    ret = aos_timer_new_ext(&g_auth.timer, on_timeout, &g_auth, BZ_AUTH_TIMEOUT, 0, 0);
    return ret;
}

void auth_reset(void)
{
    g_auth.state = AUTH_STATE_IDLE;
    aos_timer_stop(&g_auth.timer);
}

void auth_rx_command(uint8_t cmd, uint8_t *p_data, uint16_t length)
{
    int32_t err_code;
    uint8_t rekey_rsp[] = {0x01, 0x02};
    char authkey[16]={0};
    char authid[4] = {0};

    if (length == 0 || (cmd & BZ_CMD_TYPE_MASK) != BZ_CMD_AUTH) {
        return;
    }
    
    switch (g_auth.state) {
        case AUTH_STATE_RAND_SENT:
#ifdef EN_AUTH_OFFLINE
            if(cmd == BZ_CMD_AUTH_REKEY){/*check whether auth-key according to auth-id*/
                 if(length != AUTH_ID_LEN) {
                      err_code = g_auth.tx_func(BZ_CMD_AUTH_REKEY_RSP, &rekey_rsp[1] , 1);
                      BREEZE_LOG_ERR("BZ auth rx re-key (%d)\n", err_code);
                }
                memcpy(authid, p_data, AUTH_ID_LEN);
                if(authkey_get(authid, authkey) == true){
                    g_dn_complete = true;
                    g_auth.state = AUTH_STATE_REQ_RECVD;
                    
                    /*1.update current auth key*/
                    transport_update_key(authkey);
                    /*2.response with "HI Client"*/
                    err_code = g_auth.tx_func(BZ_CMD_AUTH_RSP, HI_CLIENT_STR, strlen(HI_CLIENT_STR));
                    BREEZE_LOG_DEBUG("BZ auth rx offline key:(%d)\n", err_code);
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                        return;
                    }
                    err_code = aos_timer_start(&g_auth.timer);
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                        return;
                    }
                 } else{
                    err_code = g_auth.tx_func(BZ_CMD_AUTH_REKEY_RSP, &rekey_rsp[0] , 1);
                    if (err_code != BZ_SUCCESS) {
                        core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                        return;
                    }
                    /*back to re-auth state*/
                    auth_service_enabled();
                 }
            }
#endif
            if (cmd == BZ_CMD_AUTH_REQ &&
                memcmp(HI_SERVER_STR, p_data, MIN(length, strlen(HI_SERVER_STR))) == 0) {
                g_dn_complete = true;
                g_auth.state = AUTH_STATE_REQ_RECVD;
                err_code = g_auth.tx_func(BZ_CMD_AUTH_RSP, HI_CLIENT_STR, strlen(HI_CLIENT_STR));
                BREEZE_LOG_DEBUG("BZ auth rx HI SERVER:(%d)\n", err_code);
                if (err_code != BZ_SUCCESS) {
                    core_handle_err(ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                err_code = aos_timer_start(&g_auth.timer);
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
                BREEZE_LOG_DEBUG("BZ auth rx OK:\n");
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
        aos_timer_stop(&g_auth.timer);
    } else if (g_auth.state == AUTH_STATE_FAILED) {
        aos_timer_stop(&g_auth.timer);
        ble_disconnect(AIS_BT_REASON_REMOTE_USER_TERM_CONN);
    }
}

static void update_aes_key(bool use_device_key)
{
    uint8_t rand_backup[RANDOM_SEQ_LEN];
    SHA256_CTX context;
    uint8_t okm[SHA256_BLOCK_SIZE];
#ifdef EN_AUTH_OFFLINE
    uint8_t auth_id[AUTH_ID_LEN];
#endif

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

#ifdef EN_AUTH_OFFLINE
    if (use_device_key) {
        sec_sha256_init(&context);
        sec_sha256_update(&context, rand_backup, sizeof(rand_backup));
        sec_sha256_final(&context, okm);
        memcpy(auth_id, okm, AUTH_ID_LEN);
        authkey_set(auth_id, g_auth.okm);
    }
#endif
}

void auth_connected(void)
{
    int32_t err_code;

    err_code = aos_timer_start(&g_auth.timer);
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
    int32_t err_code;

    g_auth.state = AUTH_STATE_SVC_ENABLED;
    err_code = g_auth.tx_func(BZ_CMD_AUTH_RAND, g_auth.ikm + g_auth.ikm_len, RANDOM_SEQ_LEN);
    BREEZE_LOG_DEBUG("BZ auth: tx rand(%d)\n", err_code);
    if (err_code != BZ_SUCCESS) {
        core_handle_err(ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }
}

void auth_tx_done(void)
{
    int32_t err_code;

    if (g_auth.state == AUTH_STATE_SVC_ENABLED) {
        g_auth.state = AUTH_STATE_RAND_SENT;

        if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == DEVICE_SECRET_LEN){
            err_code = g_auth.tx_func(BZ_CMD_AUTH_KEY, g_auth.key, g_auth.key_len);
            BREEZE_LOG_DEBUG("BZ auth: tx DS (%d)\n", err_code);
            if (err_code != BZ_SUCCESS) {
                core_handle_err(ALI_ERROR_SRC_AUTH_SEND_KEY, err_code);
            }
            return;
        }
    } else if (g_auth.state == AUTH_STATE_RAND_SENT) {
        if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == DEVICE_SECRET_LEN){
            BREEZE_LOG_DEBUG("BZ auth: update AES-auth\n");
            update_aes_key(true);
        } else{
            return;
        }
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

ret_code_t auth_get_device_secret(uint8_t *p_secret, uint8_t *p_length)
{
    if(p_secret == NULL || p_length == NULL){
        return BZ_EINVALIDPARAM;
    }
    return aos_kv_get(DEVICE_SECRET_STR, p_secret, p_length);
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

    if(g_auth.dyn_update_device_secret == true || g_auth.device_secret_len == DEVICE_SECRET_LEN){
        sec_sha256_update(&context, DEVICE_SECRET_STR, strlen(DEVICE_SECRET_STR));
        sec_sha256_update(&context, g_auth.secret, DEVICE_SECRET_LEN);
    } else{
        sec_sha256_update(&context, PRODUCT_SECRET_STR, strlen(PRODUCT_SECRET_STR));
        sec_sha256_update(&context, product_secret, PRODUCT_SECRET_LEN);
    }

    sec_sha256_update(&context, PRODUCT_KEY_STR, strlen(PRODUCT_KEY_STR));
    sec_sha256_update(&context, g_auth.product_key, PRODUCT_KEY_LEN);

    sec_sha256_update(&context, SEQUENCE_STR, strlen(SEQUENCE_STR));
    sec_sha256_update(&context, &seq, sizeof(seq));

    sec_sha256_final(&context, full_sign);

    memcpy(sign, full_sign, 4);

    return 0;
}
#endif

#ifdef CONFIG_MODEL_SECURITY
bool get_auth_update_status(void)
{
    return g_auth.dyn_update_device_secret;
}

ret_code_t auth_secret_update_post_process(uint8_t* p_ds, uint16_t len)
{
    /*
     * 1.update secret bit in adv, this will do in restart adv logic, not here
     * 2.update IKM with product secret for secret per device
     * 3.update g_auth.dyn_update_device_secret flag in g_auth struct
     */
    g_auth.dyn_update_device_secret = true;
    device_secret_len = len;
    g_auth.device_secret_len = len;
    memcpy(device_secret, p_ds, len);
    memcpy(g_auth.secret, p_ds, len);

    BREEZE_LOG_INFO("Auth updated :per product ->device %d len(%d)\n",g_auth.dyn_update_device_secret, device_secret_len);
    return BZ_SUCCESS;
}

#endif


#ifdef EN_AUTH_OFFLINE
void auth_keys_init(void)
{
    auth_key_storage_t auth_keys;
    uint32_t len = sizeof(auth_keys);
    memset(&auth_keys, 0, len);
    if (aos_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) != 0){
        if(aos_kv_set(AUTH_KEY_KV_PREFIX, &auth_keys, &len, 1) != 0){
            BREEZE_LOG_ERR("BZ auth init keys \r\n");
            return;
        }
    }
}

bool authkey_set(uint8_t* authid, uint8_t* authkey)
{
    uint32_t index = 0;
    uint8_t tmp_authid[4] = {0};
    int32_t ret;
    auth_key_storage_t auth_keys;
    uint32_t len = sizeof(auth_keys);
    if(authid == NULL || authkey == NULL){
        return false;
    }
    memset(&auth_keys, 0, len);
    if (aos_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) == 0){
        index = auth_keys.index_to_update;
        memcpy(auth_keys.kv_pairs[index].auth_id, authid, AUTH_ID_LEN);
        memcpy(auth_keys.kv_pairs[index].auth_key, authkey, AUTH_KEY_LEN);
        auth_keys.index_to_update ++;
        if(index = MAX_AUTH_KEYS -1){
           auth_keys.index_to_update = 0;
        }
        ret = aos_kv_set(AUTH_KEY_KV_PREFIX, &auth_keys, &len, 1);
        BREEZE_LOG_ERR("BZ auth keys update %s \r\n", ret ? "fail": "success");
        return true;
    }
    return false;
}

bool authkey_get(uint8_t* authid, uint8_t* authkey)
{
    uint32_t index = 0;
    uint8_t tmp_authid[4] = {0};
    int32_t ret;
    auth_key_storage_t auth_keys;
    uint32_t len = sizeof(auth_keys);
    if(authid == NULL || authkey == NULL){
        return false;
    }
    if (aos_kv_get(AUTH_KEY_KV_PREFIX, &auth_keys, &len) == 0){
        for(; index < MAX_AUTH_KEYS; ++index){
             if(!memcmp(auth_keys.kv_pairs[index].auth_id, authid, AUTH_ID_LEN)){
                memcpy(authkey, auth_keys.kv_pairs[index].auth_key, AUTH_KEY_LEN);
                return true;
             }
        }
        if(index == MAX_AUTH_KEYS){
            return false;
        }
    }
    return false;
}
#endif
