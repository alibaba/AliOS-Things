#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <uservice/eventid.h>
#include <uservice/uservice.h>

#include "aos_network.h"
#include "netmgr.h"
#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"
#include "utility.h"

#define WIFI_DEV_PATH "/dev/wifi0"

#define LOG_TAG       "mod_netmgr"

static mp_obj_t on_wifi_event_cb = mp_const_none;

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    if (on_wifi_event_cb != mp_const_none && mp_obj_is_callable(on_wifi_event_cb)) {
        int32_t event;
        switch (event_id) {
        case EVENT_NETMGR_WIFI_DISCONNECTED:
            event = CONN_STATE_DISCONNECTED;
            break;

        case EVENT_NETMGR_WIFI_CONNECTED:
            event = CONN_STATE_CONNECTED;
            break;

        case EVENT_NETMGR_WIFI_CONN_TIMEOUT:
            event = CONN_STATE_FAILED;
            break;

        case EVENT_NETMGR_GOT_IP:
            event = CONN_STATE_NETWORK_CONNECTED;
            break;

        default:
            event = CONN_STATE_UNKNOWN;
            break;
        }

        callback_to_python(on_wifi_event_cb, mp_obj_new_int(event));
    }
}

STATIC mp_obj_t hapy_netmgr_init(void)
{
    mp_int_t ret = 0;
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);

    if (hdl >= 0) {
        LOGD(LOG_TAG, "wifi already init by other task");
    } else {
        LOGD(LOG_TAG, "wifi has not been inited");
        ret = event_service_init(NULL);
        if (ret != 0) {
            LOGE(LOG_TAG, "event_service_init failed");
            return ret;
        }
        ret = netmgr_service_init(NULL);
        if (ret != 0) {
            LOGE(LOG_TAG, "netmgr_service_init failed");
            return ret;
        }
        netmgr_set_auto_reconnect(NULL, true);
        ret = netmgr_wifi_set_auto_save_ap(true);
        if (ret != 0) {
            LOGE(LOG_TAG, "netmgr_wifi_set_auto_save_ap failed");
            return ret;
        }
    }

    event_subscribe(EVENT_NETMGR_WIFI_DISCONNECTED, wifi_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_WIFI_CONNECTED, wifi_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_WIFI_CONN_TIMEOUT, wifi_event_cb, NULL);
    event_subscribe(EVENT_NETMGR_GOT_IP, wifi_event_cb, NULL);

    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_init_obj, hapy_netmgr_init);

STATIC mp_obj_t hapy_netmgr_deinit(void)
{
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_deinit_obj, hapy_netmgr_deinit);

STATIC mp_obj_t hapy_netmgr_connect(mp_obj_t ssid_in, mp_obj_t pwd_in)
{
    const char *_ssid = mp_obj_str_get_str(ssid_in);
    const char *_pwd = mp_obj_str_get_str(pwd_in);

    mp_int_t ret = aos_wifi_connect(_ssid, _pwd);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_2(hapy_netmgr_connect_obj, hapy_netmgr_connect);

STATIC mp_obj_t hapy_netmgr_disconnect(void)
{
    mp_int_t ret = aos_wifi_disconnect();
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_disconnect_obj, hapy_netmgr_disconnect);

STATIC mp_obj_t hapy_netmgr_getInfo(void)
{
    aos_wifi_info_t wifi_info = { 0 };
    mp_int_t ret = aos_get_wifi_info(&wifi_info);
    if (ret < 0) {
        LOGI(LOG_TAG, "Failed to get wifi info\n");
        return mp_const_none;
    }

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("ssid")), mp_obj_new_strn(wifi_info.ssid));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("mac")), mp_obj_new_strn(wifi_info.mac));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("ip")), mp_obj_new_strn(wifi_info.ip));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(qstr_from_str("rssi")), mp_obj_new_int(wifi_info.rssi));

    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getInfo_obj, hapy_netmgr_getInfo);

