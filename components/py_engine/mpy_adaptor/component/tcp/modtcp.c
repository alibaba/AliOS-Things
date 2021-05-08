#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"


// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_tcp_obj_t;



STATIC mp_obj_t obj_createSocket(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tcp_obj_t* driver_obj = (mp_tcp_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_tcp_createSocket, 5, obj_createSocket);

STATIC mp_obj_t obj_send(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tcp_obj_t* driver_obj = (mp_tcp_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_tcp_send, 5, obj_send);

STATIC mp_obj_t obj_recv(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_tcp_obj_t* driver_obj = (mp_tcp_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_obj_tcp_recv, 5, obj_recv);



STATIC const mp_rom_map_elem_t tcp_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_tcp)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_createSocket), MP_ROM_PTR(&mp_obj_tcp_createSocket)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_obj_tcp_send)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_recv), MP_ROM_PTR(&mp_obj_tcp_recv)},
};

STATIC MP_DEFINE_CONST_DICT(tcp_module_globals, tcp_module_globals_table);

const mp_obj_module_t tcp_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&tcp_module_globals,
};


