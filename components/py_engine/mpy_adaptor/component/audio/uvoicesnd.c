#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "HaasLog.h"
#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

extern int audio_install_codec_driver();

bool g_is_uvoice_inited = false;

void uvoice_snd_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
}

STATIC mp_obj_t uvoice_snd_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    return mp_const_none;
}

STATIC mp_obj_t a2sa_install_codec_driver(void)
{
    int ret = audio_install_codec_driver();
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_install_codec_driver_obj, a2sa_install_codec_driver);

STATIC mp_obj_t a2sa_uninstall_codec_driver(void)
{
    printf(" !!!! a2sa_uninstall_codec_driver not impleted !!!! \n\n");
    // int ret = audio_uninstall_codec_driver();
    return mp_obj_new_int(0);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_uninstall_codec_driver_obj, a2sa_uninstall_codec_driver);

STATIC mp_obj_t a2sa_uvoice_init(void)
{
    int ret = uvoice_init();
    if (ret != 0) {
        LOG_E("Failed to init uvoice, ret=%d", ret);
    } else {
        g_is_uvoice_inited = true;
    }
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_uvoice_init_obj, a2sa_uvoice_init);

STATIC mp_obj_t a2sa_uvoice_deinit(void)
{
    int ret = uvoice_free();
    if (ret != 0) {
        LOG_E("Failed to deinit uvoice, ret=%d", ret);
    } else {
        g_is_uvoice_inited = false;
    }
    return mp_obj_new_int(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_uvoice_deinit_obj, a2sa_uvoice_deinit);

STATIC const mp_rom_map_elem_t uvoice_module_snd_globals_table[] = {
    {MP_ROM_QSTR(MP_QSTR___del__), MP_ROM_PTR(&a2sa_uninstall_codec_driver_obj)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&a2sa_uvoice_init_obj)},
    {MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&a2sa_uvoice_deinit_obj)},
    {MP_ROM_QSTR(MP_QSTR_install_codec_driver), MP_ROM_PTR(&a2sa_install_codec_driver_obj)},
    {MP_ROM_QSTR(MP_QSTR_uninstall_codec_driver), MP_ROM_PTR(&a2sa_uninstall_codec_driver_obj)},
};

STATIC MP_DEFINE_CONST_DICT(uvoice_module_snd_globals, uvoice_module_snd_globals_table);

const mp_obj_type_t uvoice_snd_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Snd,
    .print = uvoice_snd_print,
    .make_new = uvoice_snd_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_snd_globals,
};
