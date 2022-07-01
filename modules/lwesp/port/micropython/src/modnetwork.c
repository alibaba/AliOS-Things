#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/nlr.h"
#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "py/mperrno.h"
#include "shared/netutils/netutils.h"

#include "lwesp/lwesp.h"

typedef enum {
    WIFI_MODE_NULL = 0,  /**< null mode */
    WIFI_MODE_STA,       /**< WiFi station mode */
    WIFI_MODE_AP,        /**< WiFi soft-AP mode */
    WIFI_MODE_APSTA,     /**< WiFi station + soft-AP mode */
    WIFI_MODE_MAX
} wifi_mode_t;


#define WIFI_IF_STA (0)
#define WIFI_IF_AP (1)
void _lwesp_exceptions(int e) {
    return;
}

static inline void lwesp_exceptions(int e) {
    if (e != 0) {
        _lwesp_exceptions(e);
    }
}

#define LWESP_EXCEPTIONS(x) do { lwesp_exceptions(x); } while (0);

typedef struct _wlan_if_obj_t {
    mp_obj_base_t base;
    int if_id;
} wlan_if_obj_t;

const mp_obj_type_t wlan_if_type;
STATIC const wlan_if_obj_t wlan_sta_obj = {{&wlan_if_type}, WIFI_IF_STA};
STATIC const wlan_if_obj_t wlan_ap_obj = {{&wlan_if_type}, WIFI_IF_AP};

// Set to "true" if esp_wifi_start() was called
static bool wifi_started = false;

// Set to "true" if the STA interface is requested to be connected by the
// user, used for automatic reassociation.
static bool wifi_sta_connect_requested = false;

// Set to "true" if the STA interface is connected to wifi and has IP address.
static bool wifi_sta_connected = false;

// Store the current status. 0 means None here, safe to do so as first enum value is WIFI_REASON_UNSPECIFIED=1.
static uint8_t wifi_sta_disconn_reason = 0;

static uint8_t conf_wifi_sta_reconnects = 0;
static uint8_t wifi_sta_reconnects;

STATIC void require_if(mp_obj_t wlan_if, int if_no) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(wlan_if);
    if (self->if_id != if_no) {
        mp_raise_msg(&mp_type_OSError, if_no == WIFI_IF_STA ? MP_ERROR_TEXT("STA required") : MP_ERROR_TEXT("AP required"));
    }
}

