#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/mperrno.h"

#include "k_api.h"
#include "aos/kernel.h"
#include "timer.h"
#include "HaasLog.h"
#include "videocommon.h"

extern const mp_obj_type_t video_camera_type;

void video_camera_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_video_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->mname);
}

static mp_obj_t video_camera_new(const mp_obj_type_t *type, size_t n_args,
                                 size_t n_kw, const mp_obj_t *args)
{
    mp_video_camera_obj_t* camera_obj = m_new_obj(mp_video_camera_obj_t);

    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_ENOMEM, "obj alloc fail");

    camera_obj->base.type = &video_camera_type;
    camera_obj->mname = "video-camera";
    camera_obj->camera_idx = -1;
    camera_obj->frame_rate = CAMERA_DEFAULT_FPS;
    camera_obj->frame_release = 0;
    camera_obj->frame = NULL;

    return MP_OBJ_FROM_PTR(camera_obj);
}

static mp_obj_t video_camera_open(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    int camera_idx = 0;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 2, "args num is illegal");

    if ((camera_idx = mp_obj_get_int(args[1])) < 0) {
        LOG_E("%s: camera_idx(%d) is illegal\n", __func__, camera_idx);
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    if (camera_obj->frame == NULL) {
        camera_obj->frame = m_new_obj(isp_frame_t);
        if (camera_obj->frame) {
            #if MICROPY_GC_CONSERVATIVE_CLEAR
            memset(camera_obj->frame, 0, sizeof(isp_frame_t));
            #endif
            camera_obj->camera_idx = camera_idx;
            camera_obj->frame_release = 0;
        }
        VIDEO_CAMERA_OBJ_INIT_CHK(camera_obj, MP_ENOMEM, "frame alloc failed");
        py_video_camera_open(camera_obj->camera_idx, camera_obj->frame_rate);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_open_obj, 2, video_camera_open);

static mp_obj_t video_camera_close(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    if (camera_obj->frame) {
        if (camera_obj->frame_release) {
            py_video_camera_frame_release(camera_obj->frame);
        }
        camera_obj->frame_release = 0;

        if (camera_obj->camera_idx >= 0) {
            py_video_camera_close(camera_obj->camera_idx);
        }
        camera_obj->camera_idx = -1;

        m_del_obj(isp_frame_t, camera_obj->frame);
        camera_obj->frame = NULL;
        m_del_obj(mp_video_camera_obj_t, camera_obj);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_close_obj, 1, video_camera_close);

static mp_obj_t video_camera_preview(size_t n_args, const mp_obj_t *args)
{
    uint32_t keep_time_second;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 2, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    keep_time_second = mp_obj_get_int(args[1]);

    if (camera_obj->frame == NULL || camera_obj->camera_idx < 0) {
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }

    py_usbcam_video_init(VIDEO_DEV0, VIDEO_CHN0);

    while (keep_time_second != 0) {
        aos_msleep(1000);
        keep_time_second --;
    }
    py_usbcam_video_deinit();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_preview_obj, 2, video_camera_preview);

STATIC mp_obj_t video_camera_capture(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    if (camera_obj->frame != NULL && camera_obj->camera_idx >= 0) {
        if (camera_obj->frame_release) {
            py_video_camera_frame_release(camera_obj->frame);
        }
        if (py_video_camera_frame_get(camera_obj->frame) != 0) {
            return MP_OBJ_FROM_PTR(NULL);
        }
        camera_obj->frame_release = 1;
    }

    return MP_OBJ_FROM_PTR(camera_obj->frame);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_capture_obj, 1, video_camera_capture);

STATIC mp_obj_t video_camera_save(size_t n_args, const mp_obj_t *args)
{
    int pic_typ;
    char *file_prefix;
    isp_frame_t *frame;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 4, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    frame = (isp_frame_t *)MP_OBJ_TO_PTR(args[1]);
    VIDEO_CAMERA_OBJ_CHK(frame, MP_EINVAL, "frame is NULL");

    pic_typ = mp_obj_get_int(args[2]);
    if (pic_typ != VIDEO_MEDIA_TYPE_YUV && pic_typ != VIDEO_MEDIA_TYPE_JPEG) {
        LOG_E("%s, picture type not support\n", __func__);
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }

    file_prefix = (char *)mp_obj_str_get_str(args[3]);
    VIDEO_CAMERA_OBJ_CHK(frame, MP_EINVAL, "save file_prefix is NULL");

    // transfer raw frame to destination picture with type of pic_type;
    // save destination picture to file of file_prefix;
    py_video_camera_save(frame, pic_typ, (const char *)file_prefix);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_save_obj, 4, video_camera_save);

STATIC mp_obj_t video_camera_config_set(size_t n_args, const mp_obj_t *args)
{
    int prod_id;
    void *prod_val;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 3, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    prod_id = mp_obj_get_int(args[1]);
    prod_val = (void *)MP_OBJ_TO_PTR(args[2]);

    // set config according to prod id and value;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_config_set_obj, 3, video_camera_config_set);

STATIC mp_obj_t video_camera_config_get(size_t n_args, const mp_obj_t *args)
{
    int prod_id;
    void *prod_val;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 3, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    prod_id = mp_obj_get_int(args[1]);
    prod_val = (void *)MP_OBJ_TO_PTR(args[2]);

    // get config according to prod id and save value to prod_val;

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(video_camera_config_get_obj, 3, video_camera_config_get);

STATIC const mp_rom_map_elem_t video_camera_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_camera) },
    /* original YUV */
    { MP_OBJ_NEW_QSTR(MP_QSTR_YUV), MP_ROM_INT(VIDEO_MEDIA_TYPE_YUV) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_JPEG), MP_ROM_INT(VIDEO_MEDIA_TYPE_JPEG) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_H264), MP_ROM_INT(VIDEO_MEDIA_TYPE_H264) },
    /* H265 or HEVC */
    { MP_OBJ_NEW_QSTR(MP_QSTR_HEVC), MP_ROM_INT(VIDEO_MEDIA_TYPE_HEVC) },
    /* camera.save(frame, pic_type, path) */
    { MP_ROM_QSTR(MP_QSTR_save),     MP_ROM_PTR(&video_camera_save_obj) },
    /* camera.open(camara_idx) */
    { MP_ROM_QSTR(MP_QSTR_open),     MP_ROM_PTR(&video_camera_open_obj) },
    /* camera.close() */
    { MP_ROM_QSTR(MP_QSTR_close),    MP_ROM_PTR(&video_camera_close_obj) },
    /* camera.preview() */
    { MP_ROM_QSTR(MP_QSTR_preview),  MP_ROM_PTR(&video_camera_preview_obj) },
    /* frame = camera.capture() */
    { MP_ROM_QSTR(MP_QSTR_capture),  MP_ROM_PTR(&video_camera_capture_obj) },
    /* camera.set(proID, val) */
    { MP_ROM_QSTR(MP_QSTR_set),      MP_ROM_PTR(&video_camera_config_set_obj) },
    /* camera.get(proID, val) */
    { MP_ROM_QSTR(MP_QSTR_get),      MP_ROM_PTR(&video_camera_config_get_obj) },
};
STATIC MP_DEFINE_CONST_DICT(video_camera_globals, video_camera_globals_table);

const mp_obj_type_t video_camera_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_camera,
    .print = video_camera_print,
    .make_new = video_camera_new,
    .locals_dict = (mp_obj_dict_t *)&video_camera_globals,
};
