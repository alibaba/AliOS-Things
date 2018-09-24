/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "ali_auth.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "sha256.h"
#include "ali_common.h"
#include "breeze_hal_os.h"

#ifdef TEST_VECTORS
static uint8_t const
  m_tv_rand[ALI_AUTH_PRS_LEN] /**< Injected test vector: fixed random number. */
  = "\x28\x1f\x2e\x4e\xef\xbd\x12\xd3\xb9\xf7\xee\x5a\x17\x40\xcc\x2b";
#endif

static uint8_t  device_secret[ALI_AUTH_SECRET_LEN_MAX] = { 0 };
static uint8_t  product_secret[ALI_AUTH_PKEY_LEN_MAX]  = { 0 };
static uint16_t device_secret_len                      = 0;
static uint16_t product_secret_len                     = 0;

static uint8_t const m_auth_req[9] =
  "Hi,Server"; /**< Authentication request from central. */
static uint8_t const m_auth_rsp[9] =
  "Hi,Client"; /**< Authentication response from peripheral. */
static uint8_t const m_auth_cfm[2] =
  "OK"; /**< Authentication confirm from central. */

static uint8_t const m_v2sig_p1[8] =
  "clientId"; /**< V2 network signature fixed part 1. */
static uint8_t const m_v2sig_p2[10] =
  "deviceName"; /**< V2 network signature fixed part 2. */
static uint8_t const m_v2sig_p3[10] =
  "productKey"; /**< V2 network signature fixed part 3. */
static uint8_t const m_v2sig_p4[12] =
  "deviceSecret"; /**< V2 network signature fixed part 4. */

static void update_auth_key(ali_auth_t *p_auth, bool use_device_key);

ali_auth_event_t auth_evt;

/**@brief Notify error to higher layer. */
static void notify_error(ali_auth_t *p_auth, uint32_t src, uint32_t err_code)
{
    /* send event to higher layer. */
    auth_evt.data.error.source   = src;
    auth_evt.data.error.err_code = err_code;
    os_post_event(OS_EV_AUTH, OS_EV_CODE_AUTH_ERROR, (unsigned long)&auth_evt);

    /* clean up */
    ali_auth_reset(p_auth);
}

/**@brief Timeout handler. */
static void on_timeout_helper(ali_auth_t *p_auth)
{
    notify_error(p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_2, BREEZE_ERROR_TIMEOUT);
}

static void on_timeout(void *arg1, void *arg2)
{
    ali_auth_t *p_auth = (ali_auth_t *)arg2;
    on_timeout_helper(p_auth);
}

/**@brief Notify Authentication result to higher layer. */
static void notify_result(ali_auth_t *p_auth)
{
    int32_t          err_code;

    /* send event to higher layer. */
    auth_evt.data.auth_done.result = (p_auth->state == ALI_AUTH_STATE_DONE);
    os_post_event(OS_EV_AUTH, OS_EV_CODE_AUTH_DONE, (unsigned long)&auth_evt);

    /* stop all related timers */
    if (p_auth->timeout != 0) {
        err_code = os_timer_stop(&p_auth->timer);
        VERIFY_SUCCESS_VOID(err_code);
    }
}

/**@brief Notify the newly-generated key to higher layer. */
static void notify_key(ali_auth_t *p_auth)
{
    /* send event to higher layer. */
    auth_evt.data.new_key.p_sec_key = p_auth->okm;
    os_post_event(OS_EV_AUTH, OS_EV_CODE_AUTH_KEY_UPDATE, (unsigned long)&auth_evt);
}

/**@brief Generate random number using SD calls. */
static void sd_rand(ali_auth_t *p_auth)
{
#ifndef TEST_VECTORS
    uint8_t  bytes_available = 0;
    uint32_t seed            = os_now_ms();
    uint8_t  byte[4 + 1];
    uint32_t result;
    uint16_t bytes_copy;

    srand((unsigned int)seed);
    result = rand();

    while (bytes_available < ALI_AUTH_PRS_LEN) {
        seed += result;
        seed = seed % 9999;
        snprintf((char *)byte, sizeof(byte), "%04d", seed);
        bytes_copy = ALI_AUTH_PRS_LEN - bytes_available;
        bytes_copy = (bytes_copy > 4) ? 4 : bytes_copy;
        memcpy(p_auth->ikm + p_auth->ikm_len + bytes_available, byte,
               bytes_copy);
        bytes_available += bytes_copy;
    }
#else
    memcpy(p_auth->ikm + p_auth->ikm_len, m_tv_rand, ALI_AUTH_PRS_LEN);
#endif
}

