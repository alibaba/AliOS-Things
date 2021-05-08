#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "HaasLog.h"

extern const mp_obj_type_t minicv_datainput_type;
extern const mp_obj_type_t minicv_imagecodec_type;
extern const mp_obj_type_t minicv_imageproc_type;
#if PY_BUILD_AI
extern const mp_obj_type_t minicv_ml_type;
#endif
extern const mp_obj_type_t minicv_ui_type;
extern const mp_obj_type_t minicv_videocodec_type;

STATIC const mp_rom_map_elem_t minicv_module_globals_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_minicv)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DataInput), MP_ROM_PTR(&minicv_datainput_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ImageCodec), MP_ROM_PTR(&minicv_imagecodec_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ImageProc), MP_ROM_PTR(&minicv_imageproc_type)},
#if PY_BUILD_AI
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML), MP_ROM_PTR(&minicv_ml_type)},
#endif
    {MP_OBJ_NEW_QSTR(MP_QSTR_UI), MP_ROM_PTR(&minicv_ui_type)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_VideoCodec), MP_ROM_PTR(&minicv_videocodec_type)},
};

STATIC MP_DEFINE_CONST_DICT(minicv_module_globals, minicv_module_globals_table);

const mp_obj_module_t minicv_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&minicv_module_globals,
};


