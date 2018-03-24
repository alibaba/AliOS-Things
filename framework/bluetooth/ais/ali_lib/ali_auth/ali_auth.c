/*
 * Copyright (c) 2015 - 2017, Nordic Semiconductor ASA
 * 
 * All rights reserved.
 * 
 * Use in source and binary forms, redistribution in binary form only, with
 * or without modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 * 
 * 2. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 * 
 * 3. This software, with or without modification, must only be used with a Nordic
 *    Semiconductor ASA integrated circuit.
 * 
 * 4. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 * 
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
 * TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "ali_auth.h"
#include "ali_error_src.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
#include "sha256.h"
#include "ali_common.h"
#include <aos/aos.h>

#define MOD "ble_auth"

#ifdef TEST_VECTORS
static uint8_t const m_tv_rand[ALI_AUTH_PRS_LEN]    /**< Injected test vector: fixed random number. */
    = "\x28\x1f\x2e\x4e\xef\xbd\x12\xd3\xb9\xf7\xee\x5a\x17\x40\xcc\x2b";
#endif

static uint8_t const m_auth_req[9] = "Hi,Server";   /**< Authentication request from central. */
static uint8_t const m_auth_rsp[9] = "Hi,Client";   /**< Authentication response from peripheral. */
static uint8_t const m_auth_cfm[2] = "OK";          /**< Authentication confirm from central. */



/**@brief Notify error to higher layer. */
static void notify_error (ali_auth_t * p_auth, uint32_t src, uint32_t err_code)
{
    ali_auth_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_AUTH_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_auth->event_handler(p_auth->p_evt_context, &evt);

    /* clean up */
    ali_auth_reset(p_auth);
}


/**@brief Timeout handler. */
static void on_timeout_helper (ali_auth_t * p_auth)
{
    notify_error (p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_2, NRF_ERROR_TIMEOUT);
}


static void on_timeout (void * arg1, void * arg2)
{
    ali_auth_t *p_auth = (ali_auth_t *)arg2;
    on_timeout_helper(p_auth);
}


/**@brief Notify Authentication result to higher layer. */
static void notify_result(ali_auth_t * p_auth)
{
    int32_t err_code;
    ali_auth_event_t evt;

    /* send event to higher layer. */
    evt.type                  = ALI_AUTH_EVT_DONE;
    evt.data.auth_done.result = (p_auth->state == ALI_AUTH_STATE_DONE);
    p_auth->event_handler(p_auth->p_evt_context, &evt);

    /* stop all related timers */
    if (p_auth->timeout != 0)
    {
        err_code = aos_timer_stop(&p_auth->timer);
        if (err_code != 0) LOGE("auth", "aos_timer_stop failed.");
    }
}


/**@brief Notify the newly-generated key to higher layer. */
static void notify_key(ali_auth_t * p_auth)
{
    ali_auth_event_t evt;

    /* send event to higher layer. */
    evt.type               = ALI_AUTH_EVT_KEY_UPDATE;
    evt.data.new_key.p_key = p_auth->okm;
    p_auth->event_handler(p_auth->p_evt_context, &evt);
}


/**@brief Generate random number using SD calls. */
static void sd_rand (ali_auth_t * p_auth)
{
#ifndef TEST_VECTORS
    uint8_t bytes_available = 0;
    uint32_t seed = aos_now_ms();
    uint8_t byte[4 + 1];
    uint32_t result = result;
    uint16_t bytes_copy;

    while (bytes_available < ALI_AUTH_PRS_LEN) {
        seed += result;
        seed = seed % 9999;
        snprintf((char *)byte, sizeof(byte), "%04d", seed);
        bytes_copy = ALI_AUTH_PRS_LEN - bytes_available;
        bytes_copy = (bytes_copy > 4)? 4: bytes_copy;
        memcpy(p_auth->ikm + p_auth->ikm_len + bytes_available, byte, bytes_copy);
        bytes_available += bytes_copy;
    }
#else
    memcpy(p_auth->ikm + p_auth->ikm_len, m_tv_rand, ALI_AUTH_PRS_LEN);
#endif
}

/**@brief Key derivation function. */
static void kdf (ali_auth_t * p_auth)
{
    // Key derivation by HASH-SHA256()
    SHA256_CTX context;
    uint8_t okm[SHA256_BLOCK_SIZE];

    sha256_init(&context);
    sha256_update(&context, p_auth->ikm, p_auth->ikm_len + ALI_AUTH_PRS_LEN);
    sha256_final(&context, okm);

    /* Ali-SDK specification v1.0.4, ch.4.3. */
    memcpy(p_auth->okm, okm, ALI_AUTH_KEY_LEN);
}


