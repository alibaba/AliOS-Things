#include <aos/kernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mbmaster.h"
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "ulog/ulog.h"

#define LOG_TAG "modbus"

static mb_handler_t *mb_handler = NULL;

STATIC mp_obj_t mp_modbus_init(size_t n_args, const mp_obj_t *args)
{
    if (mb_handler != NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Device busy, deinit firstly"));
        return mp_const_none;
    }

    if (n_args == 0) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Args invalid"));
        return mp_const_none;
    }

    uint8_t port = 1;
    uint32_t baud_rate = 9600;
    mb_parity_t parity = MB_PAR_NONE;
    uint32_t timeout = 200;

    if (n_args == 1) {
        port = mp_obj_get_int(args[0]);
    } else if (n_args == 2) {
        port = mp_obj_get_int(args[0]);
        baud_rate = mp_obj_get_int(args[1]);
    } else if (n_args == 3) {
        port = mp_obj_get_int(args[0]);
        baud_rate = mp_obj_get_int(args[1]);
        parity = mp_obj_get_int(args[2]);
    } else if (n_args == 4) {
        port = mp_obj_get_int(args[0]);
        baud_rate = mp_obj_get_int(args[1]);
        parity = mp_obj_get_int(args[2]);
        int t = mp_obj_get_int(args[3]);
        if (t < 0) {
            timeout = AOS_WAIT_FOREVER;
        } else {
            timeout = t;
        }
    }

    if (parity != MB_PAR_NONE && parity != MB_PAR_ODD && parity != MB_PAR_EVEN) {
        mp_raise_OSError(EINVAL);
        return mp_const_none;
    }

    mp_int_t status = mbmaster_rtu_init(&mb_handler, port, baud_rate, parity, timeout);
    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(mp_modbus_init_obj, 0, 4, mp_modbus_init);

STATIC mp_obj_t mp_modbus_deinit(size_t n_args, const mp_obj_t *args)
{
    if (mb_handler == NULL) {
        mp_raise_OSError(EBADF);
        return mp_const_none;
    }

    mp_int_t status = mbmaster_rtu_uninit(mb_handler);
    if (status == MB_SUCCESS) {
        mb_handler = NULL;
    }

    return MP_OBJ_NEW_SMALL_INT(-status);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_modbus_deinit_obj, 0, mp_modbus_deinit);

STATIC mp_obj_t mp_writeHoldingRegister(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 4) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 4 args"));
        return mp_const_none;
    }

    uint32_t timeout;
    uint16_t resp_addr = 0;
    uint8_t exception_code = 0;
    uint16_t resp_data = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t register_addr = mp_obj_get_int(args[1]);
    uint16_t register_value = mp_obj_get_int(args[2]);
    int t = mp_obj_get_int(args[3]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_write_single_register(mb_handler, slave_addr, register_addr, register_value,
                                                        &resp_addr, &resp_data, &exception_code, timeout);
    if (status == MB_SUCCESS && register_value == resp_data) {
        LOGD(LOG_TAG, "write single register ok");
    } else {
        LOGE(LOG_TAG, "write single register error");
    }

    mp_obj_t tuple[4] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_addr),
        mp_obj_new_int_from_uint(resp_data),
        mp_obj_new_int_from_uint(exception_code),
    };

    return mp_obj_new_tuple(4, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_writeHoldingRegister_obj, 4, mp_writeHoldingRegister);

STATIC mp_obj_t mp_writeMultipleHoldingRegisters(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint16_t resp_addr = 0;
    uint16_t resp_quantity = 0;
    uint16_t exception_code = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);
    if (bufinfo.len < reg_quantity) {
        mp_raise_msg(&mp_type_ValueError, MP_ERROR_TEXT("bytearray size should not smaller than "
                                                        "twice of reg_quantity"));
        return mp_const_none;
    }

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status =
        mbmaster_write_multiple_registers(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf, &resp_addr,
                                          &resp_quantity, &exception_code, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "WriteMultipleHoldingRegisters error");
    }

    mp_obj_t tuple[4] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_addr),
        mp_obj_new_int_from_uint(resp_quantity),
        mp_obj_new_int_from_uint(exception_code),
    };

    return mp_obj_new_tuple(4, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_writeMultipleHoldingRegisters_obj, 5, mp_writeMultipleHoldingRegisters);

