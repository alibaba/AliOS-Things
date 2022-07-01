/**
 * \file            lwesp_sta.c
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
#include "lwesp/lwesp_private.h"
#include "lwesp/lwesp_sta.h"
#include "lwesp/lwesp_mem.h"

#if LWESP_CFG_MODE_STATION || __DOXYGEN__

/**
 * \brief           Quit (disconnect) from access point
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_quit(const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWQAP;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Join as station to access point
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       name: SSID of access point to connect to
 * \param[in]       pass: Password of access point. Use `NULL` if AP does not have password
 * \param[in]       mac: Pointer to MAC address of AP.
 *                      If multiple APs with same name exist, MAC may help to select proper one.
 *                      Set to `NULL` if not needed
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_join(const char* name, const char* pass, const lwesp_mac_t* mac,
               const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("name != NULL", name != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWJAP;
    memcpy(LWESP_MSG_VAR_REF(msg).msg.sta_join.name, name, strlen(name)+1);
    memcpy(LWESP_MSG_VAR_REF(msg).msg.sta_join.pass, pass, strlen(pass)+1);
    if (mac == NULL) {
        LWESP_MSG_VAR_REF(msg).msg.sta_join.mac = mac;
    } else {
        memcpy(&(LWESP_MSG_VAR_REF(msg).msg.sta_join.mac_s), mac, sizeof(lwesp_mac_t));
        LWESP_MSG_VAR_REF(msg).msg.sta_join.mac = &(LWESP_MSG_VAR_REF(msg).msg.sta_join.mac_s); 
    }
    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 30000);
}

/**
 * \brief           Configure auto join to access point on startup
 * \note            For auto join feature, you need to do a join to access point with default mode.
 *                  Check \ref lwesp_sta_join for more information
 * \param[in]       en: Set to `1` to enable or `0` to disable
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_autojoin(uint8_t en,
                   const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWAUTOCONN;
    LWESP_MSG_VAR_REF(msg).msg.sta_autojoin.en = en;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 30000);
}

/**
 * \brief           Set reconnect interval and maximum tries when connection drops
 * \param[in]       interval: Interval in units of seconds. Valid numbers are `1-7200` or `0` to disable reconnect feature
 * \param[in]       rep_cnt: Repeat counter. Number of maximum tries for reconnect.
 *                      Valid entries are `1-1000` or `0` to always try.
 *                      This parameter is only valid if interval is not `0`
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_reconnect_set_config(uint16_t interval, uint16_t rep_cnt,
                               const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("interval <= 7200", interval <= 7200);
    LWESP_ASSERT("rep_cnt <= 1000", rep_cnt <= 1000);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWRECONNCFG;
    LWESP_MSG_VAR_REF(msg).msg.sta_reconn_set.interval = interval;
    LWESP_MSG_VAR_REF(msg).msg.sta_reconn_set.rep_cnt = rep_cnt;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get current access point information (name, mac, channel, rssi)
 * \note            Access point station is currently connected to
 * \param[in]       info: Pointer to connected access point information
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_get_ap_info(lwesp_sta_info_ap_t* info,
                      const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    if (!lwesp_sta_is_joined()) {
        return lwespERRWIFINOTCONNECTED;
    }
    LWESP_ASSERT("info != NULL", info != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWJAP_GET;
    LWESP_MSG_VAR_REF(msg).msg.sta_info_ap.info = info;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get station IP address
 * \param[out]      ip: Pointer to variable to save IP address
 * \param[out]      gw: Pointer to output variable to save gateway address
 * \param[out]      nm: Pointer to output variable to save netmask address
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_getip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm,
                const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPSTA_GET;
    LWESP_MSG_VAR_REF(msg).cmd = LWESP_CMD_WIFI_CWDHCP_GET;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.ip = ip;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.gw = gw;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getip.nm = nm;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Set station IP address
 *
 * Application may manually set IP address.
 * When this happens, stack will check for DHCP settings and will read actual IP address from device.
 * Once procedure is finished, \ref LWESP_EVT_WIFI_IP_ACQUIRED event will be sent to application where
 * user may read the actual new IP and DHCP settings.
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \note            DHCP is automatically disabled when using static IP address
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
lwesp_sta_setip(const lwesp_ip_t* ip, const lwesp_ip_t* gw, const lwesp_ip_t* nm,
                const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("ip != NULL", ip != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPSTA_SET;
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.ip, ip, sizeof(*ip));
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.gw, gw, sizeof(*gw));
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setip.nm, nm, sizeof(*nm));

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Get station MAC address
 * \param[out]      mac: Pointer to output variable to save MAC address
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_getmac(lwesp_mac_t* mac,
                 const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPSTAMAC_GET;
    LWESP_MSG_VAR_REF(msg).msg.sta_ap_getmac.mac = mac;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Set station MAC address
 *
 * Configuration changes will be saved in the NVS area of ESP device.
 *
 * \param[in]       mac: Pointer to variable with MAC address
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_setmac(const lwesp_mac_t* mac,
                 const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    LWESP_ASSERT("mac != NULL", mac != NULL);

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CIPSTAMAC_SET;
    LWESP_MEMCPY(&LWESP_MSG_VAR_REF(msg).msg.sta_ap_setmac.mac, mac, sizeof(*mac));

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 1000);
}

/**
 * \brief           Check if ESP got IP from access point
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_sta_has_ip(void) {
    uint8_t res;
    lwesp_core_lock();
    res = LWESP_U8(esp.m.sta.f.has_ip);
    lwesp_core_unlock();
    return res;
}

#if LWESP_CFG_IPV6 || __DOXYGEN__

/**
 * \brief           Check if station has local IPV6 IP
 * Local IP is used between station and router
 * \note            Defined as macro with `0` constant if \ref LWESP_CFG_IPV6 is disabled
 * \return          `1` if local IPv6 is available, `0` otherwise
 */