ret_code_t ali_auth_init(ali_auth_t * p_auth, ali_auth_init_t const * p_init)
{
    ret_code_t ret = NRF_SUCCESS;

    LOGD(MOD, "ali_auth_init entry.");

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_auth);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func);
    VERIFY_PARAM_NOT_NULL(p_init->p_mac);
    VERIFY_PARAM_NOT_NULL(p_init->p_secret);
    if (p_init->secret_len == 0 || p_init->secret_len > ALI_AUTH_SECRET_LEN_MAX)
    {
        return NRF_ERROR_INVALID_PARAM;
    }

    /* Initialize context */
    memset(p_auth, 0, sizeof(ali_auth_t));
    p_auth->feature_enable    = p_init->feature_enable;
    p_auth->state             = ALI_AUTH_STATE_IDLE;
    p_auth->timeout           = p_init->timeout;
    p_auth->event_handler     = p_init->event_handler;
    p_auth->p_evt_context     = p_init->p_evt_context;
    p_auth->tx_func           = p_init->tx_func;
    p_auth->p_tx_func_context = p_init->p_tx_func_context;

    /* Intialize IKM. */
    memcpy(p_auth->ikm + p_auth->ikm_len, p_init->p_secret, p_init->secret_len);
    p_auth->ikm_len += p_init->secret_len;

    p_auth->ikm[p_auth->ikm_len++] = ',';

    memcpy(p_auth->ikm + p_auth->ikm_len, p_init->p_mac, ALI_AUTH_MAC_LEN);
    p_auth->ikm_len += ALI_AUTH_MAC_LEN;

    p_auth->ikm[p_auth->ikm_len++] = ',';

    /* Initialize Tx and Rx timeout timers. */
    if (p_auth->timeout != 0)
    {
        int err;
        err = aos_timer_new_ext(&p_auth->timer, on_timeout, p_auth, p_auth->timeout, 0, 0);
        VERIFY_SUCCESS(ret);
    }

    LOGD(MOD, "ali_auth_init exit.");

    return ret;
}


void ali_auth_reset(ali_auth_t * p_auth)
{
    int err;

    /* check parameters */
    if (p_auth == NULL) assert(1);

    /* Reset state machine. */
    p_auth->state = ALI_AUTH_STATE_IDLE;

    /* stop all related timers */
    if (p_auth->timeout != 0 && p_auth->feature_enable)
    {
        err = aos_timer_stop(&p_auth->timer);
        if (err) LOGE("auth", "aos_timer_stop failed in %s", __func__);
    }
}

#ifndef MIN
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#endif

void ali_auth_on_command(ali_auth_t * p_auth, uint8_t cmd, uint8_t * p_data, uint16_t length)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0
        || (cmd & ALI_CMD_TYPE_MASK) != ALI_CMD_TYPE_AUTH)
    {
        return;
    }

    switch (p_auth->state)
    {
        case ALI_AUTH_STATE_SVC_ENABLED:
            if (cmd == ALI_CMD_AUTH_REQ
                && memcmp(m_auth_req, p_data, MIN(length, sizeof(m_auth_req))) == 0)
            {
                p_auth->state = ALI_AUTH_STATE_REQ_RECVD;
                err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_AUTH_RSP,
                                           (uint8_t *)m_auth_rsp, sizeof(m_auth_rsp));
                if (err_code != NRF_SUCCESS)
                {
                    notify_error(p_auth, ALI_ERROR_SRC_AUTH_SEND_RSP, err_code);
                    return;
                }

                /* Start procedure timeout timer */
                if (p_auth->timeout != 0)
                {
                    int err;
                    err = aos_timer_start(&p_auth->timer);
                    if (err)
                    {
                        notify_error(p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_1, err_code);
                        return;
                    }
                }
            }
            else
            {
                /* Authentication failed. */
                p_auth->state = ALI_AUTH_STATE_FAILED;
                notify_result(p_auth);
            }
            break;

        case ALI_AUTH_STATE_REQ_RECVD:
            if (cmd == ALI_CMD_AUTH_CFM
                && memcmp(m_auth_cfm, p_data, MIN(length, sizeof(m_auth_cfm))) == 0)
            {
                p_auth->state = ALI_AUTH_STATE_DONE;
            }
            else
            {
                p_auth->state = ALI_AUTH_STATE_FAILED;
            }

            notify_result(p_auth);
            break;

        default:
            /* Send error to central. */
            err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_ERROR, NULL, 0);
            if (err_code != NRF_SUCCESS)
            {
                notify_error(p_auth, ALI_ERROR_SRC_AUTH_SEND_ERROR, err_code);
                return;
            }
            break;
    }
}


void ali_auth_on_connected(ali_auth_t * p_auth)
{
    int err;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Check if feature is enabled. */
    if (!p_auth->feature_enable)
    {
        return;
    }

    /* Start procedure timeout timer */
    if (p_auth->timeout != 0)
    {
        err = aos_timer_start(&p_auth->timer);
        if (err)
        {
            notify_error(p_auth, ALI_ERROR_SRC_AUTH_PROC_TIMER_0, err);
            return;
        }
    }

    p_auth->state = ALI_AUTH_STATE_CONNECTED;
    sd_rand(p_auth);
    kdf(p_auth);

    notify_key(p_auth);
}


void ali_auth_on_enable_service(ali_auth_t * p_auth)
{
    uint32_t err_code;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_auth);

    /* Check if feature is enabled. */
    if (!p_auth->feature_enable)
    {
        return;
    }

    p_auth->state = ALI_AUTH_STATE_SVC_ENABLED;

    err_code = p_auth->tx_func(p_auth->p_tx_func_context, ALI_CMD_AUTH_RAND,
                               p_auth->ikm + p_auth->ikm_len, ALI_AUTH_PRS_LEN);
    if (err_code != NRF_SUCCESS)
    {
        notify_error(p_auth, ALI_ERROR_SRC_AUTH_SVC_ENABLED, err_code);
        return;
    }

}


