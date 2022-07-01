/**
 * \file            lwesp_ap.c
 * \brief           Access point
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
#include "lwesp/lwesp_ap.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__

/**
 * \brief           Get IP of access point
 * \param[out]      ip: Pointer to variable to write IP address
 * \param[out]      gw: Pointer to variable to write gateway address
 * \param[out]      nm: Pointer to variable to write netmask address
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_getip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm,
               const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPAP_GET;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.ip = ip;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.gw = gw;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.nm = nm;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Set IP of access point
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       ip: Pointer to IP address
 * \param[in]       gw: Pointer to gateway address. Set to `NULL` to use default gateway
 * \param[in]       nm: Pointer to netmask address. Set to `NULL` to use default netmask
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_setip(const lwesp_ip_t* ip, const lwesp_ip_t* gw, const lwesp_ip_t* nm,
               const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("ip != NULL", ip != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPAP_SET;
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.ip, ip, sizeof(*ip));
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.gw, gw, sizeof(*gw));
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.nm, nm, sizeof(*nm));

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get MAC of access point
 * \param[out]      mac: Pointer to output variable to save MAC address
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_getmac(lwesp_mac_t* mac,
                const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPAPMAC_GET;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getmac.mac = mac;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Set MAC of access point
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       mac: Pointer to variable with MAC address. Memory of at least 6 bytes is required
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_setmac(const lwesp_mac_t* mac,
                const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("mac != NULL", mac != NULL);
    LWESP_ASSERT("Bit 0 of byte 0 in AP MAC must be 0!", !(((uint8_t*)mac)[0] & 0x01));

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPAPMAC_SET;
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setmac.mac, mac, sizeof(*mac));

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Configure access point
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \note            Before you can configure access point, ESP device must be in AP mode. Check \ref lwesp_set_wifi_mode for more information
 * \param[in]       ssid: SSID name of access point
 * \param[in]       pwd: Password for network. Either set it to `NULL` or less than `64` characters
 * \param[in]       ch: Wifi RF channel
 * \param[in]       ecn: Encryption type. Valid options are `OPEN`, `WPA_PSK`, `WPA2_PSK` and `WPA_WPA2_PSK`
 * \param[in]       max_sta: Maximal number of stations access point can accept. Valid between 1 and 10 stations
 * \param[in]       hid: Set to `1` to hide access point from public access
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_set_config(const char* ssid, const char* pwd, uint8_t ch, lwesp_ecn_t ecn, uint8_t max_sta, uint8_t hid,
                    const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("ssid != NULL", ssid != NULL);
    LWESP_ASSERT("pwd == NULL || (pwd && strlen(pwd) <= 64)", pwd == NULL || (pwd != NULL && strlen(pwd) <= 64));
    LWESP_ASSERT("ecn == open || ecn == WPA_PSK || ecn == WPA2_PSK || ecn == WPA_WPA2_PSK",
                 ecn == LWESP_ECN_OPEN || ecn == LWESP_ECN_WPA_PSK || ecn == LWESP_ECN_WPA2_PSK || ecn == LWESP_ECN_WPA_WPA2_PSK);
    LWESP_ASSERT("ch <= 128", ch <= 128);
    LWESP_ASSERT("1 <= max_sta <= 10", max_sta > 0 && max_sta <= 10);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWSAP_SET;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.ssid = ssid;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.pwd = pwd;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.ch = ch;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.ecn = ecn;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.max_sta = max_sta;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf.hid = hid;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 10000);
}

/**
 * \brief           Get configuration of Soft Access Point
 *
 * \note            Before you can get configuration access point, ESP device must be in AP mode. Check \ref lwesp_set_wifi_mode for more information
 * \param[out]      ap_conf: soft access point configuration
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_get_config(lwesp_ap_conf_t* ap_conf, const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("ap_conf != NULL", ap_conf != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWSAP_GET;
    LWESP_MSG_VAR_REF(msg).msg.ap_conf_get.ap_conf = ap_conf;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 10000);
}

/**
 * \brief           List stations connected to access point
 * \param[in]       sta: Pointer to array of \ref lwesp_sta_t structure to fill with stations
 * \param[in]       stal: Number of array entries of sta parameter
 * \param[out]      staf: Number of stations connected to access point
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_list_sta(lwesp_sta_t* sta, size_t stal, size_t* staf,
                  const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("sta != NULL", sta != NULL);
    LWESP_ASSERT("stal > 0", stal > 0);

    if (staf != NULL) {
        *staf = 0;
    }

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWLIF;
    LWESP_MSG_VAR_REF(msg).msg.sta_list.stas = sta;
    LWESP_MSG_VAR_REF(msg).msg.sta_list.stal = stal;
    LWESP_MSG_VAR_REF(msg).msg.sta_list.staf = staf;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Disconnects connected station from SoftAP access point
 * \param[in]       mac: Device MAC address to disconnect.
 *                      Application may use \ref lwesp_ap_list_sta to obtain list of connected stations to SoftAP.
 *                      Set to `NULL` to disconnect all stations.
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_ap_disconn_sta(const lwesp_mac_t* mac,
                     const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWQIF;
    LWESP_MSG_VAR_REF(msg).msg.ap_disconn_sta.use_mac = mac != NULL;
    if (mac != NULL) {
        LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.ap_disconn_sta.mac, mac, sizeof(*mac));
    }
    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

#endif /* LWESP_CFG_MODE_ACCESS_POINT || __DOXYGEN__ */
