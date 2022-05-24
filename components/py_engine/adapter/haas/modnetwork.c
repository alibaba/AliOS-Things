#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "aos_network.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "aos_log.h"
#include "utility.h"

#define LOG_TAG   "mod_network"

typedef struct _aos_wifi_obj_t {
    mp_obj_base_t base;
    int if_mode;
} aos_wifi_if_obj_t;
const mp_obj_type_t aos_wifi_if_type;
STATIC const aos_wifi_if_obj_t aos_wifi_sta_obj = {{&aos_wifi_if_type}, AOS_NETWORK_WIFI_STA};
STATIC const aos_wifi_if_obj_t aos_wifi_ap_obj = {{&aos_wifi_if_type}, AOS_NETWORK_WIFI_AP};
STATIC aos_wifi_manager_t *wifi_manager = NULL;
static mp_obj_t on_wifi_event_cb = mp_const_none;

STATIC void _network_exceptions(AOS_NETWORK_ERR_E e)
{
    switch (e) {
    case AOS_ERR_WIFI_NOT_INIT:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Initialized"));
    case AOS_ERR_WIFI_NOT_STARTED:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Started"));
    case AOS_ERR_WIFI_NOT_STOPPED:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Stopped"));
    case AOS_ERR_WIFI_IF:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Interface"));
    case AOS_ERR_WIFI_MODE:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Mode"));
    case AOS_ERR_WIFI_STATE:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal State Error"));
    case AOS_ERR_WIFI_CONN:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal Error"));
    case AOS_ERR_WIFI_NVS:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Internal NVS Error"));
    case AOS_ERR_WIFI_MAC:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid MAC Address"));
    case AOS_ERR_WIFI_SSID:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi SSID Invalid"));
    case AOS_ERR_WIFI_PASSWORD:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Invalid Password"));
    case AOS_ERR_WIFI_TIMEOUT:
        mp_raise_OSError(MP_ETIMEDOUT);
    case AOS_ERR_WIFI_WAKE_FAIL:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Wakeup Failure"));
    case AOS_ERR_WIFI_WOULD_BLOCK:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Would Block"));
    case AOS_ERR_WIFI_NOT_CONNECT:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("Wifi Not Connected"));
    case AOS_ERR_TCPIP_ADAPTER_INVALID_PARAMS:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP Invalid Parameters"));
    case AOS_ERR_TCPIP_ADAPTER_IF_NOT_READY:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP IF Not Ready"));
    case AOS_ERR_TCPIP_ADAPTER_DHCPC_START_FAILED:
        mp_raise_msg(&mp_type_OSError, MP_ERROR_TEXT("TCP/IP DHCP Client Start Failed"));
    case AOS_ERR_TCPIP_ADAPTER_NO_MEM:
        mp_raise_OSError(MP_ENOMEM);
    default:
        mp_raise_msg_varg(&mp_type_RuntimeError, MP_ERROR_TEXT("Wifi Unknown Error 0x%04x"), e);
    }
}
static inline void network_exceptions(AOS_NETWORK_ERR_E e)
{
    if (e != 0) {
        _network_exceptions(e);
    }
}

static void wifi_event_cb(uint32_t event_id, void *context)
{
    if (on_wifi_event_cb != mp_const_none && mp_obj_is_callable(on_wifi_event_cb)) {
        callback_to_python_LoBo(on_wifi_event_cb, MP_OBJ_NEW_SMALL_INT(event_id), NULL);
    }
}

