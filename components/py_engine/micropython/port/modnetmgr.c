#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"

// #include "netmgr.h"

// #include "netmgr_wifi.h"
#include "amp_network.h"


STATIC mp_obj_t netmgr_init(void) {
    
    amp_wifi_init();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_init, netmgr_init);


STATIC mp_obj_t netmgr_get_info(void) {
    amp_wifi_info_t wifi_info ;
    int ret = amp_get_wifi_info(&wifi_info);
    if(ret != 0){
        printf("modnetmgr: amp_get_wifi_info failed \n ");
    }

    printf(" ssid is %s  \n ",wifi_info.ssid);
    printf(" mac is %s  \n ",wifi_info.mac);
    printf(" ip is %s  \n ",wifi_info.ip);
    printf(" rssi is %d  \n ",wifi_info.rssi);

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("SSID",4),mp_obj_new_str(wifi_info.ssid,4));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("Address",7),mp_obj_new_str(wifi_info.mac,16));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("IP",2),mp_obj_new_str(wifi_info.mac,32));
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
    return mp_obj_new_int(amp_get_network_type());
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_get_type, netmgr_get_type);


STATIC mp_obj_t netmgr_get_status(void) {
    
    int status = amp_get_network_status();
    printf(" status is %d \n",status);
    if(status){
        return mp_const_true ;
    }else{
        return mp_const_false ;
    }
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_get_status, netmgr_get_status);

STATIC mp_obj_t connect_wifi(mp_obj_t ssid,mp_obj_t pwd) {
    char* _ssid = mp_obj_str_get_str(ssid);
    char* _pwd = mp_obj_str_get_str(pwd);
    amp_wifi_connect(_ssid,_pwd);
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(netmgr_obj_connect_wifi, connect_wifi);


STATIC mp_obj_t disconnect_wifi(void) {
    wifi_service_disconnect();
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_disconnect_wifi, disconnect_wifi);



STATIC mp_obj_t netmgr_on_event(void) {
    

    printf("%s  is called \n ",__FUNCTION__);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(netmgr_obj_on, netmgr_on_event);





STATIC const mp_rom_map_elem_t netmgr_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_netmgr)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&netmgr_obj_init)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&netmgr_obj_get_info)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getType), MP_ROM_PTR(&netmgr_obj_get_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_getStatus), MP_ROM_PTR(&netmgr_obj_get_status)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_connect), MP_ROM_PTR(&netmgr_obj_connect_wifi)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&netmgr_obj_disconnect_wifi)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_on), MP_ROM_PTR(&netmgr_obj_on)},


};

STATIC MP_DEFINE_CONST_DICT(netmgr_module_globals, netmgr_module_globals_table);

const mp_obj_module_t netmgr_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&netmgr_module_globals,
};


