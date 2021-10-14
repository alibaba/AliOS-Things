/*
 * This file is part of the MicroPython project, http://micropython.org/
 *
 * Development of the code in this file was sponsored by Microbric Pty Ltd
 * and Mnemote Pty Ltd
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2016, 2017 Nick Moore @mnemote
 * Copyright (c) 2017 "Eric Poulsen" <eric@zyxod.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "modnetwork.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "shared/netutils/netutils.h"
#include "lwip/dns.h"
#include "netmgr_wifi.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG                      "mod_network"

#define DNS_MAIN                     TCPIP_ADAPTER_DNS_MAIN
#define MODNETWORK_INCLUDE_CONSTANTS (1)

#define QS(x)                        (uintptr_t) MP_OBJ_NEW_QSTR(x)

typedef enum {
    WIFI_MODE_NULL = 0x00,
    WIFI_MODE_STA = 0x01,
    WIFI_MODE_AP = 0x02,
} wifi_mode_t;

enum {
    STAT_IDLE = 1000,
    STAT_CONNECTING = 1001,
    STAT_GOT_IP = 1010,
};

NORETURN void _haas_wifi_exceptions(wifi_result_t e)
{
    switch (e) {
    case RET_WIFI_COMMON_FAIL:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Common Fail"));
    case RET_WIFI_INVALID_ARG:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Arguments"));
    case RET_WIFI_INVALID_PASSWORD:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Password"));
    case RET_WIFI_MEMORY_ERROR:
        mp_raise_OSError(MP_ENOMEM);
    case RET_WIFI_INIT_FAIL:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Init Fail"));
    case RET_WIFI_NOT_INITED:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Initialized"));
    case RET_WIFI_STATUS_ERROR:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Request In Error Status"));
    case RET_WIFI_SCAN_REQ_FAIL:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Scan Fail To Start"));
    case RET_WIFI_SCAN_NO_AP_FOUND:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Can Not Find Any SSID"));
    case RET_WIFI_NO_SUITABLE_NETWORK:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi No Suitable Network To Connect"));
    case RET_WIFI_CONN_REQ_FAIL:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Connect Fail To Start"));
    case RET_WIFI_CONN_FAIL:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Connect Procedure Result In Fail"));
    case RET_WIFI_CONN_NO_SSID_CONFIG:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi No Saved SSID Config To Connect"));
    case RET_WIFI_DISC_FAIL:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Disconnect Procedure Result In Fail"));
    case RET_WIFI_WPS_NOT_FOUND:
        mp_raise_msg(&mp_type_OSError,
                     MP_ERROR_TEXT("Wifi Can Not Find WPS AP"));
    case RET_WIFI_WPS_REQ_FAIL:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Fail To Start WPS"));
    default:
        mp_raise_msg_varg(&mp_type_RuntimeError,
                          MP_ERROR_TEXT("Wifi Unknown Error 0x%04x"), e);
    }
}

static inline void haas_exceptions(wifi_result_t e)
{
    if (e != RET_WIFI_OK) {
        _haas_wifi_exceptions(e);
    }
}

#define HAAS_EXCEPTIONS(x)  \
    do {                    \
        haas_exceptions(x); \
    } while (0);

typedef struct _wlan_if_obj_t {
    mp_obj_base_t base;
    netmgr_hdl_t hdl;  // can get through netmgr_wifi_get_dev
    netmgr_wifi_mode_t if_id;
} wlan_if_obj_t;

const mp_obj_type_t wlan_if_type;
STATIC const wlan_if_obj_t wlan_sta_obj = { { &wlan_if_type },
                                            NETMGR_WIFI_MODE_STA };
STATIC const wlan_if_obj_t wlan_ap_obj = { { &wlan_if_type },
                                           NETMGR_WIFI_MODE_AP };

// Set to "true" if haas_wifi_start() was called
static bool wifi_started = false;

// Set to "true" if the STA interface is requested to be connected by the
// user, used for automatic reassociation.
static bool wifi_sta_connect_requested = false;

// Set to "true" if the STA interface is connected to wifi and has IP address.
static bool wifi_sta_connected = false;

// Store the current status. 0 means None here, safe to do so as first enum
// value is WIFI_REASON_UNSPECIFIED=1.
static uint8_t wifi_sta_disconn_reason = 0;

static int wifi_set_mac(netmgr_hdl_t hdl, uint8_t *mac)
{
    return ioctl(hdl, WIFI_DEV_CMD_SET_MAC, mac);
}

static int wifi_get_mac(netmgr_hdl_t hdl, uint8_t *mac)
{
    return ioctl(hdl, WIFI_DEV_CMD_GET_MAC, mac);
}

static int wifi_get_rssi(netmgr_hdl_t hdl, int *rssi)
{
    int state = -1;
    wifi_ap_record_t out = { 0 };
    int ret = ioctl(hdl, WIFI_DEV_CMD_STA_GET_LINK_STATUS, &out));
    if (ret == 0) {
        memcpy(rssi, &out.rssi, sizeof(out.rssi));
        state = 0;
    }

    return state;
}

// This function is called by the system-event task and so runs in a different
// thread to the main MicroPython task.  It must not raise any Python
// exceptions.
static wifi_result_t event_handler(void *ctx, system_event_t *event)
{
    switch (event->event_id) {
    case CONN_STATE_DISCONNECTING:
        LOGD(LOG_TAG, "wifi CONN_STATE_DISCONNECTING");
        break;
    case CONN_STATE_DISCONNECTED:
        LOGI(LOG_TAG, "wifi STA_DISCONNECTED");
        break;
    case CONN_STATE_CONNECTING:
        LOGD(LOG_TAG, "wifi CONN_STATE_CONNECTING");
        break;
    case CONN_STATE_CONNECTED:
        LOGI(LOG_TAG, "wifi CONN_STATE_CONNECTED");
        break;
    case CONN_STATE_OBTAINING_IP:
        LOGI(LOG_TAG, "wifi CONN_STATE_OBTAINING_IP");
        wifi_sta_connected = true;
        wifi_sta_disconn_reason = 0;
        break;
    case CONN_STATE_NETWORK_CONNECTED:
        LOGI(LOG_TAG, "wifi CONN_STATE_NETWORK_CONNECTED");
        break;
    case CONN_STATE_FAILED:
        LOGI(LOG_TAG, "wifi CONN_STATE_FAILED");
        break;
    default:
        LOGI(LOG_TAG, "event %d", event->event_id);
        break;
    }
    return RET_WIFI_OK;
}

STATIC void require_if(mp_obj_t wlan_if, int if_no)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no) {
        mp_raise_msg(&mp_type_OSError, if_no == NETMGR_WIFI_MODE_STA
                                           ? MP_ERROR_TEXT("STA required")
                                           : MP_ERROR_TEXT("AP required"));
    }
}

STATIC mp_obj_t get_wlan(size_t n_args, const mp_obj_t *args)
{
    static int initialized = 0;
    if (!initialized) {
        LOGD(LOG_TAG, "modnetwork get WLAN start");
        HAAS_EXCEPTIONS(event_service_init(NULL));
        HAAS_EXCEPTIONS(netmgr_service_init(NULL));
        LOGD(LOG_TAG, "modnetwork got WLAN");
        initialized = 1;
    }

    int idx = (n_args > 0) ? mp_obj_get_int(args[0]) : NETMGR_WIFI_MODE_STA;
    if (idx == NETMGR_WIFI_MODE_STA) {
        return MP_OBJ_FROM_PTR(&wlan_sta_obj);
    } else if (idx == NETMGR_WIFI_MODE_AP) {
        return MP_OBJ_FROM_PTR(&wlan_ap_obj);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WLAN interface identifier"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(get_wlan_obj, 0, 1, get_wlan);

STATIC mp_obj_t wlan_initialize()
{
    static int initialized = 0;
    if (!initialized) {
        LOGD(LOG_TAG, "modnetwork Initializing start");
        HAAS_EXCEPTIONS(event_service_init(NULL));
        HAAS_EXCEPTIONS(netmgr_service_init(NULL));
        LOGD(LOG_TAG, "modnetwork Initialized");
        initialized = 1;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(wlan_initialize_obj, wlan_initialize);

STATIC mp_obj_t haas_wlan_active(size_t n_args, const mp_obj_t *args)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    wifi_mode_t mode;
    if (!wifi_started) {
        mode = WIFI_MODE_NULL;
    } else {
        HAAS_EXCEPTIONS(haas_wlan_wifi_get_mode(&mode));
    }

    int bit =
        (self->if_id == NETMGR_WIFI_MODE_STA) ? WIFI_MODE_STA : WIFI_MODE_AP;
    if (n_args > 1) {
        bool active = mp_obj_is_true(args[1]);
        mode = active ? (mode | bit) : (mode & ~bit);
        if (mode == WIFI_MODE_NULL) {
            if (wifi_started) {
                HAAS_EXCEPTIONS(haas_wlan_wifi_stop());
                wifi_started = false;
            }
        } else {
            HAAS_EXCEPTIONS(haas_wlan_wifi_set_mode(mode));
            if (!wifi_started) {
                HAAS_EXCEPTIONS(haas_wlan_wifi_start());
                wifi_started = true;
            }
        }
    }

    return (mode & bit) ? mp_const_true : mp_const_false;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(haas_wlan_active_obj, 1, 2,
                                           haas_wlan_active);

STATIC mp_obj_t haas_wlan_connect(size_t n_args, const mp_obj_t *pos_args,
                                  mp_map_t *kw_args)
{
    enum {
        ARG_ssid,
        ARG_password,
        ARG_bssid
    };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_, MP_ARG_OBJ, { .u_obj = mp_const_none } },
        { MP_QSTR_bssid,
          MP_ARG_KW_ONLY | MP_ARG_OBJ,
          { .u_obj = mp_const_none } },
    };

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(pos_args[0]);

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args,
                     MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    netmgr_connect_params_t wifi_sta_config = { 0 };

    // configure any parameters that are given
    if (n_args > 1) {
        size_t len;
        const char *p;
        if (args[ARG_ssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
            memcpy(wifi_sta_config.wifi_params.ssid, p,
                   MIN(len, sizeof(wifi_sta_config.wifi_params.ssid)));
        }
        if (args[ARG_password].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
            memcpy(wifi_sta_config.wifi_params.pwd, p,
                   MIN(len, sizeof(wifi_sta_config.wifi_params.pwd)));
        }
        if (args[ARG_bssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
            if (len != sizeof(wifi_sta_config.wifi_params.bssid)) {
                mp_raise_ValueError(NULL);
            }
            wifi_sta_config.wifi_params.bssid_set = 1;
            memcpy(wifi_sta_config.wifi_params.bssid, p,
                   sizeof(wifi_sta_config.wifi_params.bssid));
        }
        // HAAS_EXCEPTIONS(haas_wlan_wifi_set_config(NETMGR_WIFI_MODE_STA,
        // &wifi_sta_config));
    }

    // connect to the WiFi AP
    MP_THREAD_GIL_EXIT();
    HAAS_EXCEPTIONS(netmgr_connect(self->hdl, &wifi_sta_config));
    MP_THREAD_GIL_ENTER();
    wifi_sta_connect_requested = true;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(haas_wlan_connect_obj, 1, haas_wlan_connect);

STATIC mp_obj_t haas_wlan_disconnect(mp_obj_t self_in)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    wifi_sta_connect_requested = false;
    HAAS_EXCEPTIONS(netmgr_disconnect(self->hdl));
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(haas_wlan_disconnect_obj,
                                 haas_wlan_disconnect);

STATIC mp_obj_t haas_wlan_status(size_t n_args, const mp_obj_t *args)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        if (self->if_id == NETMGR_WIFI_MODE_STA) {
            // Case of no arg is only for the STA interface
            if (wifi_sta_connected) {
                // Happy path, connected with IP
                return MP_OBJ_NEW_SMALL_INT(STAT_GOT_IP);
            } else if (wifi_sta_connect_requested) {
                // No connection or error, but is requested = Still connecting
                return MP_OBJ_NEW_SMALL_INT(STAT_CONNECTING);
            } else if (wifi_sta_disconn_reason == 0) {
                // No activity, No error = Idle
                return MP_OBJ_NEW_SMALL_INT(STAT_IDLE);
            } else {
                // Simply pass the error through from haas_wlan-identifier
                return MP_OBJ_NEW_SMALL_INT(wifi_sta_disconn_reason);
            }
        }
        return mp_const_none;
    }

#if 0
    // one argument: return status based on query parameter
    switch ((uintptr_t)args[1]) {
        case QS(MP_QSTR_stations): {
            // return list of connected stations, only if in soft-AP mode
            require_if(args[0], NETMGR_WIFI_MODE_AP);
            netmgr_wifi_config_t station_list = {0};
            HAAS_EXCEPTIONS(netmgr_wifi_get_config(self->hdl, &station_list));
            netmgr_wifi_ap_info_t *stations = &station_list.config;
            mp_obj_t list = mp_obj_new_list(0, NULL);
            for (int i = 0; i < station_list.ap_num; ++i) {
                mp_obj_tuple_t *t = mp_obj_new_tuple(1, NULL);
                t->items[0] = mp_obj_new_bytes(stations[i].mac, sizeof(stations[i].mac));
                mp_obj_list_append(list, t);
            }
            return list;
        }
        case QS(MP_QSTR_rssi): {
            // return signal of AP, only in STA mode
            require_if(args[0], NETMGR_WIFI_MODE_STA);

            netmgr_ifconfig_info_t info;
            HAAS_EXCEPTIONS(netmgr_get_ifconfig(self->hdl, &info));
            return MP_OBJ_NEW_SMALL_INT(info.rssi);
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    }
#endif

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(haas_wlan_status_obj, 1, 2,
                                           haas_wlan_status);

STATIC mp_obj_t haas_wlan_scan(mp_obj_t self_in)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);

    // check that STA mode is active
    wifi_mode_t mode;
    HAAS_EXCEPTIONS(haas_wlan_wifi_get_mode(&mode));
    if ((mode & WIFI_MODE_STA) == 0) {
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("STA must be active"));
    }

    mp_obj_t list = mp_obj_new_list(0, NULL);
    netmgr_wifi_ap_list_t wifi_ap_records[16] = { 0 };
    // XXX how do we scan hidden APs (and if we can scan them, are they really
    // hidden?)
    MP_THREAD_GIL_EXIT();
    int ap_num = netmgr_wifi_scan_result(&wifi_ap_records, 16,
                                         NETMGR_WIFI_SCAN_TYPE_FULL);
    MP_THREAD_GIL_ENTER();
    if ((ap_num != -1) && (ap_num < 16)) {
        for (uint16_t i = 0; i < ap_num; i++) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(5, NULL);
            int8_t *x = memchr(wifi_ap_records[i].ssid, 0,
                               sizeof(wifi_ap_records[i].ssid));
            int ssid_len = x ? x - wifi_ap_records[i].ssid
                             : sizeof(wifi_ap_records[i].ssid);
            t->items[0] = mp_obj_new_bytes(wifi_ap_records[i].ssid, ssid_len);
            t->items[1] = mp_obj_new_bytes(wifi_ap_records[i].bssid,
                                           sizeof(wifi_ap_records[i].bssid));
            t->items[2] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].ap_power);
            t->items[3] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].channel);
            t->items[4] = MP_OBJ_NEW_SMALL_INT(wifi_ap_records[i].sec_type);
            mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
        }
        free(wifi_ap_records);
    }
    return list;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(haas_wlan_scan_obj, haas_wlan_scan);

STATIC mp_obj_t haas_wlan_isconnected(mp_obj_t self_in)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == NETMGR_WIFI_MODE_STA) {
        return mp_obj_new_bool(wifi_sta_connected);
    } else {
        wifi_sta_list_t sta;
        haas_wlan_wifi_ap_get_sta_list(&sta);
        return mp_obj_new_bool(sta.num != 0);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(haas_wlan_isconnected_obj,
                                 haas_wlan_isconnected);

STATIC mp_obj_t haas_wlan_ifconfig(size_t n_args, const mp_obj_t *args)
{
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    netmgr_ifconfig_info_t info = { 0 };
    if (n_args == 1) {
        // get
        HAAS_EXCEPTIONS(netmgr_get_ifconfig(self->hdl, &info));
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)&info.ip_addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&info.mask, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&info.gw, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)&info.dns_server,
                                      NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        // set
        if (mp_obj_is_type(args[1], &mp_type_tuple) ||
            mp_obj_is_type(args[1], &mp_type_list)) {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1], 4, &items);
            netutils_parse_ipv4_addr(items[0], (void *)&info.ip_addr,
                                     NETUTILS_BIG);
            if (mp_obj_is_integer(items[1])) {
                // allow numeric mask, i.e.:
                // 24 -> 255.255.255.0
                // 16 -> 255.255.0.0
                // etc...
                uint32_t *m = (uint32_t *)&info.mask;
                *m = htonl(0xffffffff << (32 - mp_obj_get_int(items[1])));
            } else {
                netutils_parse_ipv4_addr(items[1], (void *)&info.mask,
                                         NETUTILS_BIG);
            }
            netutils_parse_ipv4_addr(items[2], (void *)&info.gw, NETUTILS_BIG);
            netutils_parse_ipv4_addr(items[3], (void *)&info.dns_server,
                                     NETUTILS_BIG);
        } else {
            // check for the correct string
            const char *mode = mp_obj_str_get_str(args[1]);
            if (self->if_id != NETMGR_WIFI_MODE_STA || strcmp("dhcp", mode)) {
                mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
            }
        }
        HAAS_EXCEPTIONS(netmgr_set_ifconfig(self->hdl, &info));
        return mp_const_none;
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(haas_wlan_ifconfig_obj, 1, 2,
                                    haas_wlan_ifconfig);

STATIC mp_obj_t haas_wlan_config(size_t n_args, const mp_obj_t *args,
                                 mp_map_t *kwargs)
{
    if (n_args != 1 && kwargs->used != 0) {
        mp_raise_TypeError(MP_ERROR_TEXT("either pos or kw args are allowed"));
    }

    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    bool is_wifi = self->if_id == NETMGR_WIFI_MODE_AP ||
                   self->if_id == NETMGR_WIFI_MODE_STA;

    netmgr_config_t cfg = { 0 };
    if (is_wifi) {
        HAAS_EXCEPTIONS(netmgr_get_config(self->if_id, &cfg));
    }

    mp_obj_t val = mp_const_none;

    // if (kwargs->used != 0) {
    //     if (!is_wifi) {
    //         goto unknown;
    //     }

    //     for (size_t i = 0; i < kwargs->alloc; i++) {
    //         if (mp_map_slot_is_filled(kwargs, i)) {
    //             int req_if = -1;

    //             switch ((uintptr_t)kwargs->table[i].key) {
    //                 case QS(MP_QSTR_mac): {
    //                     mp_buffer_info_t bufinfo;
    //                     mp_get_buffer_raise(kwargs->table[i].value, &bufinfo,
    //                     MP_BUFFER_READ); if (bufinfo.len != 6) {
    //                         mp_raise_ValueError(MP_ERROR_TEXT("invalid buffer
    //                         length"));
    //                     }
    //                     HAAS_EXCEPTIONS(wifi_set_mac(self->hdl,
    //                     bufinfo.buf)); break;
    //                 }
    //                 case QS(MP_QSTR_essid): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     size_t len;
    //                     const char *s =
    //                     mp_obj_str_get_data(kwargs->table[i].value, &len);
    //                     len = MIN(len, sizeof(cfg.ap.ssid));
    //                     memcpy(cfg.ap.ssid, s, len);
    //                     cfg.ap.ssid_len = len;
    //                     break;
    //                 }
    //                 case QS(MP_QSTR_hidden): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     cfg.ap.ssid_hidden =
    //                     mp_obj_is_true(kwargs->table[i].value); break;
    //                 }
    //                 case QS(MP_QSTR_authmode): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     cfg.ap.authmode =
    //                     mp_obj_get_int(kwargs->table[i].value); break;
    //                 }
    //                 case QS(MP_QSTR_password): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     size_t len;
    //                     const char *s =
    //                     mp_obj_str_get_data(kwargs->table[i].value, &len);
    //                     len = MIN(len, sizeof(cfg.ap.password) - 1);
    //                     memcpy(cfg.ap.password, s, len);
    //                     cfg.ap.password[len] = 0;
    //                     break;
    //                 }
    //                 case QS(MP_QSTR_channel): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     cfg.ap.channel =
    //                     mp_obj_get_int(kwargs->table[i].value); break;
    //                 }
    //                 case QS(MP_QSTR_dhcp_hostname): {
    //                     const char *s =
    //                     mp_obj_str_get_str(kwargs->table[i].value);
    //                     HAAS_EXCEPTIONS(tcpip_adapter_set_hostname(self->if_id,
    //                     s)); break;
    //                 }
    //                 case QS(MP_QSTR_max_clients): {
    //                     req_if = NETMGR_WIFI_MODE_AP;
    //                     cfg.ap.max_connection =
    //                     mp_obj_get_int(kwargs->table[i].value); break;
    //                 }
    //                 default:
    //                     goto unknown;
    //             }

    //             // We post-check interface requirements to save on code size
    //             if (req_if >= 0) {
    //                 require_if(args[0], req_if);
    //             }
    //         }
    //     }

    //     HAAS_EXCEPTIONS(haas_wlan_wifi_set_config(self->if_id, &cfg));
    //     return mp_const_none;
    // }

    // // Get config
    // if (n_args != 2) {
    //     mp_raise_TypeError(MP_ERROR_TEXT("can query only one param"));
    // }

    // int req_if = -1;

    // switch ((uintptr_t)args[1]) {
    //     case QS(MP_QSTR_mac): {
    //         uint8_t mac[6];
    //         switch (self->if_id) {
    //             case NETMGR_WIFI_MODE_AP: // fallthrough intentional
    //             case NETMGR_WIFI_MODE_STA:
    //                 HAAS_EXCEPTIONS(wifi_get_mac(self->hdl, mac));
    //                 return mp_obj_new_bytes(mac, sizeof(mac));

    //             default:
    //                 goto unknown;
    //         }
    //     }
    //         break;
    //     case QS(MP_QSTR_essid):
    //         switch (self->if_id) {
    //             case NETMGR_WIFI_MODE_STA:
    //                 val = mp_obj_new_str((char *)cfg.sta.ssid, strlen((char
    //                 *)cfg.sta.ssid)); break;
    //             case NETMGR_WIFI_MODE_AP:
    //                 val = mp_obj_new_str((char *)cfg.ap.ssid,
    //                 cfg.ap.ssid_len); break;
    //             default:
    //                 req_if = NETMGR_WIFI_MODE_AP;
    //         }
    //         break;
    //     case QS(MP_QSTR_hidden):
    //         req_if = NETMGR_WIFI_MODE_AP;
    //         val = mp_obj_new_bool(cfg.ap.ssid_hidden);
    //         break;
    //     case QS(MP_QSTR_authmode):
    //         req_if = NETMGR_WIFI_MODE_AP;
    //         val = MP_OBJ_NEW_SMALL_INT(cfg.ap.authmode);
    //         break;
    //     case QS(MP_QSTR_channel):
    //         req_if = NETMGR_WIFI_MODE_AP;
    //         val = MP_OBJ_NEW_SMALL_INT(cfg.ap.channel);
    //         break;
    //     case QS(MP_QSTR_dhcp_hostname): {
    //         const char *s;
    //         HAAS_EXCEPTIONS(tcpip_adapter_get_hostname(self->if_id, &s));
    //         val = mp_obj_new_str(s, strlen(s));
    //         break;
    //     }
    //     case QS(MP_QSTR_max_clients): {
    //         val = MP_OBJ_NEW_SMALL_INT(cfg.ap.max_connection);
    //         break;
    //     }
    //     default:
    //         goto unknown;
    // }

    // // We post-check interface requirements to save on code size
    // if (req_if >= 0) {
    //     require_if(args[0], req_if);
    // }

    return val;

unknown:
    mp_raise_ValueError(MP_ERROR_TEXT("unknown config param"));
}
MP_DEFINE_CONST_FUN_OBJ_KW(haas_wlan_config_obj, 1, haas_wlan_config);

STATIC const mp_rom_map_elem_t wlan_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&haas_wlan_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&haas_wlan_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&haas_wlan_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&haas_wlan_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&haas_wlan_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected),
      MP_ROM_PTR(&haas_wlan_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&haas_wlan_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&haas_wlan_ifconfig_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

const mp_obj_type_t wlan_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_WLAN,
    .locals_dict = (mp_obj_t)&wlan_if_locals_dict,
};

STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&wlan_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&get_wlan_obj) },

#if MODNETWORK_INCLUDE_CONSTANTS
    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(NETMGR_WIFI_MODE_STA) },
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(NETMGR_WIFI_MODE_AP) },

#if 0
    { MP_ROM_QSTR(MP_QSTR_MODE_11B), MP_ROM_INT(WIFI_PROTOCOL_11B) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11G), MP_ROM_INT(WIFI_PROTOCOL_11G) },
    { MP_ROM_QSTR(MP_QSTR_MODE_11N), MP_ROM_INT(WIFI_PROTOCOL_11N) },

    { MP_ROM_QSTR(MP_QSTR_AUTH_OPEN), MP_ROM_INT(WIFI_AUTH_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WEP), MP_ROM_INT(WIFI_AUTH_WEP) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_PSK), MP_ROM_INT(WIFI_AUTH_WPA_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_PSK), MP_ROM_INT(WIFI_AUTH_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA_WPA2_PSK), MP_ROM_INT(WIFI_AUTH_WPA_WPA2_PSK) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_WPA2_ENTERPRISE), MP_ROM_INT(WIFI_AUTH_WPA2_ENTERPRISE) },
    { MP_ROM_QSTR(MP_QSTR_AUTH_MAX), MP_ROM_INT(WIFI_AUTH_MAX) },

    { MP_ROM_QSTR(MP_QSTR_STAT_IDLE), MP_ROM_INT(STAT_IDLE)},
    { MP_ROM_QSTR(MP_QSTR_STAT_CONNECTING), MP_ROM_INT(STAT_CONNECTING)},
    { MP_ROM_QSTR(MP_QSTR_STAT_GOT_IP), MP_ROM_INT(STAT_GOT_IP)},
    { MP_ROM_QSTR(MP_QSTR_STAT_NO_AP_FOUND), MP_ROM_INT(WIFI_REASON_NO_AP_FOUND)},
    { MP_ROM_QSTR(MP_QSTR_STAT_WRONG_PASSWORD), MP_ROM_INT(WIFI_REASON_AUTH_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_BEACON_TIMEOUT), MP_ROM_INT(WIFI_REASON_BEACON_TIMEOUT)},
    { MP_ROM_QSTR(MP_QSTR_STAT_ASSOC_FAIL), MP_ROM_INT(WIFI_REASON_ASSOC_FAIL)},
    { MP_ROM_QSTR(MP_QSTR_STAT_HANDSHAKE_TIMEOUT), MP_ROM_INT(WIFI_REASON_HANDSHAKE_TIMEOUT)},
#endif
#endif
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals,
                            mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};