STATIC mp_obj_t wifi_init()
{
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(aos_wifi_init_obj, wifi_init);

STATIC mp_obj_t wifi_active(mp_obj_t self_in, mp_obj_t active)
{
    if (wifi_manager->is_started) {
        return mp_const_none;
    }

    if (mp_obj_is_true(active)) {
        network_exceptions(aos_wifi_start(wifi_manager));
    } else {
        network_exceptions(aos_wifi_stop(wifi_manager));
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(aos_wifi_active_obj, wifi_active);

STATIC mp_obj_t wifi_connect(mp_obj_t self_in, mp_obj_t ssid_in, mp_obj_t pwd_in)
{
    const char *_ssid = mp_obj_str_get_str(ssid_in);
    const char *_pwd = mp_obj_str_get_str(pwd_in);

    if (!wifi_manager) {
        wifi_init(self_in);
        network_exceptions(aos_wifi_start(wifi_manager));
    }
    if (!wifi_manager->is_started) {
        network_exceptions(aos_wifi_start(wifi_manager));
    }

    MP_THREAD_GIL_EXIT();
    network_exceptions(aos_wifi_connect(_ssid, _pwd));
    MP_THREAD_GIL_ENTER();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(aos_wifi_connect_obj, wifi_connect);

STATIC mp_obj_t wifi_disconnect(mp_obj_t self_in)
{
    network_exceptions(aos_wifi_disconnect());
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(aos_wifi_disconnect_obj, wifi_disconnect);

STATIC mp_obj_t wifi_get_status(size_t n_args, const mp_obj_t *args)
{
    if (wifi_manager != NULL)
        return mp_obj_new_int(wifi_manager->wifi_state);
    else {
        return mp_obj_new_int(-1);
    }
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(aos_wifi_get_status_obj, 1, 2, wifi_get_status);

STATIC mp_obj_t wifi_is_connected(mp_obj_t self_in)
{
    if (wifi_manager->wifi_state == AOS_NET_STA_GOT_IP)
        return mp_obj_new_bool(true);
    return mp_obj_new_bool(false);
}
MP_DEFINE_CONST_FUN_OBJ_1(wifi_is_connected_obj, wifi_is_connected);


STATIC mp_obj_t wifi_on(mp_obj_t self_in, mp_obj_t func)
{
    on_wifi_event_cb = func;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(aos_wifi_on_obj, wifi_on);

STATIC mp_obj_t wifi_get_info(mp_obj_t self_in)
{
    aos_wifi_info_t wifi_info = { 0 };
    mp_int_t ret = aos_wifi_get_info(&wifi_info);
    if (ret < 0) {
        AOS_LOGI(LOG_TAG, "Failed to get wifi info\n");
        return mp_const_none;
    }
    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("ssid")), mp_obj_new_strn(wifi_info.ssid));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("mac")), mp_obj_new_strn(wifi_info.mac));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("ip")), mp_obj_new_strn(wifi_info.ip));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("rssi")), mp_obj_new_int(wifi_info.rssi));
    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_1(aos_wifi_get_info_obj, wifi_get_info);

static int netutils_format_ipv4_addr(uint8_t *in_ip, char* out_ip)
{
    mp_uint_t ip_len;
    if (sizeof(in_ip) < 4) {
        AOS_LOGE("network", "invalid ip format.");
        return -1;
    }
    ip_len = snprintf(out_ip, 16, "%u.%u.%u.%u", in_ip[3], in_ip[2], in_ip[1], in_ip[0]);
    AOS_LOGE("network", out_ip);
    return ip_len;
}
#ifndef NETUTILS_IPV4ADDR_BUFSIZE
#define NETUTILS_IPV4ADDR_BUFSIZE    4
#endif

static void netutils_parse_ipv4_addr(mp_obj_t addr_in, uint8_t *out_ip)
{
    size_t addr_len;
    const char *addr_str = mp_obj_str_get_data(addr_in, &addr_len);
    if (addr_len == 0) {
        // special case of no address given
        memset(out_ip, 0, NETUTILS_IPV4ADDR_BUFSIZE);
        return;
    }
    const char *s = addr_str;
    const char *s_top = addr_str + addr_len;
    for (mp_uint_t i = 3; ; i--) {
        mp_uint_t val = 0;
        for (; s < s_top && *s != '.'; s++) {
            val = val * 10 + *s - '0';
        }
        // out_ip[i] = val;
        out_ip[NETUTILS_IPV4ADDR_BUFSIZE - 1 - i] = val;
        if (i == 0 && s == s_top) {
            return;
        } else if (i > 0 && s < s_top && *s == '.') {
            s++;
        } else {
            mp_raise_ValueError(MP_ERROR_TEXT("invalid arguments"));
        }
    }
}