STATIC mp_obj_t mp_writeCoil(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 4) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 4 args"));
        return mp_const_none;
    }

    uint16_t resp_addr = 0;
    uint16_t resp_value = 0;
    uint8_t exception_code = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t coil_addr = mp_obj_get_int(args[1]);
    uint16_t coil_value = mp_obj_get_int(args[2]);
    uint32_t timeout;
    int t = mp_obj_get_int(args[3]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_write_single_coil(mb_handler, slave_addr, coil_addr, coil_value, &resp_addr,
                                                    &resp_value, &exception_code, timeout);
    if (status != MB_SUCCESS || coil_value != resp_value) {
        LOGE(LOG_TAG, "write single register error");
    }

    mp_obj_t tuple[4] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_addr),
        mp_obj_new_int_from_uint(resp_value),
        mp_obj_new_int_from_uint(exception_code),
    };

    return mp_obj_new_tuple(4, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_writeCoil_obj, 4, mp_writeCoil);

STATIC mp_obj_t mp_writeMultipleCoils(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint16_t resp_addr = 0;
    uint16_t resp_quantity = 0;
    uint16_t exception_code = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_READ);

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_write_multiple_coils(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf,
                                                       &resp_addr, &resp_quantity, &exception_code, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "writeMultipleCoils error");
    }

    mp_obj_t tuple[4] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_addr),
        mp_obj_new_int_from_uint(resp_quantity),
        mp_obj_new_int_from_uint(exception_code),
    };

    return mp_obj_new_tuple(4, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_writeMultipleCoils_obj, 5, mp_writeMultipleCoils);

STATIC mp_obj_t mp_readHoldingRegisters(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint8_t resp_quantity = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_WRITE);

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_read_holding_registers(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf,
                                                         &resp_quantity, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "readHoldingRegisters error");
    }

    mp_obj_t tuple[2] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_quantity),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_readHoldingRegisters_obj, 5, mp_readHoldingRegisters);

STATIC mp_obj_t mp_readInputRegisters(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint16_t resp_quantity = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_WRITE);

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_read_input_registers(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf,
                                                       &resp_quantity, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "readInputRegisters error");
    }

    mp_obj_t tuple[2] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_quantity),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_readInputRegisters_obj, 5, mp_readInputRegisters);

STATIC mp_obj_t mp_readDiscreteInputs(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint16_t resp_quantity = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_WRITE);

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status = mbmaster_read_discrete_inputs(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf,
                                                       &resp_quantity, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "readDiscreteInputs error");
    }

    mp_obj_t tuple[2] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_quantity),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_readDiscreteInputs_obj, 5, mp_readDiscreteInputs);

STATIC mp_obj_t mp_readCoils(size_t n_args, const mp_obj_t *args)
{
    if (n_args != 5) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("Should input 5 args"));
        return mp_const_none;
    }

    uint16_t resp_quantity = 0;
    uint8_t slave_addr = mp_obj_get_int(args[0]);
    uint16_t start_addr = mp_obj_get_int(args[1]);
    uint16_t reg_quantity = mp_obj_get_int(args[2]);

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[3], &bufinfo, MP_BUFFER_WRITE);

    uint32_t timeout;
    int t = mp_obj_get_int(args[4]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_status_t status =
        mbmaster_read_coils(mb_handler, slave_addr, start_addr, reg_quantity, bufinfo.buf, &resp_quantity, timeout);
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "read_coils error");
    }

    mp_obj_t tuple[2] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(resp_quantity),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_readCoils_obj, 5, mp_readCoils);

