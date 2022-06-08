#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_AUDIO

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

#define LOG_TAG "audio_snd"

void audio_snd_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
}

STATIC mp_obj_t audio_snd_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    return mp_const_none;
}

STATIC mp_obj_t audio_snd_init(void)
{
    mp_int_t ret = 0;
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(audio_snd_init_obj, audio_snd_init);

STATIC mp_obj_t audio_snd_deinit(void)
{
    mp_int_t ret = 0;
    return MP_OBJ_NEW_SMALL_INT(ret);
}
MP_DEFINE_CONST_FUN_OBJ_0(audio_snd_deinit_obj, audio_snd_deinit);

STATIC const mp_rom_map_elem_t audio_module_snd_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&audio_snd_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&audio_snd_deinit_obj) },
};
STATIC MP_DEFINE_CONST_DICT(audio_module_snd_globals, audio_module_snd_globals_table);

const mp_obj_type_t audio_snd_type = {
    .base = { &mp_type_type },
    .name = MP_QSTR_Snd,
    .print = audio_snd_print,
    .make_new = audio_snd_new,
    .locals_dict = (mp_obj_dict_t *)&audio_module_snd_globals,
};

#endif  // MICROPY_PY_AUDIO