/**@brief Key derivation function. */
static void kdf(ali_auth_t *p_auth)
{
    // Key derivation by HASH-SHA256()
    SHA256_CTX context;
    uint8_t    okm[SHA256_BLOCK_SIZE];

    sha256_init(&context);
    sha256_update(&context, p_auth->ikm, p_auth->ikm_len + ALI_AUTH_PRS_LEN);
    sha256_final(&context, okm);

    /* Ali-SDK specification v1.0.4, ch.4.3. */
    memcpy(p_auth->okm, okm, ALI_AUTH_SEC_KEY_LEN);
}

/**@brief Function for verifying initialization parameters. */
static ret_code_t verify_init_params(ali_auth_init_t const *p_init)
{
    VERIFY_PARAM_NOT_NULL(p_init);

    if (!p_init->feature_enable) {
        return BREEZE_SUCCESS;
    }

    VERIFY_PARAM_NOT_NULL(p_init->tx_func);
    VERIFY_PARAM_NOT_NULL(p_init->p_mac);
    VERIFY_PARAM_NOT_NULL(p_init->p_secret);

    if (p_init->pkey_len == 0 && p_init->dkey_len == 0) {
        return BREEZE_ERROR_INVALID_LENGTH; // no product or device key available.
    }

    if (p_init->pkey_len != 0) {
        VERIFY_PARAM_NOT_NULL(p_init->p_pkey);
    }

    if (p_init->dkey_len != 0) {
        VERIFY_PARAM_NOT_NULL(p_init->p_dkey);
        if (p_init->dkey_len > ALI_AUTH_DKEY_LEN_MAX) {
            return BREEZE_ERROR_INVALID_LENGTH;
        }
    }

    if ((p_init->pkey_len != ALI_AUTH_PKEY_V1_LEN || p_init->dkey_len != 0 ||
         p_init->secret_len != ALI_AUTH_SECRET_V1P_LEN) &&
        (p_init->pkey_len != 0 || p_init->dkey_len != ALI_AUTH_DKEY_V1_LEN ||
         p_init->secret_len != ALI_AUTH_SECRET_V1D_LEN) &&
        (p_init->pkey_len != ALI_AUTH_PKEY_V2_LEN || p_init->dkey_len == 0 ||
         (p_init->secret_len != ALI_AUTH_SECRET_V2P_LEN &&
          p_init->secret_len != ALI_AUTH_SECRET_V2D_LEN))) {
        return BREEZE_ERROR_INVALID_LENGTH;
    }

    return BREEZE_SUCCESS;
}

/**@brief Convert a hex digit to ASCII character. */
static uint8_t hex2ascii(uint8_t digit)
{
    uint8_t val;

    if (digit <= 9) {
        val = digit - 0x0 + '0';
    } else {
        val = digit - 0xA + 'A';
    }

    return val;
}

/**@brief Initialize V2 network signature (see specification v1.0.5, ch. 5.6.1).
 */
