#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"

#include "netmgr.h"
#include <uservice/uservice.h>
#include <uservice/eventid.h>

#include "lwip/icmp.h"
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/prot/ip.h"
#include "lwip/inet_chksum.h"
#include <aos/ble.h>
#include <atomic.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/gatt.h>
#include <bluetooth/uuid.h>
#include <netmgr.h>
#include <netmgr_wifi.h>
#include <ble_netconfig.h>

#include "HaasLog.h"

// BLE_NetCfg_init set WIFI DEV PATH
#define WIFI_DEV_PATH "/dev/wifi0"

typedef struct amp_wifi_info {
    char ssid[128];
    char mac[6];
    char ip[16];
    int rssi;
} amp_wifi_info_t;

STATIC mp_obj_t ble_config_run(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BleCfg_run();
    if (ret) {
        LOG_E("error: BLE Config Run! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Config Run OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_config_obj_run, ble_config_run);

STATIC mp_obj_t ble_config_recovery_wifi(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BleCfg_recovery_wifi();
    if (ret) {
        LOG_E("error: BLE Config recovery wifi! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Config recovery wifi OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_config_obj_recovery_wifi, ble_config_recovery_wifi);

STATIC mp_obj_t ble_config_recovery_devinfo(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    int ret;
    ret = BleCfg_recovery_devinfo();
    if (ret) {
        LOG_E("error: BLE Config recovery wifi! ret %x\r\n", ret);
        return mp_const_false;
    } else {
        LOG_D("%s:%d BLE Config recovery wifi OK!\n", __func__, __LINE__);
        return mp_const_true;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_config_obj_recovery_devinfo, ble_config_recovery_devinfo);

// get wifi info
int ble_config_get_wifi_info(amp_wifi_info_t *wifi_info)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    netmgr_hdl_t hdl;
    netmgr_config_t config;
    netmgr_ifconfig_info_t info;
    int ap_num;
    int used_ap;                                         /**< ap that is used in the array */

    if((hdl = netmgr_get_dev(WIFI_DEV_PATH)) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

	memset(&info, 0, sizeof(info));
    if(netmgr_get_ifconfig(hdl, &info) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

	memset(&config, 0, sizeof(config));
    if(netmgr_get_config(hdl, &config) == -1) {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }
    ap_num = config.config.wifi_config.ap_num;
    used_ap = config.config.wifi_config.used_ap;

    if((ap_num < MAX_AP_CONFIG_NUM) && (used_ap < ap_num)) {
	    memset(wifi_info->ssid, 0, sizeof(wifi_info->ssid));
	    strncpy(wifi_info->ssid, config.config.wifi_config.config[used_ap].ssid, sizeof(wifi_info->ssid) - 1);
    } else {
        printf(" %s :  %d \r\n",__func__,__LINE__);
        return -1;
    }

    snprintf(wifi_info->ip, sizeof(wifi_info->ip), "%s", info.ip_addr);
	memcpy(wifi_info->mac, info.mac, sizeof(wifi_info->mac));
	wifi_info->rssi = info.rssi;

    return 0;
}

STATIC mp_obj_t ble_config_get_wifi_status(void) {
    amp_wifi_info_t wifi_info ;
    int ret = ble_config_get_wifi_info(&wifi_info);
    if(ret != 0){
        printf("modnetmgr: amp_get_wifi_info failed \n ");
    }

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("SSID",4),mp_obj_new_str(wifi_info.ssid,strlen(wifi_info.ssid)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("MAC",3),mp_obj_new_str(wifi_info.mac,6));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("IP",2),mp_obj_new_str(wifi_info.ip,strlen(wifi_info.ip)));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("RSSI",4),mp_obj_new_int(wifi_info.rssi));

    return dict;
}
MP_DEFINE_CONST_FUN_OBJ_0(ble_config_obj_get_wifi_info, ble_config_get_wifi_status);

STATIC mp_obj_t ble_config_get_status(void)
{
    LOG_D("%s:%d function entry\n", __func__, __LINE__);
    netmgr_hdl_t hdl;
    hdl = netmgr_get_dev(WIFI_DEV_PATH);

    int status = netmgr_get_state(hdl);
    printf(" status is %d \n",status);
    if(status == 5){
        return mp_const_true ;
    }else{
        return mp_const_false ;
    }

    return mp_const_false ;

}
MP_DEFINE_CONST_FUN_OBJ_0(ble_config_obj_get_status, ble_config_get_status);

STATIC const mp_rom_map_elem_t blenetconfig_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ble_netconfig)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_run), MP_ROM_PTR(&ble_config_obj_run)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_recovery_wifi), MP_ROM_PTR(&ble_config_obj_recovery_wifi)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_recovery_devinfo), MP_ROM_PTR(&ble_config_obj_recovery_devinfo)},

    {MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&ble_config_obj_get_wifi_info)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&ble_config_obj_get_status)},
};

STATIC MP_DEFINE_CONST_DICT(blenetconfig_module_globals, blenetconfig_module_globals_table);

const mp_obj_module_t blenetconfig_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&blenetconfig_module_globals,
};


