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

#include "HaasLog.h"

#include "amp_network.h"
// #include "netmgr_wifi.h"
// extern   netmgr_conn_t*  g_wifi_conn_info;

#define WIFI_DEV_PATH "/dev/wifi0"

static mp_obj_t on_wifi_connected ;

typedef enum{

    WIFI_CONNECT_SUCC =1 ,

}wifi_callback_reg_func_t;


netmgr_hdl_t hdl ;

static void wifi_event_cb(uint32_t event_id, const void *param, void *context)
{
    printf(" python Got IP\r\n");

    if(mp_obj_is_fun(on_wifi_connected)){
        callback_to_python(on_wifi_connected, mp_const_none);
    }else{
        printf("Error : on_connected : is not function  \n");
    }

}

STATIC mp_obj_t netmgr_init(void) {

    //amp_wifi_init();
    event_service_init(NULL);
    netmgr_service_init(NULL);
    netmgr_set_auto_reconnect(NULL, true);
    netmgr_wifi_set_auto_save_ap(true);

    netmgr_add_dev(WIFI_DEV_PATH);
    hdl = netmgr_get_dev(WIFI_DEV_PATH);
    event_subscribe(EVENT_NETMGR_DHCP_SUCCESS, wifi_event_cb, NULL);

    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_init, netmgr_init);


int amp_get_wifi_info_local(amp_wifi_info_t *wifi_info)
{
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


STATIC mp_obj_t netmgr_get_info(void) {
    amp_wifi_info_t wifi_info ;
    int ret = amp_get_wifi_info_local(&wifi_info);
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
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_get_info, netmgr_get_info);


/*
	0   :   NETWORK_WIFI,
	1   :   NETWORK_CELLULAR,
	2   :   NETWORK_ETHERNET,
	3   :   NETWORK_UNKNOW,
*/

STATIC mp_obj_t netmgr_get_type(void) {
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_get_type, netmgr_get_type);


STATIC mp_obj_t netmgr_get_status(void) {

    //int status = amp_get_network_status();
    int status = netmgr_get_state(hdl);
    printf(" status is %d \n",status);
    if(status == 5){
        return mp_const_true ;
    }else{
        return mp_const_false ;
    }

    return mp_const_false ;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_get_status, netmgr_get_status);


static void wifi_connect_handle(char * args)
{
    netmgr_wifi_connect_params_t* params = (netmgr_wifi_connect_params_t*) args;

    netmgr_wifi_connect(hdl, params);
}


STATIC mp_obj_t connect_wifi(mp_obj_t ssid,mp_obj_t pwd) {
    char* _ssid = mp_obj_str_get_str(ssid);
    char* _pwd = mp_obj_str_get_str(pwd);
    // amp_wifi_connect(_ssid,_pwd);
    netmgr_wifi_connect_params_t *params;
    params = (netmgr_wifi_connect_params_t*) malloc(sizeof(netmgr_wifi_connect_params_t));
    if(params == NULL) {
        LOG_E("%s:%d malloc failed\n", __func__, __LINE__);
        return ;
    }
    memset(params, 0, sizeof(netmgr_wifi_connect_params_t));
    strncpy(params->ssid, _ssid, sizeof(params->ssid)-1);
    params->timeout = 18000;

    strncpy((char* )params->pwd, _pwd, sizeof(params->pwd)-1);


    aos_task_new("wifi_connect_task",wifi_connect_handle, params, 4*1024);
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_2(netmgr_obj_connect_wifi, connect_wifi);


STATIC mp_obj_t disconnect_wifi(void) {
   // wifi_service_disconnect();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_disconnect_wifi, disconnect_wifi);



STATIC mp_obj_t netmgr_on_event(mp_obj_t event_id) {

    int event = mp_obj_get_int(event_id);


    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(netmgr_obj_on, netmgr_on_event);


STATIC mp_obj_t register_wifi_call_back(mp_obj_t id,mp_obj_t func) {

    int callback_id =  mp_obj_get_int(id);
    switch(callback_id){
        case WIFI_CONNECT_SUCC:
            on_wifi_connected = func ;
            break;

        default:
            break;
    }
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(mp_wifi_register_call_back, register_wifi_call_back);





STATIC const mp_rom_map_elem_t netmgr_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_netmgr)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&netmgr_obj_init)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&netmgr_obj_get_info)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getType), MP_ROM_PTR(&netmgr_obj_get_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&netmgr_obj_get_status)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&netmgr_obj_connect_wifi)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&netmgr_obj_disconnect_wifi)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&netmgr_obj_on)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_register_call_back), MP_ROM_PTR(&mp_wifi_register_call_back)},
};

STATIC MP_DEFINE_CONST_DICT(netmgr_module_globals, netmgr_module_globals_table);

const mp_obj_module_t netmgr_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&netmgr_module_globals,
};


