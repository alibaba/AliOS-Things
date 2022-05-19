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

#define LOG_TAG   "mod_netmgr"

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
    if (!wifi_manager) {
        wifi_manager = calloc(sizeof(aos_wifi_manager_t), 1);
        if (!wifi_manager) {
            network_exceptions(MP_ENOMEM);
        }
        wifi_manager->cb = wifi_event_cb;
        wifi_manager->wifi_state = AOS_NET_NOINIT;
        // wifi_manager->wifi_mode = self->if_mode;
        wifi_manager->wifi_mode = AOS_NETWORK_WIFI_STA;
        wifi_manager->is_initialized = false;
        wifi_manager->is_started = false;
    }
    network_exceptions(aos_wifi_init(wifi_manager));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_init_obj, wifi_init);

STATIC mp_obj_t wifi_active()
{
    network_exceptions(aos_wifi_start(wifi_manager));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_active_obj, wifi_active);

STATIC mp_obj_t wifi_deactive()
{
    network_exceptions(aos_wifi_stop(wifi_manager));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_deactive_obj, wifi_deactive);


STATIC mp_obj_t wifi_deinit()
{
    network_exceptions(aos_wifi_deinit(wifi_manager));
    wifi_manager->cb = NULL;
    free(wifi_manager);
    wifi_manager = NULL;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_deinit_obj, wifi_deinit);

STATIC mp_obj_t wifi_connect(mp_obj_t ssid_in, mp_obj_t pwd_in)
{
    const char *_ssid = mp_obj_str_get_str(ssid_in);
    const char *_pwd = mp_obj_str_get_str(pwd_in);

    if (!wifi_manager) {
        wifi_init();
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
MP_DEFINE_CONST_FUN_OBJ_2(netmgr_wifi_connect_obj, wifi_connect);

STATIC mp_obj_t wifi_disconnect()
{
    network_exceptions(aos_wifi_disconnect());
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_disconnect_obj, wifi_disconnect);

STATIC mp_obj_t wifi_get_status()
{
    if (wifi_manager != NULL)
        return mp_obj_new_int(wifi_manager->wifi_state);
    else {
        return mp_obj_new_int(-1);
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_get_status_obj, wifi_get_status);


STATIC mp_obj_t wifi_on(mp_obj_t func)
{
    on_wifi_event_cb = func;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(netmgr_wifi_on_obj, wifi_on);

STATIC mp_obj_t wifi_get_info()
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
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_wifi_get_info_obj, wifi_get_info);

STATIC mp_obj_t aos_ifconfig(mp_obj_t config_in)
{
    aos_ifconfig_info_t info = { 0 };
    info.dhcp_en = get_int_from_dict(config_in, "dhcp_en") == 0 ? false : true;

    const char *ip_addr = get_str_from_dict(config_in, "ip_addr");
    if (ip_addr == NULL) {
        return mp_obj_new_int(-1);
    } else {
        memcpy(info.ip_addr, ip_addr, strlen(ip_addr));
    }

    const char *mask = get_str_from_dict(config_in, "mask");
    if (mask == NULL) {
        return mp_obj_new_int(-1);
    } else {
        memcpy(info.mask, mask, strlen(mask));
    }

    const char *gw = get_str_from_dict(config_in, "gw");
    if (gw == NULL) {
        return mp_obj_new_int(-1);
    } else {
        memcpy(info.gw, gw, strlen(gw));
    }

    const char *dns_server = get_str_from_dict(config_in, "dns_server");
    if (dns_server == NULL) {
        return mp_obj_new_int(-1);
    } else {
        memcpy(info.dns_server, dns_server, strlen(dns_server));
    }

    const char *mac = get_str_from_dict(config_in, "mac");
    if (mac == NULL) {
        return mp_obj_new_int(-1);
    } else {
        memcpy(info.mac, mac, strlen(mac));
    }

    info.rssi = get_int_from_dict(config_in, "rssi");
    mp_int_t ret = aos_net_set_ifconfig(&info);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(netmgr_ifconfig_obj, aos_ifconfig);

STATIC mp_obj_t aos_get_ifconfig()
{
    aos_ifconfig_info_t info = { 0 };
    mp_int_t ret = aos_net_get_ifconfig(&info);
    if (ret != 0) {
        AOS_LOGI(LOG_TAG, "netmgr_get_config failed, ret = %d\n", ret);
        return mp_const_none;
    }

    mp_obj_t config_dict = mp_obj_new_dict(6);
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("ip_addr")), mp_obj_new_strn(info.ip_addr));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("mask")), mp_obj_new_strn(info.mask));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("gateway")), mp_obj_new_strn(info.gw));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("dns")), mp_obj_new_strn(info.dns_server));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("mac")), mp_obj_new_strn(info.mac));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("rssi")), mp_obj_new_int(info.rssi));

    return config_dict;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_get_ifconfig_obj, aos_get_ifconfig);


STATIC const mp_rom_map_elem_t netmgr_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_netmgr) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&netmgr_wifi_init_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&netmgr_wifi_deinit_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&netmgr_wifi_connect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&netmgr_wifi_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&netmgr_wifi_get_status_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&netmgr_wifi_on_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&netmgr_wifi_get_info_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&netmgr_ifconfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getIfconfig), MP_ROM_PTR(&netmgr_get_ifconfig_obj) },
    /* network connect state */
    { MP_ROM_QSTR(MP_QSTR_NET_AP_DISCONNECTING), MP_ROM_INT(AOS_NET_STA_LOST_IP) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_DISCONNECTED), MP_ROM_INT(AOS_NET_STA_DISCONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_CONNECTING), MP_ROM_INT(AOS_NET_STA_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_CONNECTED), MP_ROM_INT(AOS_NET_STA_CONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_NET_IP_OBTAINING), MP_ROM_INT(AOS_NET_STA_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_NET_IP_OBTAINED), MP_ROM_INT(AOS_NET_STA_GOT_IP) },
    { MP_ROM_QSTR(MP_QSTR_NET_FAILED), MP_ROM_INT(AOS_NET_STA_DISCONNECTED) },

    /* network type */
    { MP_ROM_QSTR(MP_QSTR_NET_TYPE_WIFI), MP_ROM_INT(AOS_NETWORK_WIFI) },
    { MP_ROM_QSTR(MP_QSTR_NET_TYPE_CELLULAR), MP_ROM_INT(AOS_NETWORK_CELLULAR) },
    { MP_ROM_QSTR(MP_QSTR_NET_TYPE_ETH), MP_ROM_INT(AOS_NETWORK_ETHERNET) },
};
STATIC MP_DEFINE_CONST_DICT(hapy_netmgr_module_globals, netmgr_module_globals_table);

const mp_obj_module_t netmgr_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&hapy_netmgr_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_netmgr, netmgr_module, MICROPY_PY_NETMGR);