uint8_t
lwesp_sta_has_ipv6_local(void) {
    uint8_t res;
    lwesp_core_lock();
    res = LWESP_U8(esp.m.sta.f.has_ipv6_ll);
    lwesp_core_unlock();
    return res;
}

/**
 * \brief           Check if station has global IPV6 IP
 * Global IP is used router and outside network
 * \note            Defined as macro with `0` constant if \ref LWESP_CFG_IPV6 is disabled
 * \return          `1` if global IPv6 is available, `0` otherwise
 */
uint8_t
lwesp_sta_has_ipv6_global(void) {
    uint8_t res;
    lwesp_core_lock();
    res = LWESP_U8(esp.m.sta.f.has_ipv6_gl);
    lwesp_core_unlock();
    return res;
}

#endif /* LWESP_CFG_IPV6 || __DOXYGEN__ */

/**
 * \brief           Check if station is connected to WiFi network
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_sta_is_joined(void) {
    return lwesp_sta_has_ip();
}

/**
 * \brief           Copy IP address from internal value to user variable
 * \note            Use \ref lwesp_sta_getip to refresh actual IP value from device
 * \param[out]      ip: Pointer to output IP variable. Set to `NULL` if not interested in IP address
 * \param[out]      gw: Pointer to output gateway variable. Set to `NULL` if not interested in gateway address
 * \param[out]      nm: Pointer to output netmask variable. Set to `NULL` if not interested in netmask address
 * \param[out]      is_dhcp: Pointer to output DHCP status variable. Set to `NULL` if not interested
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_copy_ip(lwesp_ip_t* ip, lwesp_ip_t* gw, lwesp_ip_t* nm, uint8_t* is_dhcp) {
    lwespr_t res = lwespERR;
    if ((ip != NULL || gw != NULL || nm != NULL || is_dhcp != NULL)
        && lwesp_sta_has_ip()) {                /* Do we have a valid IP address? */
        lwesp_core_lock();
        if (ip != NULL) {
            LWESP_MEMCPY(ip, &esp.m.sta.ip, sizeof(esp.m.sta.ip));
        }
        if (gw != NULL) {
            LWESP_MEMCPY(gw, &esp.m.sta.gw, sizeof(esp.m.sta.gw));
        }
        if (nm != NULL) {
            LWESP_MEMCPY(nm, &esp.m.sta.nm, sizeof(esp.m.sta.nm));
        }
        if (is_dhcp != NULL) {
            *is_dhcp = esp.m.sta.dhcp;
        }
        res = lwespOK;
        lwesp_core_unlock();
    }
    return res;
}

/**
 * \brief           List for available access points ESP can connect to
 * \param[in]       ssid: Optional SSID name to search for. Set to `NULL` to disable filter
 * \param[in]       aps: Pointer to array of available access point parameters
 * \param[in]       apsl: Length of aps array
 * \param[out]      apf: Pointer to output variable to save number of access points found
 * \param[in]       evt_fn: Callback function called when command has finished. Set to `NULL` when not used
 * \param[in]       evt_arg: Custom argument for event callback function
 * \param[in]       blocking: Status whether command should be blocking or not
 * \return          \ref lwespOK on success, member of \ref lwespr_t enumeration otherwise
 */
lwespr_t
lwesp_sta_list_ap(const char* ssid, lwesp_ap_t* aps, size_t apsl, size_t* apf,
                  const lwesp_api_cmd_evt_fn evt_fn, void* const evt_arg, const uint32_t blocking) {
    LWESP_MSG_VAR_DEFINE(msg);

    if (apf != NULL) {
        *apf = 0;
    }

    LWESP_MSG_VAR_ALLOC(msg, blocking);
    LWESP_MSG_VAR_SET_EVT(msg, evt_fn, evt_arg);
    LWESP_MSG_VAR_REF(msg).cmd_def = LWESP_CMD_WIFI_CWLAP;
    LWESP_MSG_VAR_REF(msg).msg.ap_list.ssid = ssid;
    LWESP_MSG_VAR_REF(msg).msg.ap_list.aps = aps;
    LWESP_MSG_VAR_REF(msg).msg.ap_list.apsl = apsl;
    LWESP_MSG_VAR_REF(msg).msg.ap_list.apf = apf;

    return lwespi_send_msg_to_producer_mbox(&LWESP_MSG_VAR_REF(msg), lwespi_initiate_cmd, 30000);
}

/**
 * \brief           Check if access point is `802.11b` compatible
 * \param[in]       ap: Access point detailes acquired by \ref lwesp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_sta_is_ap_802_11b(lwesp_ap_t* ap) {
    return LWESP_U8((ap->bgn & 0x01) == 0x01);  /* Bit 0 is for b check */
}

/**
 * \brief           Check if access point is `802.11g` compatible
 * \param[in]       ap: Access point detailes acquired by \ref lwesp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_sta_is_ap_802_11g(lwesp_ap_t* ap) {
    return LWESP_U8((ap->bgn & 0x02) == 0x02);  /* Bit 1 is for g check */
}

/**
 * \brief           Check if access point is `802.11n` compatible
 * \param[in]       ap: Access point detailes acquired by \ref lwesp_sta_list_ap
 * \return          `1` on success, `0` otherwise
 */
uint8_t
lwesp_sta_is_ap_802_11n(lwesp_ap_t* ap) {
    return LWESP_U8((ap->bgn & 0x04) == 0x04);  /* Bit 2 is for n check */
}

#endif /* LWESP_CFG_MODE_STATION || __DOXYGEN__ */