static void v2_network_signature_init(ali_auth_t            *p_auth,
                                      ali_auth_init_t const *p_init)
{
    uint8_t    str_id[8], n;
    uint32_t   model_id;
    SHA256_CTX context;

    sha256_init(&context);

    sha256_update(&context, m_v2sig_p1, sizeof(m_v2sig_p1)); /* "clientId" */

    /* Convert 32-bit Model ID to ASCII string. */
    model_id = p_init->model_id;
    for (n = 0; n < 8; n++) {
        str_id[n] = hex2ascii((model_id >> 28));
        model_id <<= 4;
    }
    sha256_update(&context, str_id,
                  sizeof(str_id)); /* model_id from from init parameters. */

    sha256_update(&context, m_v2sig_p2, sizeof(m_v2sig_p2)); /* "deviceName" */
    sha256_update(&context, p_init->p_dkey,
                  p_init->dkey_len); /* Device key from init parameters. */

    sha256_update(&context, m_v2sig_p3, sizeof(m_v2sig_p3)); /* "productKey" */
    sha256_update(&context, p_init->p_pkey,
                  ALI_AUTH_PKEY_V2_LEN); /* Product key from init parameters. */

    sha256_update(&context, m_v2sig_p4,
                  sizeof(m_v2sig_p4)); /* "deviceSecret" */
    sha256_update(&context, p_init->p_secret,
                  p_init->secret_len); /* Secret from init parameters. */

    sha256_final(&context, p_auth->v2_network.v2_signature);
}

/**@brief Initialize IKM (see specification v1.0.4, ch. 4.3) */
static void ikm_init(ali_auth_t *p_auth, ali_auth_init_t const *p_init)
{
    /* Save device secret info for later use */
    device_secret_len = p_init->secret_len;
    memcpy(device_secret, p_init->p_secret, device_secret_len);
    /* Save product secret info for later use */
    product_secret_len = p_init->product_secret_len;
    memcpy(product_secret, p_init->p_product_secret, product_secret_len);

    memcpy(p_auth->ikm + p_auth->ikm_len, p_init->p_product_secret,
           p_init->product_secret_len);
    p_auth->ikm_len += p_init->product_secret_len;

    p_auth->ikm[p_auth->ikm_len++] = ',';

#ifndef CONFIG_ENHANCED_AUTH
    memcpy(p_auth->ikm + p_auth->ikm_len, p_init->p_mac, ALI_AUTH_MAC_LEN);
    p_auth->ikm_len += ALI_AUTH_MAC_LEN;

    p_auth->ikm[p_auth->ikm_len++] = ',';
#endif
}

ret_code_t ali_auth_init(ali_auth_t *p_auth, ali_auth_init_t const *p_init)
{
    ret_code_t ret = BREEZE_SUCCESS;
    bool       secret_per_device;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    ret = verify_init_params(p_init);
    VERIFY_SUCCESS(ret);

    /* Initialize context */
    memset(p_auth, 0, sizeof(ali_auth_t));
    p_auth->feature_enable    = p_init->feature_enable;
    p_auth->state             = ALI_AUTH_STATE_IDLE;
    p_auth->timeout           = p_init->timeout;
    p_auth->p_evt_context     = p_init->p_evt_context;
    p_auth->tx_func           = p_init->tx_func;
    p_auth->p_tx_func_context = p_init->p_tx_func_context;

    if (!p_auth->feature_enable) {
        return ret;
    }

    /* check if V1 or V2 network. */
    if (p_init->pkey_len == ALI_AUTH_PKEY_V2_LEN && p_init->dkey_len != 0 &&
        (p_init->secret_len == ALI_AUTH_SECRET_V2P_LEN ||
         p_init->secret_len == ALI_AUTH_SECRET_V2D_LEN)) {
        /* V2 Network. */
        secret_per_device = (p_init->secret_len == ALI_AUTH_SECRET_V2D_LEN);
        memcpy(p_auth->v2_network.product_key, p_init->p_pkey,
               ALI_AUTH_PKEY_V2_LEN);
        memcpy(p_auth->v2_network.secret, p_init->p_secret,
               ALI_AUTH_SECRET_V2D_LEN);
        memcpy(p_auth->v2_network.device_name, p_init->p_dkey,
               p_init->dkey_len);
        p_auth->v2_network.device_name_len = p_init->dkey_len;

        v2_network_signature_init(p_auth, p_init);
    } else {
        /* V1 Network. */
        secret_per_device = (p_init->dkey_len != 0);
    }

    /* Backup product or device key. */
    if (secret_per_device) {
        memcpy(p_auth->key, p_init->p_dkey, p_init->dkey_len);
        p_auth->key_len = p_init->dkey_len;
    } else {
        memcpy(p_auth->key, p_init->p_pkey, p_init->pkey_len);
        p_auth->key_len = p_init->pkey_len;
    }

    /* Intialize IKM. */
    ikm_init(p_auth, p_init);

    /* Initialize Tx and Rx timeout timers. */
    if (p_auth->timeout != 0) {
        ret = os_timer_new(&p_auth->timer, on_timeout, p_auth, p_auth->timeout);
        VERIFY_SUCCESS(ret);
    }

    return ret;
}

