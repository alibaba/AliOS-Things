/**
 * \file            lwesp_dns.c
 * \brief           DNS API
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
#include "lwesp/lwesp_dns.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_DNS || __DOXYGEN__

/**
 * \brief           Get IP address from host name
 * \param[in]       host: Pointer to host name to get IP for
 * \param[out]      ip: Pointer to \ref lwesp_ip_t variable to save IP
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_dns_gethostbyname(const char* host, lwesp_ip_t* const ip,
                        const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("host != NULL", host != NULL);
    LWESP_ASSERT("ip != NULL", ip != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPDOMAIN;
    LWESP_MSG_VAR_REF(msg).msg.dns_getbyhostname.host = host;
    LWESP_MSG_VAR_REF(msg).msg.dns_getbyhostname.ip = ip;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 20000);
}

/**
 * \brief           Enable or disable custom DNS server configuration
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       en: Set to `1` to enable, `0` to disable custom DNS configuration.
 *                  When disabled, default DNS servers are used as proposed by ESP AT commands firmware
 * \param[in]       s1: First server IP address in string format, set to `NULL` if not used
 * \param[in]       s2: Second server IP address in string format, set to `NULL` if not used.
 *                  Address `s1` cannot be the same as `s2`
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_dns_set_config(uint8_t en, const char* s1, const char* s2,
                     const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPDNS_SET;
    LWESP_MSG_VAR_REF(msg).msg.dns_setconfig.en = en;
    LWESP_MSG_VAR_REF(msg).msg.dns_setconfig.s1 = s1;
    LWESP_MSG_VAR_REF(msg).msg.dns_setconfig.s2 = s2;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get the DNS server configuration
 *
 * Retrive configuration saved in the NVS area of ESP device.
 *
 * \param[out]      s1: First server IP address in lwesp_ip_t format, set to 0.0.0.0 if not used
 * \param[out]      s2: Second server IP address in lwesp_ip_t format, set to to 0.0.0.0 if not used.
 *                  Address `s1` cannot be the same as `s2`
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_dns_get_config(lwesp_ip_t* s1, lwesp_ip_t* s2,
                     const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_TCPIP_CIPDNS_GET;
    LWESP_MSG_VAR_REF(msg).msg.dns_getconf.s1 = s1;
    LWESP_MSG_VAR_REF(msg).msg.dns_getconf.s2 = s2;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

#endif /* LWESP_CFG_DNS || __DOXYGEN__ */
