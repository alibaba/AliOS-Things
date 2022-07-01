/**
 * \file            lwesp_sta.h
 * \brief           Station API
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
#ifndef LWESP_HDR_STA_H
#define LWESP_HDR_STA_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_STA Station API
 * \brief           Station API
 * \{
 */

lwespr_t    lwesp_sta_join(const char* name, const char* pass, const lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_quit(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_autojoin(uint8_t en, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_reconnect_set_config(uint16_t interval, uint16_t rep_cnt, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_getip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_setip(const lwesp_ip_t* ip, const lwesp_ip_t* gw, const lwesp_ip_t* nm, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_getmac(lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_setmac(const lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
uint8_t     lwesp_sta_has_ip(void);
uint8_t     lwesp_sta_is_joined(void);
lwespr_t    lwesp_sta_copy_ip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm, uint8_t* is_dhcp);
lwespr_t    lwesp_sta_list_ap(const char* ssid, lwesp_ap_t* aps, size_t apsl, size_t* apf, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_sta_get_ap_info(lwesp_sta_info_ap_t* info, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
uint8_t     lwesp_sta_is_ap_802_11b(lwesp_ap_t* ap);
uint8_t     lwesp_sta_is_ap_802_11g(lwesp_ap_t* ap);
uint8_t     lwesp_sta_is_ap_802_11n(lwesp_ap_t* ap);

#if LWESP_CFG_IPV6 || __DOXYGEN__
uint8_t     lwesp_sta_has_ipv6_local(void);
uint8_t     lwesp_sta_has_ipv6_global(void);
#else
#define     lwesp_sta_has_ipv6_local()          0
#define     lwesp_sta_has_ipv6_global()         0
#endif /* LWESP_CFG_IPV6 || __DOXYGEN__ */

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_STA_H */