void ali_auth_reset(ali_auth_t *p_auth)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Reset state machine. */
    p_auth->state = ALI_AUTH_STATE_IDLE;

    /* stop all related timers */
    if (p_auth->timeout != 0 && p_auth->feature_enable) {
        err_code = os_timer_stop(&p_auth->timer);
        VERIFY_SUCCESS_VOID(err_code);
    }
}

bool g_dn_complete = false;
void ali_auth_on_command(ali_auth_t *p_auth, uint8_t cmd, uint8_t *p_data,
                         uint16_t length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0 || (cmd & ALI_CMD_TYPE_MASK) != ALI_CMD_TYPE_AUTH) {
        return;
    }

    switch (p_auth->state) {
        case ALI_AUTH_STATE_RAND_SENT: {
            g_dn_complete = true;
            if (cmd == ALI_CMD_AUTH_REQ &&
                memcmp(m_auth_req, p_data, MIN(length, sizeof(m_auth_req))) ==
                  0) {

                p_auth->state = ALI_AUTH_STATE_REQ_RECVD;
                err_code =
                  p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_AUTH_RSP,
                                  (uint8_t *)m_auth_rsp, sizeof(m_auth_rsp));

                if (err_code != BREEZE_SUCCESS) {
                    notify_error(p_auth, ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                /* Start procedure timeout timer */
                if (p_auth->timeout != 0) {
                    err_code = os_timer_start(&p_auth->timer);
                    if (err_code != BREEZE_SUCCESS) {
                        notify_error(p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_1,
                                     err_code);
                        return;
                    }
                }
            } else {
                /* Authentication failed. */
                p_auth->state = ALI_AUTH_STATE_FAILED;
                notify_result(p_auth);
            }
        } break;

        case ALI_AUTH_STATE_REQ_RECVD:
            if (cmd == ALI_CMD_AUTH_CFM &&
                memcmp(m_auth_cfm, p_data, MIN(length, sizeof(m_auth_cfm))) ==
                  0) {
                p_auth->state = ALI_AUTH_STATE_DONE;
            } else {
                p_auth->state = ALI_AUTH_STATE_FAILED;
            }

            notify_result(p_auth);
            break;

        default:

            /* Send error to central. */
            err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_ERROR,
                                       NULL, 0);
            if (err_code != BREEZE_SUCCESS) {
                notify_error(p_auth, ALI_ERROR_SRC_AUTH_SEND_ERROR, err_code);
                return;
            }
            break;
    }
}

void ali_auth_on_connected(ali_auth_t *p_auth)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Check if feature is enabled. */
    if (!p_auth->feature_enable) {
        return;
    }

    /* Start procedure timeout timer */
    if (p_auth->timeout != 0) {
        err_code = os_timer_start(&p_auth->timer);
        if (err_code != BREEZE_SUCCESS) {
            notify_error(p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_0, err_code);
            return;
        }
    }

    p_auth->state = ALI_AUTH_STATE_CONNECTED;
    sd_rand(p_auth);
    update_auth_key(p_auth, false);
}

void ali_auth_on_enable_service(ali_auth_t *p_auth)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Check if feature is enabled. */
    if (!p_auth->feature_enable) {
        return;
    }

    p_auth->state = ALI_AUTH_STATE_SVC_ENABLED;

    err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_AUTH_RAND,
                               p_auth->ikm + p_auth->ikm_len, ALI_AUTH_PRS_LEN);
    if (err_code != BREEZE_SUCCESS) {
        notify_error(p_auth, ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }
}