STATIC mp_obj_t lwesp_get_wlan(size_t n_args, const mp_obj_t *args) {

    int idx = (n_args > 0) ? mp_obj_get_int(args[0]) : WIFI_IF_STA;
    if (idx == WIFI_IF_STA) {
        return MP_OBJ_FROM_PTR(&wlan_sta_obj);
    } else if (idx == WIFI_IF_AP) {
        mp_raise_ValueError(MP_ERROR_TEXT("not supported now"));
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WLAN interface identifier"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwesp_get_wlan_obj, 0, 1, lwesp_get_wlan);

static lwespr_t lwesp_callback_func(lwesp_evt_t* evt)
{
    return 0;
}

STATIC mp_obj_t lwesp_initialize() {
    static int initialized = 0;
    if (!initialized) {
        LWESP_DEBUGF(LWESP_CFG_DBG_INIT | LWESP_DBG_LVL_SEVERE | LWESP_DBG_TYPE_TRACE, "init lwesp\r\n");

        int res = lwesp_init(lwesp_callback_func, 1);
        if (res != lwespOK) {
            if (res == lwespTIMEOUT) {
                mp_raise_ValueError(MP_ERROR_TEXT("esp8266 init fail, hardware error"));
            } else {
                mp_raise_ValueError(MP_ERROR_TEXT("esp8266 init fail, system error"));
            }
        } else {
            initialized = 1;
        }
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(lwesp_initialize_obj, lwesp_initialize);

STATIC mp_obj_t lwesp_active(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);

    if (n_args > 1) {
        bool active = mp_obj_is_true(args[1]);
        if (!active) {
            if (wifi_started) {
                lwesp_sta_quit(NULL, NULL, 1);
                wifi_started = false;
            }
        } else {
            if (!wifi_started) {
                lwesp_set_wifi_mode(1, NULL, NULL, 1);
                wifi_started = true;
            }
        }
    }

    return  mp_const_true;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwesp_active_obj, 1, 2, lwesp_active);

STATIC mp_obj_t lwesp_connect(size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_ssid, ARG_password, ARG_bssid };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_, MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_bssid, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
    };

    // parse args
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args - 1, pos_args + 1, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);
    uint8_t ssid[32 + 1] = {0};      /**< SSID of target AP. Null terminated string. */
    uint8_t password[64 + 1] = {0};  /**< Password of target AP. Null terminated string.*/
    lwesp_mac_t bssid;     /**< MAC address of target AP*/
    uint8_t bssid_set = 0;

    // configure any parameters that are given
    if (n_args > 1) {
        size_t len;
        const char *p;
        if (args[ARG_ssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_ssid].u_obj, &len);
            memcpy(ssid, p, MIN(len, sizeof(ssid)));
        }
        if (args[ARG_password].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_password].u_obj, &len);
            memcpy(password, p, MIN(len, sizeof(password)));
        }
        if (args[ARG_bssid].u_obj != mp_const_none) {
            p = mp_obj_str_get_data(args[ARG_bssid].u_obj, &len);
            if (len != sizeof(bssid.mac)) {
                mp_raise_ValueError(NULL);
            }
            bssid_set = 1;
            memcpy(bssid.mac, p, sizeof(bssid.mac));
        }
    }

    wifi_sta_reconnects = 0;
    // connect to the WiFi AP
    MP_THREAD_GIL_EXIT();
    if (bssid_set)
        lwesp_sta_join((const char *)ssid, (const char *)password, &bssid, NULL, NULL, 0);
    else 
        lwesp_sta_join((const char *)ssid, (const char *)password, NULL, NULL, NULL, 0);
    MP_THREAD_GIL_ENTER();
    wifi_sta_connect_requested = true;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_KW(lwesp_connect_obj, 1, lwesp_connect);

STATIC mp_obj_t lwesp_disconnect(mp_obj_t self_in) {
    wifi_sta_connect_requested = false;
    lwesp_sta_quit(NULL, NULL, 1);
    return mp_const_none;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwesp_disconnect_obj, lwesp_disconnect);

// Cases similar to ESP8266 user_interface.h
// Error cases are referenced from wifi_err_reason_t in ESP-IDF
enum {
    STAT_IDLE       = 1000,
    STAT_CONNECTING = 1001,
    STAT_GOT_IP     = 1010,
};

STATIC mp_obj_t lwesp_status(size_t n_args, const mp_obj_t *args) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(args[0]);
    if (n_args == 1) {
        if (self->if_id == WIFI_IF_STA) {
            // Case of no arg is only for the STA interface
            if (lwesp_sta_is_joined()) {
                // Happy path, connected with IP
                return MP_OBJ_NEW_SMALL_INT(STAT_GOT_IP);
            } else if (wifi_sta_connect_requested
                       && (lwesp_sta_is_joined() == 0)) {
                // No connection or error, but is requested = Still connecting
                return MP_OBJ_NEW_SMALL_INT(STAT_CONNECTING);
            } else if (wifi_sta_disconn_reason == 0) {
                // No activity, No error = Idle
                return MP_OBJ_NEW_SMALL_INT(STAT_IDLE);
            } else {
                // Simply pass the error through from ESP-identifier
                return MP_OBJ_NEW_SMALL_INT(wifi_sta_disconn_reason);
            }
        }
        return mp_const_none;
    }

    // one argument: return status based on query parameter
    switch ((uintptr_t)args[1]) {
        case (uintptr_t)MP_OBJ_NEW_QSTR(MP_QSTR_rssi): {
            // return signal of AP, only in STA mode
            require_if(args[0], WIFI_IF_STA);
            lwespr_t res;
            lwesp_sta_info_ap_t info;

            res = lwesp_sta_get_ap_info(&info, NULL, NULL, 1);
            if (res != lwespOK) {
                 mp_raise_ValueError(MP_ERROR_TEXT("unknown error"));
                 return mp_const_none;
            }
            return MP_OBJ_NEW_SMALL_INT(info.rssi);
        }
        default:
            mp_raise_ValueError(MP_ERROR_TEXT("unknown status param"));
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwesp_status_obj, 1, 2, lwesp_status);

