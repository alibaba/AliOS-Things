#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"

extern const mp_obj_type_t uvoice_snd_type;
extern const mp_obj_type_t uvoice_player_type;
extern const mp_obj_type_t uvoice_recorder_type;
extern const mp_obj_type_t uvoice_tts_type;

const char *get_str_from_dict(mp_obj_t dict, const char *key)
{
    mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
    return (char *)mp_obj_str_get_str(value_obj);
}

int get_int_from_dict(mp_obj_t dict, const char *key)
{
    mp_obj_t value_obj = mp_obj_dict_get(dict, mp_obj_new_strn(key));
    return mp_obj_get_int(value_obj);
}

// this is the actual C-structure for our new object
STATIC const mp_rom_map_elem_t uvoice_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audio)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Snd), MP_ROM_PTR(&uvoice_snd_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Player), MP_ROM_PTR(&uvoice_player_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Recorder), MP_ROM_PTR(&uvoice_recorder_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_Tts), MP_ROM_PTR(&uvoice_tts_type)},
};

STATIC MP_DEFINE_CONST_DICT(uvoice_locals_dict, uvoice_locals_dict_table);

const mp_obj_module_t audio_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&uvoice_locals_dict,
};