static void update_auth_key(ali_auth_t *p_auth, bool use_device_key)
{
    uint8_t rand_backup[ALI_AUTH_PRS_LEN];

    memcpy(rand_backup, p_auth->ikm + p_auth->ikm_len, ALI_AUTH_PRS_LEN);
    p_auth->ikm_len = 0;

    if (use_device_key) {
        memcpy(p_auth->ikm + p_auth->ikm_len, device_secret, device_secret_len);
        p_auth->ikm_len += device_secret_len;
    } else {
        memcpy(p_auth->ikm + p_auth->ikm_len, product_secret,
               product_secret_len);
        p_auth->ikm_len += product_secret_len;
    }

    p_auth->ikm[p_auth->ikm_len++] = ',';
    memcpy(p_auth->ikm + p_auth->ikm_len, rand_backup, sizeof(rand_backup));

    kdf(p_auth);
    notify_key(p_auth);
}

void ali_auth_on_tx_complete(ali_auth_t *p_auth)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Check if feature is enabled. */
    if (!p_auth->feature_enable) {
        return;
    }

    /* Check if service is enabled and it is sending random sequence. */
    if (p_auth->state == ALI_AUTH_STATE_SVC_ENABLED) {
#ifdef CONFIG_MODEL_SECURITY
        p_auth->state = ALI_AUTH_STATE_RAND_SENT;
        return;
#else
        p_auth->state = ALI_AUTH_STATE_RAND_SENT;
        err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_AUTH_KEY,
                                   p_auth->key, p_auth->key_len);
        if (err_code != BREEZE_SUCCESS) {
            notify_error(p_auth, ALI_ERROR_SRC_AUTH_SEND_KEY, err_code);
            return;
        }
        return;
#endif
    } else if (p_auth->state == ALI_AUTH_STATE_RAND_SENT) {
#ifdef CONFIG_MODEL_SECURITY
        return;
#else
        /* Update AES key after device name sent */
        update_auth_key(p_auth, true);
#endif
    }
}

ret_code_t ali_auth_get_device_name(ali_auth_t *p_auth,
                                    uint8_t **pp_device_name, uint8_t *p_length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    VERIFY_PARAM_NOT_NULL(pp_device_name);
    VERIFY_PARAM_NOT_NULL(p_length);

    /* check for V2 network */
    if (p_auth->v2_network.device_name_len > 0) {
        *pp_device_name = p_auth->v2_network.device_name;
        *p_length       = p_auth->v2_network.device_name_len;

        err_code = BREEZE_SUCCESS;
    } else {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    }

    return err_code;
}

ret_code_t ali_auth_get_product_key(ali_auth_t *p_auth, uint8_t **pp_prod_key,
                                    uint8_t *p_length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    VERIFY_PARAM_NOT_NULL(pp_prod_key);
    VERIFY_PARAM_NOT_NULL(p_length);

    /* check for V2 network */
    if (p_auth->v2_network.device_name_len > 0) {
        *pp_prod_key = p_auth->v2_network.product_key;
        *p_length    = ALI_AUTH_PKEY_V2_LEN;

        err_code = BREEZE_SUCCESS;
    } else {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    }

    return err_code;
}

ret_code_t ali_auth_get_secret(ali_auth_t *p_auth, uint8_t **pp_secret,
                               uint8_t *p_length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    VERIFY_PARAM_NOT_NULL(pp_secret);
    VERIFY_PARAM_NOT_NULL(p_length);

    /* check for V2 network */
    if (p_auth->v2_network.device_name_len > 0) {
        *pp_secret = p_auth->v2_network.secret;
        *p_length  = ALI_AUTH_V2_SECRET_LEN;

        err_code = BREEZE_SUCCESS;
    } else {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    }

    return err_code;
}

ret_code_t ali_auth_get_v2_signature(ali_auth_t *p_auth, uint8_t **pp_signature,
                                     uint8_t *p_length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    VERIFY_PARAM_NOT_NULL(pp_signature);
    VERIFY_PARAM_NOT_NULL(p_length);

    /* check for V2 network */
    if (p_auth->v2_network.device_name_len > 0) {
        *pp_signature = p_auth->v2_network.v2_signature;
        *p_length     = sizeof(p_auth->v2_network.v2_signature);

        err_code = BREEZE_SUCCESS;
    } else {
        err_code = BREEZE_ERROR_NOT_SUPPORTED;
    }

    return err_code;
}