STATIC mp_obj_t mp_slave_framerecv(size_t n_args, const mp_obj_t *args)
{
    if (mb_handler == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("slave not inited"));
        return mp_const_none;
    }

    // 1. recv frame from master
    MP_THREAD_GIL_EXIT();
    mb_status_t status = mb_handler->frame_recv(mb_handler);
    MP_THREAD_GIL_ENTER();
    if (status == MB_SUCCESS) {
        // 2. disassemble frame
        status = mb_handler->adu_disassemble(mb_handler);
        if (status != MB_SUCCESS) {
            LOGE(LOG_TAG, "Failed to disassemble frame");
        }
    }

    mp_obj_t tuple[2];
    tuple[0] = MP_OBJ_NEW_SMALL_INT(-status);
    tuple[1] =
        status == MB_SUCCESS ? mp_obj_new_bytes(mb_handler->mb_frame_buff, mb_handler->mb_frame_length) : mp_const_none;

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_slave_framerecv_obj, 0, mp_slave_framerecv);

STATIC mp_obj_t mp_slave_framesend(size_t n_args, const mp_obj_t *args)
{
    if (mb_handler == NULL) {
        mp_raise_msg(&mp_type_RuntimeError, MP_ERROR_TEXT("slave not inited"));
        return mp_const_none;
    }

    uint16_t slave_addr = mp_obj_get_int(args[0]);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_READ);

    uint32_t timeout;
    int t = mp_obj_get_int(args[2]);
    if (t < 0) {
        timeout = AOS_WAIT_FOREVER;
    } else {
        timeout = t;
    }

    mb_handler->slave_addr = slave_addr;
    mb_handler->pdu_length = bufinfo.len;

    memcpy(&mb_handler->mb_frame_buff[ADU_SER_PDU_OFF], bufinfo.buf, bufinfo.len);

    mb_status_t status = mb_handler->adu_assemble(mb_handler);

    MP_THREAD_GIL_EXIT();
    status = mb_handler->frame_send(mb_handler, timeout);
    MP_THREAD_GIL_ENTER();
    if (status != MB_SUCCESS) {
        LOGE(LOG_TAG, "frame send failed");
    }

    mp_obj_t tuple[2] = {
        MP_OBJ_NEW_SMALL_INT(-status),
        mp_obj_new_int_from_uint(mb_handler->mb_frame_length),
    };

    return mp_obj_new_tuple(2, tuple);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(mp_slave_framesend_obj, 2, mp_slave_framesend);

STATIC const mp_rom_map_elem_t modbus_module_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_modbus) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_init), MP_ROM_PTR(&mp_modbus_init_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&mp_modbus_deinit_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeHoldingRegister), MP_ROM_PTR(&mp_writeHoldingRegister_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeMultipleHoldingRegisters), MP_ROM_PTR(&mp_writeMultipleHoldingRegisters_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeCoil), MP_ROM_PTR(&mp_writeCoil_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_writeMultipleCoils), MP_ROM_PTR(&mp_writeMultipleCoils_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readHoldingRegisters), MP_ROM_PTR(&mp_readHoldingRegisters_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readInputRegisters), MP_ROM_PTR(&mp_readInputRegisters_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readDiscreteInputs), MP_ROM_PTR(&mp_readDiscreteInputs_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_readCoils), MP_ROM_PTR(&mp_readCoils_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_recv), MP_ROM_PTR(&mp_slave_framerecv_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_send), MP_ROM_PTR(&mp_slave_framesend_obj) },

    { MP_ROM_QSTR(MP_QSTR_PARITY_NONE), MP_ROM_INT(MB_PAR_NONE) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_ODD), MP_ROM_INT(MB_PAR_ODD) },
    { MP_ROM_QSTR(MP_QSTR_PARITY_EVEN), MP_ROM_INT(MB_PAR_EVEN) },
};

STATIC MP_DEFINE_CONST_DICT(modbus_module_globals, modbus_module_globals_table);

const mp_obj_module_t modbus_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&modbus_module_globals,
};
