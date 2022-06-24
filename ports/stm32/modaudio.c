/**
 ******************************************************************************
 * This file is part of the MicroPython project, http://bbs.01studio.org/
 * Copyright (C), 2020 -2021, 01studio Tech. Co., Ltd.
 * File Name    :
 * Author       :
 * Version      :
 * date         :
 * Description  :
 ******************************************************************************
**/

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/objlist.h"
#include "py/runtime.h"
#include "py/mphal.h"
#include "shared/netutils/netutils.h"
#include "systick.h"
#include "pendsv.h"
#include "portmodules.h"

#if MICROPY_ENABLE_AUDIO

#if MICROPY_HW_WM8978
#include "wm8978.h"
#endif


STATIC const mp_rom_map_elem_t audio_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_audio) },
#if MICROPY_HW_WM8978
    { MP_ROM_QSTR(MP_QSTR_WM8978), MP_ROM_PTR(&audio_wm8978_type) },
#endif
};
STATIC MP_DEFINE_CONST_DICT(audio_module_globals, audio_module_globals_table);

const mp_obj_module_t audio_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&audio_module_globals,
};

/*******************************************************************************/

#endif // MICROPY_ENABLE_AUDIO
