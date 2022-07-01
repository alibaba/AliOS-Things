/**
 * \file            lwesp_sntp.c
 * \brief           Connection API
 */

/*
 * Copyright (c) 2022 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of LwESP - Lightweight ESP-AT parser library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v1.1.2-dev
 */
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_sntp.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_SNTP || __DOXYGEN__

/**
 * \brief           Configure SNTP mode parameters.
 * It must be called prior any \ref lwesp_sntp_gettime can be used,
 * otherwise wrong data will be received back
 * \param[in]       en: Status whether SNTP mode is enabled or disabled on ESP device
 * \param[in]       tz: Timezone to use when SNTP acquires time, between `-12` and `14`
 * \param[in]       h1: Optional first SNTP server for time. Set to `NULL` if not used
 * \param[in]       h2: Optional second SNTP server for time. Set to `NULL` if not used
 * \param[in]       h3: Optional third SNTP server for time. Set to `NULL` if not used
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sntp_set_config(uint8_t en, int16_t tz, const char* h1, const char* h2, const char* h3,
                      const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSNTPCFG;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_cfg.en = en;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_cfg.tz = tz;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_cfg.h1 = h1;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_cfg.h2 = h2;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_cfg.h3 = h3;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Set SNTP synchronization interval on Espressif device
 * SNTP must be configured using \ref lwesp_sntp_set_config before you can use this function.
 *
 * \note            This command is not available for all Espressif devices using AT commands
 *                      and will return error when this is the case.
 * \param[in]       interval: Synchronization interval in units of seconds.
 *                      Value can be set between `15` and `4294967` included
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sntp_set_interval(uint32_t interval, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("interval >= 15 && interval <= 4294967", interval >= 15 && interval <= 4294967);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSNTPINTV;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_intv.interval = interval;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get time from SNTP servers
 * SNTP must be configured using \ref lwesp_sntp_set_config before you can use this function
 * \param[out]      dt: Pointer to \ref lwesp_datetime_t structure to fill with date and time values
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sntp_gettime(lwesp_datetime_t* dt,
                   const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSNTPTIME;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_sntp_time.dt = dt;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 10000);
}

#endif /* LWESP_CFG_SNTP || __DOXYGEN__ */
