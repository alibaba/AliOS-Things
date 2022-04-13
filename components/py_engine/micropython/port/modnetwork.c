#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t network_udp_type;
extern const mp_obj_type_t network_tcp_type;
extern const mp_obj_type_t network_iot_type;
extern const mp_obj_type_t network_mqtt_type;
extern const mp_obj_type_t network_http_type;
extern const mp_obj_type_t network_cellular_type;
extern const mp_obj_type_t network_wifi_type;
// this is the actual C-structure for our new object

STATIC const mp_rom_map_elem_t network_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_network)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_HTTP), MP_ROM_PTR(&network_http_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_MQTT), MP_ROM_PTR(&network_mqtt_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IOT), MP_ROM_PTR(&network_iot_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_TCP), MP_ROM_PTR(&network_tcp_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_UDP), MP_ROM_PTR(&network_udp_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_WIFI), MP_ROM_PTR(&network_wifi_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Cellular), MP_ROM_PTR(&network_cellular_type)},
};

STATIC MP_DEFINE_CONST_DICT(network_locals_dict, network_locals_dict_table);

const mp_obj_module_t network_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&network_locals_dict,
};

