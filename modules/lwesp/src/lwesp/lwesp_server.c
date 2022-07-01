/**
 * \file            lwesp.c
 * \brief           LwESP core file
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
#include "lwesp/lwesp_mem.h"

/**
 * \brief           Enables or disables server mode
 * \param[in]       en: Set to `1` to enable server, `0` otherwise
 * \param[in]       port: Port number used to listen on. Must also be used when disabling server mode
 * \param[in]       max_conn: Number of maximal connections populated by server
 * \param[in]       timeout: Time used to automatically close the connection in units of seconds.
 *                      Set to `0` to disable timeout feature (not recommended)
 * \param[in]       server_evt_fn: Connection callback function for new connections started as server
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_set_server(uint8_t en, lwesp_port_t port, uint16_t max_conn, uint16_t timeout, lwesp_evt_fn server_evt_fn,
                 const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("port > 0", port > 0);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPSERVER;
    if (en) {
        LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_TCPIP_CIPSERVERMAXCONN;  /* First command is to set maximal number of connections for server */
    }
    LWESP_MSG_VAR_REF(msg).msg.tcpip_server.en = en;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_server.port = port;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_server.max_conn = max_conn;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_server.timeout = timeout;
    LWESP_MSG_VAR_REF(msg).msg.tcpip_server.cb = server_evt_fn;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}