STATIC mp_obj_t aos_ifconfig(size_t n_args, const mp_obj_t *args)
{
    aos_ifconfig_info_t info = { 0 };
    if (n_args == 1) {
        // get
        mp_int_t ret = aos_net_get_ifconfig(&info);
        if (ret != 0) {
            AOS_LOGI(LOG_TAG, "netmgr_get_config failed, ret = %d\n", ret);
            return mp_const_none;
        }

        mp_obj_t tuple[4] = {
            mp_obj_new_strn(info.ip_addr),
            mp_obj_new_strn(info.mask),
            mp_obj_new_strn(info.gw),
            mp_obj_new_strn(info.dns_server),
            mp_obj_new_strn(info.mac),
        };
        return mp_obj_new_tuple(5, tuple);
    } else {
        // set
        if (mp_obj_is_type(args[1], &mp_type_tuple) || mp_obj_is_type(args[1], &mp_type_list)) {
            mp_obj_t *items;
            mp_obj_get_array_fixed_n(args[1], 4, &items);
            netutils_format_ipv4_addr(items[0], (void *)&info.ip_addr);
            netutils_format_ipv4_addr(items[1], (void *)&info.mask);
            netutils_format_ipv4_addr(items[2], (void *)&info.gw);
            netutils_format_ipv4_addr(items[3], (void *)&info.dns_server);
        }
        mp_int_t ret = aos_net_set_ifconfig(&info);
        return mp_obj_new_int(ret);
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(aos_ifconfig_obj, 1, 2, aos_ifconfig);

STATIC const mp_rom_map_elem_t wifi_if_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_active), MP_ROM_PTR(&aos_wifi_active_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&aos_wifi_connect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&aos_wifi_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_status), MP_ROM_PTR(&aos_wifi_get_status_obj) },
    // { MP_ROM_QSTR(MP_QSTR_scan), MP_ROM_PTR(&aos_wifi_scan_obj) },
    { MP_ROM_QSTR(MP_QSTR_isconnected), MP_ROM_PTR(&wifi_is_connected_obj) },
    // { MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&esp_config_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&aos_ifconfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&aos_wifi_on_obj) },
};

STATIC MP_DEFINE_CONST_DICT(wifi_if_locals_dict, wifi_if_locals_dict_table);
const mp_obj_type_t aos_wifi_if_type = {
    { &mp_type_type },
    .name = MP_QSTR_WIFI,
    .locals_dict = (mp_obj_t)&wifi_if_locals_dict,
};

STATIC mp_obj_t get_wifi(size_t n_args, const mp_obj_t *args)
{
    int idx = (n_args > 0) ? mp_obj_get_int(args[0]) : AOS_NETWORK_WIFI_STA;
    if (idx == AOS_NETWORK_WIFI_STA) {
        if (!wifi_manager) {
            wifi_manager = calloc(sizeof(aos_wifi_manager_t), 1);
            if (!wifi_manager) {
                network_exceptions(MP_ENOMEM);
            }
            wifi_manager->cb = wifi_event_cb;
            wifi_manager->wifi_state = AOS_NET_NOINIT;
            wifi_manager->wifi_mode = AOS_NETWORK_WIFI_STA;
            wifi_manager->is_initialized = false;
            wifi_manager->is_started = false;
        }
        network_exceptions(aos_wifi_init(wifi_manager));
        return MP_OBJ_FROM_PTR(&aos_wifi_sta_obj);
    } else if (idx == AOS_NETWORK_WIFI_AP) {
        return MP_OBJ_FROM_PTR(&aos_wifi_ap_obj);
    } else {
        mp_raise_ValueError(MP_ERROR_TEXT("invalid WLAN interface identifier"));
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(aos_get_wifi_obj, 0, 1, get_wifi);



STATIC const mp_rom_map_elem_t mp_module_network_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network) },
    // { MP_ROM_QSTR(MP_QSTR___init__), MP_ROM_PTR(&aos_wifi_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_WLAN), MP_ROM_PTR(&aos_get_wifi_obj) },

    { MP_ROM_QSTR(MP_QSTR_STA_IF), MP_ROM_INT(AOS_NETWORK_WIFI_STA)},
    { MP_ROM_QSTR(MP_QSTR_AP_IF), MP_ROM_INT(AOS_NETWORK_WIFI_AP)},

    { MP_ROM_QSTR(MP_QSTR_STA_STARTED), MP_ROM_INT(AOS_NET_STA_STARTED) },
    { MP_ROM_QSTR(MP_QSTR_STA_CONNECTED), MP_ROM_INT(AOS_NET_STA_CONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_STA_GOT_IP), MP_ROM_INT(AOS_NET_STA_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_STA_LOST_IP), MP_ROM_INT(AOS_NET_STA_LOST_IP) },
    { MP_ROM_QSTR(MP_QSTR_STA_DISCONNECTED), MP_ROM_INT(AOS_NET_STA_DISCONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_STA_STOPED), MP_ROM_INT(AOS_NET_STA_STOPED) },
    { MP_ROM_QSTR(MP_QSTR_STA_UNKNOWN), MP_ROM_INT(AOS_NET_STATE_UNKNOWN) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_network_globals, mp_module_network_globals_table);

const mp_obj_module_t mp_module_network = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_network_globals,
};

MP_REGISTER_MODULE(MP_QSTR_network, mp_module_network, 1);
