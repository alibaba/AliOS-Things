#include "crc16.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stackctrl.h"

#if MICROPY_PY_UTILITY

STATIC mp_obj_t mod_utility_crc16Ibm(mp_obj_t data_in)
{
    const char *data = NULL;
    int32_t size = 0;

    if (mp_obj_is_type(data_in, &mp_type_bytearray)) {
        mp_buffer_info_t bufinfo = { 0 };
        mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_WRITE);
        data = (const char *)bufinfo.buf;
        size = bufinfo.len;
    } else if (mp_obj_is_type(data_in, &mp_type_bytes)) {
        data = mp_obj_str_get_str(data_in);
        size = strlen(data);
    } else {
        mp_raise_TypeError(MP_ERROR_TEXT("data should be bytearray or bytes"));
    }

    return MP_OBJ_NEW_SMALL_INT(crc_ibm((const uint8_t *)data, size));
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_utility_crc16Ibm_obj, mod_utility_crc16Ibm);

STATIC mp_obj_t mod_utility_crc16Modbus(mp_obj_t data_in)
{
    if (!mp_obj_is_type(data_in, &mp_type_bytearray)) {
        mp_raise_TypeError(MP_ERROR_TEXT("data should be bytearray"));
    }

    mp_buffer_info_t bufinfo = { 0 };
    mp_get_buffer_raise(data_in, &bufinfo, MP_BUFFER_WRITE);
    const uint8_t *data = (const uint8_t *)bufinfo.buf;
    int32_t size = bufinfo.len;

    return MP_OBJ_NEW_SMALL_INT(crc_modbus(data, size));
}
MP_DEFINE_CONST_FUN_OBJ_1(mod_utility_crc16Modbus_obj, mod_utility_crc16Modbus);

STATIC const mp_rom_map_elem_t mp_module_utility_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_utility) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_crc16Ibm), MP_ROM_PTR(&mod_utility_crc16Ibm_obj) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_crc16Modbus), MP_ROM_PTR(&mod_utility_crc16Modbus_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_utility_globals, mp_module_utility_globals_table);

const mp_obj_module_t mp_module_utility = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_utility_globals,
};

MP_REGISTER_MODULE(MP_QSTR_utility, mp_module_utility, MICROPY_PY_UTILITY);

#endif  // MICROPY_PY_UTILITY