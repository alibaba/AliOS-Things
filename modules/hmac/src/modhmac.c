#include "crc16.h"
#include "hmac_sha256.h"
#include "py/objstr.h"
#include "py/runtime.h"
#include "py/stackctrl.h"
#include "sha256.h"

#if MICROPY_PY_HMAC

STATIC mp_obj_t mod_hmac_sha256(mp_obj_t key_in, mp_obj_t data_in)
{
    if (!mp_obj_is_str(key_in)) {
        mp_raise_TypeError(MP_ERROR_TEXT("key should be string"));
    }

    if (!mp_obj_is_str(data_in)) {
        mp_raise_TypeError(MP_ERROR_TEXT("data should be string"));
    }

    const char *key = mp_obj_str_get_str(key_in);
    const char *data = mp_obj_str_get_str(data_in);

    uint8_t tmp[SHA256_HASH_SIZE] = { 0 };
    size_t len = hmac_sha256(key, strlen(key), data, strlen(data), &tmp, sizeof(tmp));

    char out_str[SHA256_HASH_SIZE * 2 + 1] = { 0 };
    for (int i = 0; i < len; i++) {
        snprintf(&out_str[i * 2], 3, "%02x", tmp[i]);
    }

    return mp_obj_new_str((const char *)out_str, len * 2);
}
MP_DEFINE_CONST_FUN_OBJ_2(mod_hmac_sha256_obj, mod_hmac_sha256);

STATIC const mp_rom_map_elem_t mp_module_hmac_globals_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_hmac) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_sha256), MP_ROM_PTR(&mod_hmac_sha256_obj) },
};

STATIC MP_DEFINE_CONST_DICT(mp_module_hmac_globals, mp_module_hmac_globals_table);

const mp_obj_module_t mp_module_hmac = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&mp_module_hmac_globals,
};

MP_REGISTER_MODULE(MP_QSTR_hmac, mp_module_hmac, MICROPY_PY_HMAC);

#endif  // MICROPY_PY_HMAC