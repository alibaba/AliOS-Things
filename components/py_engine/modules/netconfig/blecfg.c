#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

#include "ulog/ulog.h"

#if PY_BUILD_BLE

#define LOG_TAG "bleNetconfig"

typedef enum {
    BLECFG_WIFIINFO  = 0x1,
    BLECFG_DEVINFO = 0x2,
} blecfg_info_params_t;

STATIC mp_obj_t ble_init(void)
{
    mp_int_t ret = BleCfg_run();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_init_obj, ble_init);

// STATIC mp_obj_t ble_recovery_wifi(void)
// {
//     mp_int_t ret = BleCfg_recovery_wifi();
//     return MP_OBJ_NEW_SMALL_INT(ret);
// }
// MP_DEFINE_CONST_FUN_OBJ_0(ble_recovery_wifi_obj, ble_recovery_wifi);

// STATIC mp_obj_t ble_recovery_devinfo(void)
// {
//     mp_int_t ret = BleCfg_recovery_devinfo();
//     return MP_OBJ_NEW_SMALL_INT(ret);
// }
// MP_DEFINE_CONST_FUN_OBJ_0(ble_recovery_devinfo_obj, ble_recovery_devinfo);

// mp_int_t ble_config_get_wifi_info(amp_wifi_info_t *wifi_info)
// {
//     netmgr_config_t config = { 0 };
//     netmgr_ifconfig_info_t info = { 0 };
//     mp_int_t ap_num;
//     mp_int_t used_ap; /**< ap that is used in the array */

//     netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
//     if (hdl == -1) {
//         LOGE(LOG_TAG, "get wifi info failed!, netmgr_hdl_t = %d", hdl);
//         return BLE_ERR_NET_DEV;
//     }

//     memset(&info, 0, sizeof(info));
//     if (netmgr_get_ifconfig(hdl, &info) == -1) {
//         return BLE_ERR_NET_IFCONFIG;
//     }

//     memset(&config, 0, sizeof(config));
//     if (netmgr_get_config(hdl, &config) == -1) {
//         return BLE_ERR_NET_CONFIG;
//     }
//     ap_num = config.config.wifi_config.ap_num;
//     used_ap = config.config.wifi_config.used_ap;

//     if ((ap_num < MAX_AP_CONFIG_NUM) && (used_ap < ap_num)) {
//         memset(wifi_info->ssid, 0, sizeof(wifi_info->ssid));
//         strncpy(wifi_info->ssid, config.config.wifi_config.config[used_ap].ssid, sizeof(wifi_info->ssid) - 1);
//     } else {
//         return BLE_ERROR_NET_AP_CNT_INVALID;
//     }

//     snprintf(wifi_info->ip, sizeof(wifi_info->ip), "%s", info.ip_addr);
//     memcpy(wifi_info->mac, info.mac, sizeof(wifi_info->mac));
//     wifi_info->rssi = info.rssi;

//     return BLE_ERR_NONE;
// }

// STATIC mp_obj_t ble_info(void)
// {
//     amp_wifi_info_t wifi_info = { 0 };
//     mp_int_t ret = ble_config_get_wifi_info(&wifi_info);
//     if (ret != BLE_ERR_NONE) {
//         LOGE(LOG_TAG, "Failed to get wifi info, ret = %d", ret);
//     }

//     mp_obj_t dict = mp_obj_new_dict(4);
//     mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("SSID", 4),
//                       mp_obj_new_str(wifi_info.ssid, strlen(wifi_info.ssid)));
//     mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("MAC", 3), mp_obj_new_str(wifi_info.mac, 6));
//     mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("IP", 2),
//                       mp_obj_new_str(wifi_info.ip, strlen(wifi_info.ip)));
//     mp_obj_dict_store(MP_OBJ_FROM_PTR(dict), mp_obj_new_str("RSSI", 4), mp_obj_new_int(wifi_info.rssi));

//     return dict;
// }
// MP_DEFINE_CONST_FUN_OBJ_0(ble_info_obj, ble_info);

// STATIC mp_obj_t ble_status(void)
// {
//     netmgr_hdl_t hdl = netmgr_get_dev(WIFI_DEV_PATH);
//     if (hdl == -1) {
//         LOGE(LOG_TAG, "get wifi info failed!, netmgr_hdl_t = %d", hdl);
//         return MP_OBJ_NEW_SMALL_INT(BLE_ERR_NET_DEV);
//     }

//     mp_int_t ret = netmgr_get_state(hdl);
//     return MP_OBJ_NEW_SMALL_INT(ret);
// }
// MP_DEFINE_CONST_FUN_OBJ_0(ble_status_obj, ble_status);

STATIC mp_obj_t blecfg_restore(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = -1;

    uint32_t params = (uint32_t)mp_obj_get_int(args[0]);

    if (BLECFG_WIFIINFO == params) {
        ret = BleCfg_recovery_wifi();
    } else if (BLECFG_DEVINFO == params) {
        ret = BleCfg_recovery_devinfo();
    } else {
        LOGE(LOG_TAG, "error, recovery params abnormal!\n");
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(blecfg_restore_obj, 1, blecfg_restore);

STATIC mp_obj_t blecfg_get(size_t n_args, const mp_obj_t *args)
{
    mp_int_t ret = 0;
    // wait C interface finish
    return MP_OBJ_NEW_SMALL_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(blecfg_get_obj, 1, blecfg_get);

STATIC mp_obj_t blecfg_clear(void)
{
    mp_int_t ret = 0;
    // wait C interface finish
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(blecfg_clear_obj, blecfg_clear);

STATIC const mp_rom_map_elem_t blecfg_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_blecfg) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ble_init_obj) },
    // { MP_ROM_QSTR(MP_QSTR_recovery_wifi), MP_ROM_PTR(&ble_recovery_wifi_obj) },
    // { MP_ROM_QSTR(MP_QSTR_recovery_devinfo), MP_ROM_PTR(&ble_recovery_devinfo_obj) },
    // { MP_ROM_QSTR(MP_QSTR_info), MP_ROM_PTR(&ble_info_obj) },
    // { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&ble_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_restore), MP_ROM_PTR(&blecfg_restore_obj) },
    { MP_ROM_QSTR(MP_QSTR_get), MP_ROM_PTR(&blecfg_get_obj) },
    { MP_ROM_QSTR(MP_QSTR_clear), MP_ROM_PTR(&blecfg_clear_obj) },

    { MP_ROM_QSTR(MP_QSTR_WifiInfo), MP_ROM_INT(BLECFG_WIFIINFO) },
    { MP_ROM_QSTR(MP_QSTR_DevInfo), MP_ROM_INT(BLECFG_DEVINFO) },
};
STATIC MP_DEFINE_CONST_DICT(blecfg_module_globals, blecfg_module_globals_table);

const mp_obj_module_t mp_module_blecfg = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&blecfg_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_blecfg, mp_module_blecfg, PY_BUILD_BLE);

#endif