STATIC mp_obj_t hapy_netmgr_getType(void)
{
    AOS_NETWORK_TYPE_E type = aos_get_network_type();
    return mp_obj_new_int(type);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getType_obj, hapy_netmgr_getType);

STATIC mp_obj_t hapy_netmgr_getStatus(void)
{
    mp_int_t ret = aos_wifi_get_state();
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getStatus_obj, hapy_netmgr_getStatus);

STATIC mp_obj_t hapy_netmgr_getDev(void)
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    return mp_obj_new_int(hdl);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getDev_obj, hapy_netmgr_getDev);

STATIC mp_obj_t hapy_netmgr_autoReconnect(mp_obj_t autoReconnect_in)
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    bool isAutoReconnect = mp_obj_is_true(autoReconnect_in);
    netmgr_set_auto_reconnect(hdl, isAutoReconnect);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_autoReconnect_obj, hapy_netmgr_autoReconnect);

STATIC mp_obj_t hapy_netmgr_autoSavaAP(mp_obj_t autoSave_in)
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    bool isAutoSave = mp_obj_is_true(autoSave_in);
    mp_int_t ret = netmgr_wifi_set_auto_save_ap(hdl, isAutoSave);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_autoSavaAP_obj, hapy_netmgr_autoSavaAP);

STATIC mp_obj_t hapy_netmgr_ifconfig(mp_obj_t config_in)
{
    netmgr_ifconfig_info_t info = { 0 };

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

    mp_int_t ret = aos_wifi_set_ifconfig(&info);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_ifconfig_obj, hapy_netmgr_ifconfig);

STATIC mp_obj_t hapy_netmgr_getConfig(void)
{
    netmgr_config_t netmgr_config = { 0 };
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    mp_int_t ret = netmgr_get_config(hdl, &netmgr_config);
    if (ret != 0) {
        LOGI(LOG_TAG, "netmgr_get_config failed, ret = %d\n", ret);
        return mp_const_none;
    }

    netmgr_wifi_config_t wifi_config = netmgr_config.config.wifi_config;
    netmgr_wifi_ap_info_t ap_info = wifi_config.config[wifi_config.used_ap];

    mp_obj_t config_dict = mp_obj_new_dict(7);
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("ap_num")), mp_obj_new_int(wifi_config.ap_num));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("used_ap")), mp_obj_new_int(wifi_config.used_ap));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("ssid")), mp_obj_new_strn(ap_info.ssid));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("pwd")), mp_obj_new_strn(ap_info.pwd));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("bssid")), mp_obj_new_strn(ap_info.bssid));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("ap_power")), mp_obj_new_int(ap_info.ap_power));
    mp_obj_dict_store(config_dict, MP_OBJ_NEW_QSTR(qstr_from_str("channel")), mp_obj_new_int(ap_info.channel));

    mp_obj_t res_dict = mp_obj_new_dict(2);
    mp_obj_dict_store(res_dict, MP_OBJ_NEW_QSTR(qstr_from_str("type")), mp_obj_new_int(netmgr_config.type));
    mp_obj_dict_store(res_dict, MP_OBJ_NEW_QSTR(qstr_from_str("config")), config_dict);

    return res_dict;
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getConfig_obj, hapy_netmgr_getConfig);

STATIC mp_obj_t hapy_netmgr_saveConfig()
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    mp_int_t ret = netmgr_save_config(hdl);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_saveConfig_obj, hapy_netmgr_saveConfig);

STATIC mp_obj_t hapy_netmgr_deleteConfig(mp_obj_t config_in)
{
    netmgr_del_config_t config = { 0 };

    config.type = get_int_from_dict(config_in, "type");
    const char *ssid = get_str_from_dict(config_in, "ssid");
    memcpy(config.config.ssid, ssid, strlen(MP_QSTR_server_side));

    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
    mp_int_t ret = netmgr_del_config(hdl, &config);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_deleteConfig_obj, hapy_netmgr_deleteConfig);

