/**
 * \file            lwesp_evt.h
 * \brief           Event helper functions
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
#ifndef LWESP_HDR_EVT_H
#define LWESP_HDR_EVT_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_EVT Events management
 * \brief           Event helper functions
 * \{
 */

lwespr_t          lwesp_evt_register(lwesp_evt_fn fn);
lwespr_t          lwesp_evt_unregister(lwesp_evt_fn fn);
lwesp_evt_type_t  lwesp_evt_get_type(lwesp_evt_t* cc);

/**
 * \anchor          LWESP_EVT_RESET_DETECTED
 * \name            Reset detected
 * \brief           Event helper functions for \ref LWESP_EVT_RESET_DETECTED event
 * \{
 */

uint8_t     lwesp_evt_reset_detected_is_forced(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_RESET
 * \name            Reset event
 * \brief           Event helper functions for \ref LWESP_EVT_RESET event
 * \{
 */

lwespr_t    lwesp_evt_reset_get_result(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_RESTORE
 * \name            Restore event
 * \brief           Event helper functions for \ref LWESP_EVT_RESTORE event
 * \{
 */

lwespr_t    lwesp_evt_restore_get_result(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_AP_IP_STA
 * \name            Access point or station IP or MAC
 * \brief           Event helper functions for \ref LWESP_EVT_AP_IP_STA event
 * \{
 */

lwesp_mac_t*  lwesp_evt_ap_ip_sta_get_mac(lwesp_evt_t* cc);
lwesp_ip_t*   lwesp_evt_ap_ip_sta_get_ip(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_AP_CONNECTED_STA
 * \name            Connected station to access point
 * \brief           Event helper functions for \ref LWESP_EVT_AP_CONNECTED_STA event
 * \{
 */

lwesp_mac_t*  lwesp_evt_ap_connected_sta_get_mac(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_AP_DISCONNECTED_STA
 * \name            Disconnected station from access point
 * \brief           Event helper functions for \ref LWESP_EVT_AP_DISCONNECTED_STA event
 * \{
 */

lwesp_mac_t*  lwesp_evt_ap_disconnected_sta_get_mac(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_RECV
 * \name            Connection data received
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_RECV event
 * \{
 */

lwesp_pbuf_p  lwesp_evt_conn_recv_get_buff(lwesp_evt_t* cc);
lwesp_conn_p  lwesp_evt_conn_recv_get_conn(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_SEND
 * \name            Connection data send
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_SEND event
 * \{
 */

lwesp_conn_p  lwesp_evt_conn_send_get_conn(lwesp_evt_t* cc);
size_t      lwesp_evt_conn_send_get_length(lwesp_evt_t* cc);
lwespr_t    lwesp_evt_conn_send_get_result(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_ACTIVE
 * \name            Connection active
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_ACTIVE event
 * \{
 */

lwesp_conn_p  lwesp_evt_conn_active_get_conn(lwesp_evt_t* cc);
uint8_t     lwesp_evt_conn_active_is_client(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_CLOSE
 * \name            Connection close event
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_CLOSE event
 * \{
 */

lwesp_conn_p  lwesp_evt_conn_close_get_conn(lwesp_evt_t* cc);
uint8_t     lwesp_evt_conn_close_is_client(lwesp_evt_t* cc);
uint8_t     lwesp_evt_conn_close_is_forced(lwesp_evt_t* cc);
lwespr_t    lwesp_evt_conn_close_get_result(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_POLL
 * \name            Connection poll
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_POLL event
 * \{
 */

lwesp_conn_p  lwesp_evt_conn_poll_get_conn(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_CONN_ERROR
 * \name            Connection error
 * \brief           Event helper functions for \ref LWESP_EVT_CONN_ERROR event
 * \{
 */

lwespr_t            lwesp_evt_conn_error_get_error(lwesp_evt_t* cc);
lwesp_conn_type_t   lwesp_evt_conn_error_get_type(lwesp_evt_t* cc);
const char*         lwesp_evt_conn_error_get_host(lwesp_evt_t* cc);
lwesp_port_t        lwesp_evt_conn_error_get_port(lwesp_evt_t* cc);
void*               lwesp_evt_conn_error_get_arg(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_STA_LIST_AP
 * \name            List access points
 * \brief           Event helper functions for \ref LWESP_EVT_STA_LIST_AP event
 * \{
 */

lwespr_t    lwesp_evt_sta_list_ap_get_result(lwesp_evt_t* cc);
lwesp_ap_t* lwesp_evt_sta_list_ap_get_aps(lwesp_evt_t* cc);
size_t      lwesp_evt_sta_list_ap_get_length(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_STA_JOIN_AP
 * \name            Join access point
 * \brief           Event helper functions for \ref LWESP_EVT_STA_JOIN_AP event
 * \{
 */

lwespr_t    lwesp_evt_sta_join_ap_get_result(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_STA_INFO_AP
 * \name            Get access point info
 * \brief           Event helper functions for \ref LWESP_EVT_STA_INFO_AP event
 * \{
 */

lwespr_t    lwesp_evt_sta_info_ap_get_result(lwesp_evt_t* cc);
const char* lwesp_evt_sta_info_ap_get_ssid(lwesp_evt_t* cc);
lwesp_mac_t lwesp_evt_sta_info_ap_get_mac(lwesp_evt_t* cc);
uint8_t     lwesp_evt_sta_info_ap_get_channel(lwesp_evt_t* cc);
int16_t     lwesp_evt_sta_info_ap_get_rssi(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_DNS_HOSTBYNAME
 * \name            Get host address by name
 * \brief           Event helper functions for \ref LWESP_EVT_DNS_HOSTBYNAME event
 * \{
 */

lwespr_t    lwesp_evt_dns_hostbyname_get_result(lwesp_evt_t* cc);
const char* lwesp_evt_dns_hostbyname_get_host(lwesp_evt_t* cc);
lwesp_ip_t* lwesp_evt_dns_hostbyname_get_ip(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_PING
 * \name            Ping
 * \brief           Event helper functions for \ref LWESP_EVT_PING event
 * \{
 */

lwespr_t    lwesp_evt_ping_get_result(lwesp_evt_t* cc);
const char* lwesp_evt_ping_get_host(lwesp_evt_t* cc);
uint32_t    lwesp_evt_ping_get_time(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_SNTP_TIME
 * \name            Simple Network Time Protocol
 * \brief           Event helper functions for \ref LWESP_EVT_SNTP_TIME event
 * \{
 */

lwespr_t    lwesp_evt_sntp_time_get_result(lwesp_evt_t* cc);
const lwesp_datetime_t* lwesp_evt_sntp_time_get_datetime(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_WEBSERVER
 * \name            Web Server
 * \brief           Event helper functions for \ref LWESP_EVT_WEBSERVER event
 * \{
 */

uint8_t    lwesp_evt_webserver_get_status(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \anchor          LWESP_EVT_SERVER
 * \name            Server
 * \brief           Event helper functions for \ref LWESP_EVT_SERVER
 * \{
 */

lwespr_t    lwesp_evt_server_get_result(lwesp_evt_t* cc);
lwesp_port_t  lwesp_evt_server_get_port(lwesp_evt_t* cc);
uint8_t     lwesp_evt_server_is_enable(lwesp_evt_t* cc);

/**
 * \}
 */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_EVT_H */
