/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2019 <ESPRESSIF SYSTEMS (SHANGHAI) CO., LTD>
 *
 * Permission is hereby granted for use on all ESPRESSIF SYSTEMS products, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include <stdio.h>
#include <string.h>

#if MICROPY_PY_AUDIO

#include "modaudio.h"

#include "py/objstr.h"
#include "py/runtime.h"
#include "esp_audio.h"
#include "audio_mem.h"

const char *verno = "0.5-beta1";

STATIC mp_obj_t audio_mem_info(void)
{
#ifdef CONFIG_SPIRAM_BOOT_INIT
    mp_obj_dict_t *dict = mp_obj_new_dict(3);

    mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_mem_total), MP_OBJ_TO_PTR(mp_obj_new_int(esp_get_free_heap_size())));
    mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_inter), MP_OBJ_TO_PTR(mp_obj_new_int(heap_caps_get_free_size(MALLOC_CAP_INTERNAL))));
    mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_dram), MP_OBJ_TO_PTR(mp_obj_new_int(heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT))));
#else
    mp_obj_dict_t *dict = mp_obj_new_dict(1);
    mp_obj_dict_store(dict, MP_ROM_QSTR(MP_QSTR_mem_total), MP_OBJ_TO_PTR(mp_obj_new_int(esp_get_free_heap_size())));
#endif
    return dict;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(audio_mem_info_obj, audio_mem_info);

STATIC mp_obj_t audio_mod_verno(void)
{
    return mp_obj_new_str(verno, strlen(verno));
}
STATIC MP_DEFINE_CONST_FUN_OBJ_0(audio_mod_verno_obj, audio_mod_verno);

extern const mp_obj_type_t audio_snd_type;
extern const mp_obj_type_t audio_player_type;
extern const mp_obj_type_t audio_recorder_type;

STATIC const mp_rom_map_elem_t audio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audio) },
    { MP_ROM_QSTR(MP_QSTR_mem_info), MP_ROM_PTR(&audio_mem_info_obj) },
    { MP_ROM_QSTR(MP_QSTR_verno), MP_ROM_PTR(&audio_mod_verno_obj) },

    { MP_ROM_QSTR(MP_QSTR_Snd), MP_ROM_PTR(&audio_snd_type) },
    { MP_ROM_QSTR(MP_QSTR_Player), MP_ROM_PTR(&audio_player_type) },
    { MP_ROM_QSTR(MP_QSTR_Recorder), MP_ROM_PTR(&audio_recorder_type) },

    // audio_err_t
    { MP_ROM_QSTR(MP_QSTR_AUDIO_OK), MP_ROM_INT(ESP_ERR_AUDIO_NO_ERROR) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_FAIL), MP_ROM_INT(ESP_ERR_AUDIO_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_NO_INPUT_STREAM), MP_ROM_INT(ESP_ERR_AUDIO_NO_INPUT_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_NO_OUTPUT_STREAM), MP_ROM_INT(ESP_ERR_AUDIO_NO_OUTPUT_STREAM) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_NO_CODEC), MP_ROM_INT(ESP_ERR_AUDIO_NO_CODEC) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_HAL_FAIL), MP_ROM_INT(ESP_ERR_AUDIO_HAL_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_MEMORY_LACK), MP_ROM_INT(ESP_ERR_AUDIO_MEMORY_LACK) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_INVALID_URI), MP_ROM_INT(ESP_ERR_AUDIO_INVALID_URI) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_INVALID_PATH), MP_ROM_INT(ESP_ERR_AUDIO_INVALID_PATH) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_INVALID_PARAMETER), MP_ROM_INT(ESP_ERR_AUDIO_INVALID_PARAMETER) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_NOT_READY), MP_ROM_INT(ESP_ERR_AUDIO_NOT_READY) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_NOT_SUPPORT), MP_ROM_INT(ESP_ERR_AUDIO_NOT_SUPPORT) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_TIMEOUT), MP_ROM_INT(ESP_ERR_AUDIO_TIMEOUT) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_ALREADY_EXISTS), MP_ROM_INT(ESP_ERR_AUDIO_ALREADY_EXISTS) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_LINK_FAIL), MP_ROM_INT(ESP_ERR_AUDIO_LINK_FAIL) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_UNKNOWN), MP_ROM_INT(ESP_ERR_AUDIO_UNKNOWN) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_OPEN), MP_ROM_INT(ESP_ERR_AUDIO_OPEN) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_INPUT), MP_ROM_INT(ESP_ERR_AUDIO_INPUT) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_PROCESS), MP_ROM_INT(ESP_ERR_AUDIO_PROCESS) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_OUTPUT), MP_ROM_INT(ESP_ERR_AUDIO_OUTPUT) },
    { MP_ROM_QSTR(MP_QSTR_AUDIO_CLOSE), MP_ROM_INT(ESP_ERR_AUDIO_CLOSE) },
};

STATIC MP_DEFINE_CONST_DICT(audio_module_globals, audio_module_globals_table);

const mp_obj_module_t mp_module_audio = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audio_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_audio, mp_module_audio, MICROPY_PY_AUDIO);

#endif  // MICROPY_PY_AUDIO