STATIC mp_obj_t hapy_netmgr_getChannelList(mp_obj_t channel_num_in)
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);

    int channel_num = mp_obj_get_int(channel_num_in);
    int *channel_array = (int *)aos_calloc(channel_num, sizeof(int));
    if (channel_array == NULL) {
        return mp_const_none;
    }

    mp_int_t ret = netmgr_wifi_get_channelist(hdl, &channel_array, &channel_num);
    if (ret != 0) {
        aos_free(channel_array);
        LOGE(LOG_TAG, "netmgr_wifi_get_channelist failed, ret = %d\n", ret);
        return mp_const_none;
    }

    vstr_t vstr = { 0 };
    vstr_init_len(&vstr, channel_num * sizeof(int));
    memcpy(vstr.buf, channel_array, channel_num * sizeof(int));
    aos_free(channel_array);

    return mp_obj_new_str_from_vstr(&mp_type_bytes, &vstr);
}
MP_DEFINE_CONST_FUN_OBJ_0(hapy_netmgr_getChannelList_obj, hapy_netmgr_getChannelList);

STATIC mp_obj_t hapy_netmgr_setChannelList(mp_obj_t channels_in)
{
    netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);

    mp_buffer_info_t channels;
    mp_get_buffer_raise(channels_in, &channels, MP_BUFFER_READ);

    mp_int_t ret = netmgr_wifi_set_channelist(hdl, channels.buf, channels.len);
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_setChannelList_obj, hapy_netmgr_setChannelList);

STATIC mp_obj_t disconnect_wifi(void)
{
    mp_int_t ret = aos_wifi_disconnect();
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_disconnect_wifi, disconnect_wifi);

STATIC mp_obj_t hapy_netmgr_on(mp_obj_t func)
{
    on_wifi_event_cb = func;
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(hapy_netmgr_on_obj, hapy_netmgr_on);

STATIC const mp_rom_map_elem_t netmgr_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_netmgr) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&hapy_netmgr_init_obj) },
    // { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&hapy_netmgr_deinit_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&hapy_netmgr_connect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&hapy_netmgr_disconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&hapy_netmgr_getStatus_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getDev), MP_ROM_PTR(&hapy_netmgr_getDev_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_autoReconnect), MP_ROM_PTR(&hapy_netmgr_autoReconnect_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_autoSaveAP), MP_ROM_PTR(&hapy_netmgr_autoSavaAP_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_ifconfig), MP_ROM_PTR(&hapy_netmgr_ifconfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getConfig), MP_ROM_PTR(&hapy_netmgr_getConfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_saveConfig), MP_ROM_PTR(&hapy_netmgr_saveConfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deleteConfig), MP_ROM_PTR(&hapy_netmgr_deleteConfig_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_setChannelList), MP_ROM_PTR(&hapy_netmgr_setChannelList_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getChannelList), MP_ROM_PTR(&hapy_netmgr_getChannelList_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&hapy_netmgr_on_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&hapy_netmgr_getInfo_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_getType), MP_ROM_PTR(&hapy_netmgr_getType_obj) },

    /* network connect state */
    { MP_ROM_QSTR(MP_QSTR_NET_AP_DISCONNECTING), MP_ROM_INT(CONN_STATE_DISCONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_DISCONNECTED), MP_ROM_INT(CONN_STATE_DISCONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_CONNECTING), MP_ROM_INT(CONN_STATE_CONNECTING) },
    { MP_ROM_QSTR(MP_QSTR_NET_AP_CONNECTED), MP_ROM_INT(CONN_STATE_CONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_NET_IP_OBTAINING), MP_ROM_INT(CONN_STATE_OBTAINING_IP) },
    { MP_ROM_QSTR(MP_QSTR_NET_IP_OBTAINED), MP_ROM_INT(CONN_STATE_NETWORK_CONNECTED) },
    { MP_ROM_QSTR(MP_QSTR_NET_FAILED), MP_ROM_INT(CONN_STATE_FAILED) },

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
