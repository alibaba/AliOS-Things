#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <aos/kernel.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "py/stackctrl.h"

#include "mbmaster.h"
#include "HaasLog.h"

static mb_handler_t *mb_handler = NULL;

STATIC mp_obj_t mp_modbus_init(size_t n_args, const mp_obj_t *args) {

    if(mb_handler != NULL) {
        mp_raise_OSError(EBUSY);
        return mp_const_none; 
    }

    if(n_args == 0) {
        mp_raise_OSError(EINVAL);
        return mp_const_none; 
    }
    
    uint8_t port = 2;
    uint32_t baud_rate = 9600;
    mb_parity_t parity = MB_PAR_NONE;

    if(n_args == 1) {
        port = mp_obj_get_int(args[0]);
    } else if(n_args == 2) {
        port = mp_obj_get_int(args[0]);
        baud_rate = mp_obj_get_int(args[1]);
    } else if(n_args == 3) {
        port = mp_obj_get_int(args[0]);
        baud_rate = mp_obj_get_int(args[1]);
        parity = mp_obj_get_int(args[2]);
    }

    if(parity != MB_PAR_NONE && parity != MB_PAR_ODD && parity != MB_PAR_EVEN) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    mp_int_t status = mbmaster_rtu_init(&mb_handler, port, baud_rate, parity);
    return mp_obj_new_int(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_modbus_init_obj, 0, 3, mp_modbus_init);

STATIC mp_obj_t mp_modbus_deinit(size_t n_args, const mp_obj_t *args) {

    if(mb_handler == NULL) {
        mp_raise_OSError(EBADF);
        return mp_const_none; 
    }
    
    mp_int_t status = mbmaster_rtu_uninit(mb_handler);
    if(status == MB_SUCCESS) {
        mb_handler = NULL;
    }
    
    return mp_obj_new_int(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_modbus_deinit_obj, 0, mp_modbus_deinit);

STATIC mp_obj_t mp_modbus_write(size_t n_args, const mp_obj_t *args) {
    
    if(n_args != 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    } 

    uint16_t  data_resp = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t register_addr = mp_obj_get_int(args[1]);
    uint16_t register_value = mp_obj_get_int(args[2]);
    uint32_t timeout = mp_obj_get_int(args[3]);
    if(timeout < 0) {
        timeout = AOS_WAIT_FOREVER;
    }

    mb_status_t status = mbmaster_write_single_register(mb_handler, slave_addr, register_addr,
                                            register_value, NULL, &data_resp, NULL, timeout);
    if (status == MB_SUCCESS && register_value == data_resp) {
        LOG_D("write single register ok");
    } else {
        LOG_E("write single register error");
    }

    return mp_obj_new_int(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_modbus_write_obj, 4, mp_modbus_write);

STATIC mp_obj_t mp_modbus_read(size_t n_args, const mp_obj_t *args) {
    
    if(n_args != 4) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    } 

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[0], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);
    uint16_t quantity = (uint16_t)bufinfo.len;
    uint8_t *respond_buf = (uint8_t*)bufinfo.buf;

    uint8_t respond_count = 0;
    uint8_t slave_addr = mp_obj_get_int(args[1]);
    uint16_t start_addr = mp_obj_get_int(args[2]);
    uint32_t timeout = mp_obj_get_int(args[3]);
    if(timeout < 0) {
        timeout = AOS_WAIT_FOREVER;
    }

    mb_status_t status = mbmaster_read_holding_registers(mb_handler, slave_addr, start_addr,
                                             quantity, respond_buf, &respond_count, timeout);
    if (status != MB_SUCCESS) {
        LOG_E("read holding register error, status = %d", status);
        return mp_obj_new_int(-status);
    } else {
        return mp_obj_new_int(respond_count);
    }
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_modbus_read_obj, 4, mp_modbus_read);

STATIC const mp_rom_map_elem_t modbus_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_modbus)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_modbus_init_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_modbus_deinit_obj)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_write), MP_ROM_PTR(&mp_modbus_write_obj)}, 
    {MP_OBJ_NEW_QSTR(MP_QSTR_read), MP_ROM_PTR(&mp_modbus_read_obj)}, 

    {MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(MB_PAR_NONE)},
    {MP_ROM_QSTR(MP_QSTR_PARITY_ODD), MP_ROM_INT(MB_PAR_ODD)},
    {MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(MB_PAR_EVEN)},
};

STATIC MP_DEFINE_CONST_DICT(modbus_module_globals, modbus_module_globals_table);

const mp_obj_module_t modbus_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&modbus_module_globals,
};