STATIC mp_obj_t lwesp_scan(mp_obj_t self_in) {
    mp_obj_t list = mp_obj_new_list(0, NULL);
    if (1) {
        size_t count = 0;
        lwesp_ap_t aps[32];
        lwesp_sta_list_ap(NULL, aps, LWESP_ARRAYSIZE(aps), &count, NULL, NULL, 1);
        for (uint16_t i = 0; i < count; i++) {
            mp_obj_tuple_t *t = mp_obj_new_tuple(6, NULL);
            char *x = (char *)memchr(aps[i].ssid, 0, sizeof(aps[i].ssid));
            int ssid_len = x ? x - aps[i].ssid : sizeof(aps[i].ssid);
            t->items[0] = mp_obj_new_bytes((const byte *)aps[i].ssid, ssid_len);
            t->items[1] = mp_obj_new_bytes(aps[i].mac.mac, sizeof(aps[i].mac.mac));
            t->items[2] = mp_const_false;
            t->items[3] = MP_OBJ_NEW_SMALL_INT(aps[i].rssi);
            t->items[4] = mp_const_false;
            t->items[5] = mp_const_false; // XXX hidden?
            mp_obj_list_append(list, MP_OBJ_FROM_PTR(t));
        }
    }
    return list;
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwesp_scan_obj, lwesp_scan);

STATIC mp_obj_t lwesp_isconnected(mp_obj_t self_in) {
    wlan_if_obj_t *self = MP_OBJ_TO_PTR(self_in);
    if (self->if_id == WIFI_IF_STA) {
        return mp_obj_new_bool(lwesp_sta_is_joined());
    }
    return mp_obj_new_bool(0);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_1(lwesp_isconnected_obj, lwesp_isconnected);

STATIC mp_obj_t lwesp_ifconfig(size_t n_args, const mp_obj_t *args) {
    lwespr_t ret;
    lwesp_ip_t ip = {0}, gw = {0}, nm = {0}, ds = {0};
    if (n_args == 1) {
        ret = lwesp_sta_getip(&ip, &gw, &nm, NULL, NULL, 1);
        // get
        if (ret != lwespOK) {
            mp_raise_ValueError(MP_ERROR_TEXT("lwesp_sta_getip"));
        }
        mp_obj_t tuple[4] = {
            netutils_format_ipv4_addr((uint8_t *)ip.addr.ip4.addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)gw.addr.ip4.addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)nm.addr.ip4.addr, NETUTILS_BIG),
            netutils_format_ipv4_addr((uint8_t *)ds.addr.ip4.addr, NETUTILS_BIG),
        };
        return mp_obj_new_tuple(4, tuple);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("not supported"));
    }
    return mp_const_none;
}

MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(lwesp_ifconfig_obj, 1, 2, lwesp_ifconfig);

STATIC mp_obj_t lwesp_config(size_t n_args, const mp_obj_t *args, mp_map_t *kwargs) {
    mp_raise_ValueError(MP_ERROR_TEXT("not supported"));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_KW(lwesp_config_obj, 1, lwesp_config);

STATIC const mp_rom_map_elem_t wlan_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&lwesp_active_obj) },
    { MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&lwesp_connect_obj) },
    { MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&lwesp_disconnect_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&lwesp_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&lwesp_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&lwesp_isconnected_obj) },
    { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&lwesp_config_obj) },
    { MP_ROM_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&lwesp_ifconfig_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wlan_if_locals_dict, wlan_if_locals_dict_table);

const mp_obj_type_t wlan_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_WLAN,
    .locals_dict = (mp_obj_t)&wlan_if_locals_dict,
};


STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&lwesp_initialize_obj) },
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&lwesp_get_wlan_obj) },

    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(WIFI_IF_STA)},
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(WIFI_IF_AP)},
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_lwesp_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};
