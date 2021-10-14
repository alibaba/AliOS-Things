#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "py/builtin.h"
#include "py/obj.h"
#include "py/runtime.h"

extern const mp_obj_type_t video_player_type;
extern const mp_obj_type_t video_recorder_type;
extern const mp_obj_type_t video_camera_type;

STATIC const mp_rom_map_elem_t video_locals_dict_table[] = {
    { MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_video) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Player), MP_ROM_PTR(&video_player_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_Recorder), MP_ROM_PTR(&video_recorder_type) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_camera), MP_ROM_PTR(&video_camera_type) },
};

STATIC MP_DEFINE_CONST_DICT(video_locals_dict, video_locals_dict_table);

const mp_obj_module_t video_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&video_locals_dict,
};
