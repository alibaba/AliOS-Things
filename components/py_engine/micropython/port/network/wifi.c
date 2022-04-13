#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "amp_network.h"

extern const mp_obj_type_t network_wifi_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_wifi_obj_t;

void wifi_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_wifi_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t wifi_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_wifi_obj_t* driver_obj = m_new_obj(mp_wifi_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &network_wifi_type;
    driver_obj->ModuleName = "wifi";

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wifi_obj_t* driver_obj = (mp_wifi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    amp_wifi_init();
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wifi_obj_open, 1, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wifi_obj_t* driver_obj = (mp_wifi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wifi_obj_close, 1, obj_close);

STATIC mp_obj_t obj_connect(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wifi_obj_t* driver_obj = (mp_wifi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    char *_ssid = (char *)mp_obj_str_get_str(args[1]);
    char *_pwd = (char *)mp_obj_str_get_str(args[2]);
    amp_wifi_connect(_ssid,_pwd);
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wifi_obj_connect, 3, obj_connect);

STATIC mp_obj_t obj_getInfo(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
#if 0
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wifi_obj_t* driver_obj = (mp_wifi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    amp_wifi_info_t wifi_info ;
    ret = amp_get_wifi_info(&wifi_info);
    if(ret != 0){
        LOG_E("modnetmgr: amp_get_wifi_info failed \n ");
    }

    LOG_D(" ssid is %s  \n ",wifi_info.ssid);
    LOG_D(" mac is %s  \n ",wifi_info.mac);
    LOG_D(" ip is %s  \n ",wifi_info.ip);
    LOG_D(" rssi is %d  \n ",wifi_info.rssi);

    mp_obj_t dict = mp_obj_new_dict(4);
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("SSID",4),mp_obj_new_str(wifi_info.ssid,4));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("Address",7),mp_obj_new_str(wifi_info.mac,16));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("IP",2),mp_obj_new_str(wifi_info.mac,32));
    mp_obj_dict_store(MP_OBJ_FROM_PTR(dict),mp_obj_new_str("RSSI",4),mp_obj_new_int(wifi_info.rssi));

    LOG_D("%s:out\n", __func__);
    return dict;
#else
    return mp_const_none;
#endif
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wifi_obj_getInfo, 1, obj_getInfo);

STATIC mp_obj_t obj_disconnect(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_wifi_obj_t* driver_obj = (mp_wifi_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    wifi_service_disconnect();
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(wifi_obj_disconnect, 1, obj_disconnect);

STATIC const mp_rom_map_elem_t wifi_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_WIFI)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&wifi_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&wifi_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_connect), MP_ROM_PTR(&wifi_obj_connect)},
    {MP_ROM_QSTR(MP_QSTR_getInfo), MP_ROM_PTR(&wifi_obj_getInfo)},
    {MP_ROM_QSTR(MP_QSTR_disconnect), MP_ROM_PTR(&wifi_obj_disconnect)},
};

STATIC MP_DEFINE_CONST_DICT(wifi_locals_dict, wifi_locals_dict_table);

const mp_obj_type_t network_wifi_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_WIFI,
    .print = wifi_obj_print,
    .make_new = wifi_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&wifi_locals_dict,
};

