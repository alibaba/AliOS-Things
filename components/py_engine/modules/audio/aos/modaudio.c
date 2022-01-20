#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if MICROPY_PY_AUDIO

#include "py/builtin.h"
#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "ulog/ulog.h"

extern const mp_obj_type_t uvoice_snd_type;
extern const mp_obj_type_t uvoice_player_type;
extern const mp_obj_type_t uvoice_recorder_type;
extern const mp_obj_type_t uvoice_tts_type;

// this is the actual C-structure for our new object
STATIC const mp_rom_map_elem_t uvoice_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audio) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Snd), MP_ROM_PTR(&uvoice_snd_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Player), MP_ROM_PTR(&uvoice_player_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Recorder), MP_ROM_PTR(&uvoice_recorder_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Tts), MP_ROM_PTR(&uvoice_tts_type) },
};

STATIC MP_DEFINE_CONST_DICT(uvoice_locals_dict, uvoice_locals_dict_table);

const mp_obj_module_t mp_module_audio = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&uvoice_locals_dict,
};

MP_REGISTER_MODULE(MP_QSTR_audio, mp_module_audio, MICROPY_PY_AUDIO);

#endif  // MICROPY_PY_AUDIO
