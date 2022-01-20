#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_AUDIO

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

#define LOG_TAG "uvoice_snd"

static bool is_uvoice_inited = false;

bool get_uvoice_state()
{
    return is_uvoice_inited;
}

void set_uvoice_state(bool state)
{
    is_uvoice_inited = state;
}

void uvoice_snd_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
}

STATIC mp_obj_t uvoice_snd_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    return mp_const_none;
}

STATIC mp_obj_t a2sa_uvoice_snd_init(void)
{
    extern int audio_install_codec_driver();
    mp_int_t ret = audio_install_codec_driver();
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to install codec driver, ret=%d", ret);
        return MP_OBJ_NEW_SMALL_INT(ret);
    }

    ret = uvoice_init();
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to init uvoice, ret=%d", ret);
    } else {
        set_uvoice_state(true);
    }

    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_uvoice_snd_init_obj, a2sa_uvoice_snd_init);

static mp_int_t audio_uninstall_codec_driver()
{
    return 0;
}

STATIC mp_obj_t a2sa_uvoice_snd_deinit(void)
{
    mp_int_t ret = audio_uninstall_codec_driver();
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to deinit uvoice, ret=%d", ret);
        return MP_OBJ_NEW_SMALL_INT(ret);
    }

    ret = uvoice_free();
    if (ret != 0) {
        LOGE(LOG_TAG, "Failed to deinit uvoice, ret=%d", ret);
    } else {
        set_uvoice_state(false);
    }
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(a2sa_uvoice_snd_deinit_obj, a2sa_uvoice_snd_deinit);

STATIC const mp_rom_map_elem_t uvoice_module_snd_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&a2sa_uvoice_snd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&a2sa_uvoice_snd_deinit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(uvoice_module_snd_globals, uvoice_module_snd_globals_table);

const mp_obj_type_t uvoice_snd_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Snd,
    .print = uvoice_snd_print,
    .make_new = uvoice_snd_new,
    .locals_dict = (mp_obj_dict_t *)&uvoice_module_snd_globals,
};

#endif // MICROPY_PY_AUDIO
