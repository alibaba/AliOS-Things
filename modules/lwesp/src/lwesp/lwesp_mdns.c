/**
 * \file            lwesp_mdns.c
 * \brief           mDNS API
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
#include "lwesp/lwesp_mdns.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_MDNS || __DOXYGEN__

/**
 * \brief           Configure mDNS parameters with hostname and server
 * \param[in]       en: Status to enable `1` or disable `0` mDNS function
 * \param[in]       host: mDNS host name
 * \param[in]       server: mDNS server name
 * \param[in]       port: mDNS server port number
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_mdns_set_config(uint8_t en, const char* host, const char* server, lwesp_port_t port,
                      const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    if (en) {
        LWESP_ASSERT("host != NULL", host != NULL);
        LWESP_ASSERT("server != NULL", server != NULL);
        LWESP_ASSERT("port > 0", port > 0);
    }

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_MDNS;
    LWESP_MSG_VAR_REF(msg).msg.mdns.en = en;
    LWESP_MSG_VAR_REF(msg).msg.mdns.host = host;
    LWESP_MSG_VAR_REF(msg).msg.mdns.server = server;
    LWESP_MSG_VAR_REF(msg).msg.mdns.port = port;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

#endif /* LWESP_CFG_MDNS || __DOXYGEN__ */
