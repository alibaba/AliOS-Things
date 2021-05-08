#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t driver_crypto_type;

// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
} mp_crypto_obj_t;

void crypto_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_crypto_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t crypto_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_crypto_obj_t* driver_obj = m_new_obj(mp_crypto_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &driver_crypto_type;
    driver_obj->ModuleName = "crypto";

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
    mp_crypto_obj_t* driver_obj = (mp_crypto_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(crypto_obj_open, 1, obj_open);

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
    mp_crypto_obj_t* driver_obj = (mp_crypto_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(crypto_obj_close, 1, obj_close);

STATIC mp_obj_t obj_md5(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    uint8_t result[16] = {0};
    char md5[33] = {0};
    int i = 0;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_crypto_obj_t* driver_obj = (mp_crypto_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

	mp_buffer_info_t src;
    mp_get_buffer_raise(args[1], &src, MP_BUFFER_READ);
    LOG_D("%s:src.buf = %p;src.len = %d;\n", __func__, src.buf, src.len);

    //mbedtls_md5(src.buf, src.len, result);
    for(i = 0; i < 16; ++i)
    {
        //num2hex(result[i], &md5[2 * i]);
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(crypto_obj_md5, 2, obj_md5);

STATIC mp_obj_t obj_encrypt(size_t n_args, const mp_obj_t *args)
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
    mp_crypto_obj_t* driver_obj = (mp_crypto_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(crypto_obj_encrypt, 1, obj_encrypt);

STATIC mp_obj_t obj_decrypt(size_t n_args, const mp_obj_t *args)
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
    mp_crypto_obj_t* driver_obj = (mp_crypto_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(crypto_obj_decrypt, 1, obj_decrypt);

STATIC const mp_rom_map_elem_t crypto_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Crypto)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&crypto_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&crypto_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_md5), MP_ROM_PTR(&crypto_obj_md5)},
    {MP_ROM_QSTR(MP_QSTR_encrypt), MP_ROM_PTR(&crypto_obj_encrypt)},
    {MP_ROM_QSTR(MP_QSTR_decrypt), MP_ROM_PTR(&crypto_obj_decrypt)},
};

STATIC MP_DEFINE_CONST_DICT(crypto_locals_dict, crypto_locals_dict_table);

const mp_obj_type_t driver_crypto_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Crypto,
    .print = crypto_obj_print,
    .make_new = crypto_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&crypto_locals_dict,
};

