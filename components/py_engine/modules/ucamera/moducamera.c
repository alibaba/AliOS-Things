#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "wifi_camera.h"
#include "ucamera_service.h"
#include "ulog/ulog.h"

#define LOG_TAG "MOD_UCAMERA"

STATIC mp_obj_t obj_init(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 2)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    char *camera_type = (char *)mp_obj_str_get_str(args[0]);
    char *camera_url = (char *)mp_obj_str_get_str(args[1]);
    if (!strcmp(camera_type, "wifi")) {
        ret = ucamera_service_init(WIFI_CAMERA_NAME);
    } else {
        ret = -1;
        LOGE(LOG_TAG, "unsupport camera type %s;\n", __func__);
    }

    if (ret < 0) {
        LOGD(LOG_TAG, "init failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}

STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_init, 0, obj_init);

STATIC mp_obj_t obj_saveFrameToJpeg(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    if (n_args < 1)
    {
        LOGE(LOG_TAG, "%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }

    frame_buffer_t *frame = NULL;
    char *path = (char *)mp_obj_str_get_str(args[0]);
    /*get one camera frame*/
    frame = ucamera_service_get_frame();
    if (!frame) {
        LOGE(LOG_TAG, "ucamera get frame fail\n");
        return;
    } else {
        LOGD(LOG_TAG, "ucamera get frame OK!\n");
    }

    /*save frame to jpeg file*/
    ret = ucamera_service_save_frame(frame, path);
    if (ret < 0) {
        LOGE(LOG_TAG, "save image fail\n");
        return;
    } else {
        LOGD(LOG_TAG, "save image to %s successfully!\n", path);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_saveFrameToJpeg, 0, obj_saveFrameToJpeg);

STATIC mp_obj_t obj_uninit(size_t n_args, const mp_obj_t *args)
{
    LOGD(LOG_TAG, "entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;

    ret = ucamera_service_uninit();
    if (ret < 0) {
        LOGD(LOG_TAG, "init failed  %s;\n", __func__);
    }
    return mp_obj_new_int(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ucamera_obj_uninit, 0, obj_uninit);

STATIC const mp_rom_map_elem_t ucamera_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_UCamera)},
    {MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&ucamera_obj_init)},
    {MP_ROM_QSTR(MP_QSTR_uninit), MP_ROM_PTR(&ucamera_obj_uninit)},
    {MP_ROM_QSTR(MP_QSTR_saveFrameToJpeg), MP_ROM_PTR(&ucamera_obj_saveFrameToJpeg)},
};

STATIC MP_DEFINE_CONST_DICT(ucamera_locals_dict, ucamera_locals_dict_table);

const mp_obj_module_t ucamera_module = {
    .base = {&mp_type_module},
    .globals = (mp_obj_dict_t *)&ucamera_locals_dict,
};
