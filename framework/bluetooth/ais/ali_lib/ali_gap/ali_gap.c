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
#include "ali_gap.h"
#include "ali_error_src.h"
#include <stdio.h>                  /* Standard input/output definitions */
#include <string.h>                 /* String function definitions */
#include <stdbool.h>
//#include "nordic_common.h"
//#include "sdk_macros.h"
//#include "nrf_soc.h"
#include <aos/aos.h>

#define CHECK_PAYLOAD_FORMAT    0   /**< 0: Do not check payload format.  */

#define MOD "ble_gap"

/**@brief Notify error to higher layer. */
static void notify_error (ali_gap_t * p_gap, uint32_t src, uint32_t err_code)
{
    ali_gap_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_GAP_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Notify TX_DONE event to higher layer. */
static void notify_tx_done (ali_gap_t * p_gap)
{
    ali_gap_event_t evt;

    /* send event to higher layer. */
    evt.type = ALI_GAP_EVT_TX_DONE;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Notify received data to higher layer. */
static void notify_data(ali_gap_t * p_gap, uint8_t cmd, uint8_t * data, uint16_t len)
{
    ali_gap_event_t evt;

    // Assumption: only commands @ref ALI_CMD_CTRL cnd @ref ALI_CMD_QUERY can reach here.

    /* send event to higher layer. */
    evt.type                = (cmd == ALI_CMD_QUERY)? ALI_GAP_EVT_ON_QUERY: ALI_GAP_EVT_ON_CTRL;
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Try parsing message and check integrity by the format. */
static bool try_parse (uint8_t * data, uint16_t len)
{
#if CHECK_PAYLOAD_FORMAT    // On-site: removed chapter 8 from specification v1.0.4 
    uint16_t offset = 0, next_offset;
    uint8_t  attr_len;

    /* Parse message according to Ali-SDK specification v1.0.4 ch. 8.1 */
    do
    {
        if ((attr_len = data[offset + 2]) == 0)
        {
            return false;   // zero length attribute
        }

        next_offset = offset + 3 + attr_len;
        if (len == next_offset)
        {
            break;  // correct payload
        }
        else if (len < next_offset)
        {
            return false;
        }
        else
        {
            offset = next_offset;
        }
    } while (1) ;
#endif
    return true;
}


/**@brief Core function for Tx. */
static ret_code_t send_msg(ali_gap_t * p_gap, uint8_t * p_data, uint16_t length, ali_gap_tx_func_t tx_func)
{
    uint32_t err_code = NRF_SUCCESS;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_data);
    if (length == 0)
    {
        return NRF_ERROR_DATA_SIZE;
    }

    /* Check if there is any on-going Tx-process. */
    if (p_gap->tx_active)
    {
        return NRF_ERROR_BUSY;
    }

    if (try_parse(p_data, length))
    {
        err_code = tx_func(p_gap->p_tx_func_context, p_gap->tx_cmd, p_data, length);
#if 0
        if (err_code == NRF_SUCCESS)
        {
            p_gap->tx_active = true;
        }
#endif
    }
    else
    {
        err_code = NRF_ERROR_INVALID_DATA;
    }

    return err_code;
}


ret_code_t ali_gap_init(ali_gap_t * p_gap, ali_gap_init_t const * p_init)
{
    LOGD(MOD, "ali_gap_init entry.");

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_gap);
    VERIFY_PARAM_NOT_NULL(p_init);
    VERIFY_PARAM_NOT_NULL(p_init->event_handler);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func_notify);
    VERIFY_PARAM_NOT_NULL(p_init->tx_func_indicate);

    /* Initialize context */
    memset(p_gap, 0, sizeof(ali_gap_t));
    p_gap->event_handler     = p_init->event_handler;
    p_gap->p_evt_context     = p_init->p_evt_context;
    p_gap->tx_func_notify    = p_init->tx_func_notify;
    p_gap->tx_func_indicate  = p_init->tx_func_indicate;
    p_gap->p_tx_func_context = p_init->p_tx_func_context;
    p_gap->tx_cmd            = ALI_CMD_STATUS;

    LOGD(MOD, "ali_gap_init exit.");

    return NRF_SUCCESS;
}


void ali_gap_reset(ali_gap_t * p_gap)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);

    /* Reset state machine. */
    p_gap->tx_active = false;
    p_gap->tx_cmd    = ALI_CMD_STATUS;
}


ret_code_t ali_gap_send_notify(ali_gap_t * p_gap, uint8_t * p_data, uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_gap);
    return send_msg(p_gap, p_data, length, p_gap->tx_func_notify);
}


ret_code_t ali_gap_send_indicate(ali_gap_t * p_gap, uint8_t * p_data, uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_gap);
    return send_msg(p_gap, p_data, length, p_gap->tx_func_indicate);
}


void ali_gap_on_command(ali_gap_t * p_gap, uint8_t cmd, uint8_t * p_data, uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0
        || (cmd != ALI_CMD_CTRL && cmd != ALI_CMD_QUERY))
    {
        return;
    }

    if (try_parse(p_data, length))
    {
        p_gap->tx_cmd = (cmd == ALI_CMD_QUERY)? ALI_CMD_REPLY: ALI_CMD_STATUS;
        notify_data(p_gap, cmd, p_data, length);
    }
    else
    {
        notify_error(p_gap, ALI_ERROR_SRC_GAP_CMD_RECEIVED, NRF_ERROR_INVALID_DATA);
    }
}


void ali_gap_on_tx_done(ali_gap_t * p_gap, uint8_t cmd)
{
    LOGD(MOD, "In %s, cmd is %d", __func__, cmd);

    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);

    if (cmd == ALI_CMD_AUTH_RAND) {
        LOGD(MOD, "In %s, ALI_CMD_AUTH_RAND type.", __func__);
        p_gap->tx_active = false;
        p_gap->tx_cmd    = ALI_CMD_STATUS; // default: status
        notify_tx_done(p_gap);
        return;
    }

    if (cmd != ALI_CMD_REPLY && cmd != ALI_CMD_STATUS)
    {
        return;
    }

    LOGD(MOD, "In %s, p_gap->tx_cmd is %d, p_gap->tx_active is %d",
         __func__, p_gap->tx_cmd, p_gap->tx_active);

    /* check whether the command has been requested before. */
    //if (cmd == p_gap->tx_cmd && p_gap->tx_active)
    if (cmd == p_gap->tx_cmd)
    {
        p_gap->tx_active = false;
        p_gap->tx_cmd    = ALI_CMD_STATUS; // default: status
        notify_tx_done(p_gap);
    }
    else
    {
        notify_error(p_gap, ALI_ERROR_SRC_GAP_TX_DONE, NRF_ERROR_INVALID_STATE);
    }
}


