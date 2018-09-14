/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include "ali_gap.h"
#include "ali_common.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define CHECK_PAYLOAD_FORMAT 0 /**< 0: Do not check payload format.  */

/**@brief Notify error to higher layer. */
static void notify_error(ali_gap_t *p_gap, uint32_t src, uint32_t err_code)
{
    ali_gap_event_t evt;

    /* send event to higher layer. */
    evt.type                = ALI_GAP_EVT_ERROR;
    evt.data.error.source   = src;
    evt.data.error.err_code = err_code;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Notify TX_DONE event to higher layer. */
static void notify_tx_done(ali_gap_t *p_gap)
{
    ali_gap_event_t evt;

    /* send event to higher layer. */
    evt.type = ALI_GAP_EVT_TX_DONE;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Notify received data to higher layer. */
static void notify_data(ali_gap_t *p_gap, uint8_t cmd, uint8_t *data,
                        uint16_t len)
{
    ali_gap_event_t evt;

    // Assumption: only commands @ref ALI_CMD_CTRL cnd @ref ALI_CMD_QUERY can
    // reach here.

    /* send event to higher layer. */
    evt.type =
      (cmd == ALI_CMD_QUERY) ? ALI_GAP_EVT_ON_QUERY : ALI_GAP_EVT_ON_CTRL;
    evt.data.rx_data.p_data = data;
    evt.data.rx_data.length = len;
    p_gap->event_handler(p_gap->p_evt_context, &evt);
}


/**@brief Try parsing message and check integrity by the format. */
static bool try_parse(uint8_t *data, uint16_t len)
{
#if CHECK_PAYLOAD_FORMAT // On-site: removed chapter 8 from specification v1.0.4
    uint16_t offset = 0, next_offset;
    uint8_t  attr_len;

    /* Parse message according to Ali-SDK specification v1.0.4 ch. 8.1 */
    do {
        if ((attr_len = data[offset + 2]) == 0) {
            return false; // zero length attribute
        }

        next_offset = offset + 3 + attr_len;
        if (len == next_offset) {
            break; // correct payload
        } else if (len < next_offset) {
            return false;
        } else {
            offset = next_offset;
        }
    } while (1);
#endif
    return true;
}


/**@brief Core function for Tx. */
static ret_code_t send_msg(ali_gap_t *p_gap, uint8_t *p_data, uint16_t length,
                           ali_gap_tx_func_t tx_func)
{
    uint32_t err_code = BREEZE_SUCCESS;

    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_data);
    if (length == 0) {
        return BREEZE_ERROR_DATA_SIZE;
    }

    /* Check if there is any on-going Tx-process. */
    if (p_gap->tx_active) {
        return BREEZE_ERROR_BUSY;
    }

    if (try_parse(p_data, length)) {
        err_code =
          tx_func(p_gap->p_tx_func_context, p_gap->tx_cmd, p_data, length);
#if 0
        if (err_code == BREEZE_SUCCESS)
        {
            p_gap->tx_active = true;
        }
#endif
    } else {
        err_code = BREEZE_ERROR_INVALID_DATA;
    }

    return err_code;
}


ret_code_t ali_gap_init(ali_gap_t *p_gap, ali_gap_init_t const *p_init)
{
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
    return BREEZE_SUCCESS;
}


void ali_gap_reset(ali_gap_t *p_gap)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);

    /* Reset state machine. */
    p_gap->tx_active = false;
    p_gap->tx_cmd    = ALI_CMD_STATUS;
}


ret_code_t ali_gap_send_notify(ali_gap_t *p_gap, uint8_t *p_data,
                               uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_gap);
    return send_msg(p_gap, p_data, length, p_gap->tx_func_notify);
}


ret_code_t ali_gap_send_indicate(ali_gap_t *p_gap, uint8_t *p_data,
                                 uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL(p_gap);
    return send_msg(p_gap, p_data, length, p_gap->tx_func_indicate);
}


void ali_gap_on_command(ali_gap_t *p_gap, uint8_t cmd, uint8_t *p_data,
                        uint16_t length)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);
    VERIFY_PARAM_NOT_NULL_VOID(p_data);
    if (length == 0 || (cmd != ALI_CMD_CTRL && cmd != ALI_CMD_QUERY)) {
        return;
    }

    if (try_parse(p_data, length)) {
        p_gap->tx_cmd = (cmd == ALI_CMD_QUERY) ? ALI_CMD_REPLY : ALI_CMD_STATUS;
        notify_data(p_gap, cmd, p_data, length);
    } else {
        notify_error(p_gap, ALI_ERROR_SRC_GAP_CMD_RECEIVED,
                     BREEZE_ERROR_INVALID_DATA);
    }
}


void ali_gap_on_tx_done(ali_gap_t *p_gap, uint8_t cmd)
{
    /* check parameters */
    VERIFY_PARAM_NOT_NULL_VOID(p_gap);
    if (cmd != ALI_CMD_REPLY && cmd != ALI_CMD_STATUS) {
        return;
    }

    /* check whether the command has been requested before. */
    // if (cmd == p_gap->tx_cmd && p_gap->tx_active)
    if (cmd == p_gap->tx_cmd) {
        p_gap->tx_active = false;
        p_gap->tx_cmd    = ALI_CMD_STATUS; // default: status
        notify_tx_done(p_gap);
    } else {
        notify_error(p_gap, ALI_ERROR_SRC_GAP_TX_DONE, BREEZE_ERROR_INVALID_STATE);
    }
}
