/**
 * \file            lwesp_ap.h
 * \brief           Access point API
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
#ifndef LWESP_HDR_AP_H
#define LWESP_HDR_AP_H

#include "lwesp/lwesp.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * \ingroup         LWESP
 * \defgroup        LWESP_AP Access point
 * \brief           Access point
 * \{
 *
 * Functions to manage access point (AP) on ESP device.
 *
 * In order to be able to use AP feature, \ref LWESP_CFG_MODE_ACCESS_POINT must be enabled.
 */

lwespr_t    lwesp_ap_getip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_ap_setip(const lwesp_ip_t* ip, const lwesp_ip_t* gw, const lwesp_ip_t* nm, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_ap_getmac(lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_ap_setmac(const lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

lwespr_t    lwesp_ap_get_config(lwesp_ap_conf_t* ap_conf, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_ap_set_config(const char* ssid, const char* pwd, uint8_t ch, lwesp_ecn_t ecn, uint8_t max_sta, uint8_t hid, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg,
                                const uint32_t blocking);

lwespr_t    lwesp_ap_list_sta(lwesp_sta_t* sta, size_t stal, size_t* staf, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);
lwespr_t    lwesp_ap_disconn_sta(const lwesp_mac_t* mac, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking);

/**
 * \}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LWESP_HDR_AP_H */
