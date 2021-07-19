#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/runtime0.h"
#include "py/nlr.h"
#include "py/runtime.h"
#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/stream.h"
#include "py/mperrno.h"

#include "k_api.h"
#include "HaasLog.h"
#include "ak_common.h"
#include "ak_vi.h"
#include "ak_mem.h"
#include "videocommon.h"

extern const mp_obj_type_t video_recorder_type;

void video_recorder_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_video_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->mname);
}

static mp_obj_t video_recorder_new(const mp_obj_type_t *type, size_t n_args,
                                   size_t n_kw, const mp_obj_t *args)
{
    mp_video_camera_obj_t* camera_obj = m_new_obj(mp_video_camera_obj_t);

    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_ENOMEM, "obj alloc fail");

    camera_obj->base.type = &video_recorder_type;
    camera_obj->mname = "video-recorder";
    camera_obj->frame_rate = CAMERA_DEFAULT_FPS;
    camera_obj->frame_release = 0;
    camera_obj->camera_idx = -1;
    camera_obj->frame = NULL;

    return MP_OBJ_FROM_PTR(camera_obj);
}

static mp_obj_t video_recorder_open(size_t n_args, const mp_obj_t *args)
{
    int ret = -1;
    int camera_idx = 0;
    mp_video_camera_obj_t *camera_obj;
    int media_type = VIDEO_MEDIA_TYPE_H264;

    if (n_args < 2 || n_args > 3) {
        LOG_E("%s: n_args(%d) is illegal\n", __func__, n_args);
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }

    if ((camera_idx = mp_obj_get_int(args[1])) < 0) {
        LOG_E("%s: camera_idx(%d) is illegal\n", __func__, camera_idx);
        mp_raise_OSError(MP_EINVAL);
        return mp_const_none;
    }

    if (n_args == 3) {
        media_type = mp_obj_get_int(args[2]);
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
        py_rtsp_open(camera_obj->camera_idx, media_type, camera_obj->frame_rate, NULL);
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(video_recorder_open_obj, 2, 3, video_recorder_open);

static mp_obj_t video_recorder_close(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    if (camera_obj->frame) {
        if (camera_obj->camera_idx >= 0)
            py_rtsp_close();
        camera_obj->camera_idx = -1;
        m_del_obj(isp_frame_t, camera_obj->frame);
        camera_obj->frame = NULL;
    }
    m_del_obj(mp_video_camera_obj_t, camera_obj);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_recorder_close_obj, 1, video_recorder_close);

static mp_obj_t video_recorder_pause(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");
    py_rtsp_pause();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_recorder_pause_obj, 1, video_recorder_pause);

static mp_obj_t video_recorder_resume(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    py_rtsp_resume();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_recorder_resume_obj, 1, video_recorder_resume);

static mp_obj_t video_recorder_start(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    py_rtsp_start();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_recorder_start_obj, 1, video_recorder_start);

static mp_obj_t video_recorder_stop(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    py_rtsp_stop();

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_recorder_stop_obj, 1, video_recorder_stop);

static const mp_rom_map_elem_t video_recorder_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Recorder) },
    { MP_OBJ_NEW_QSTR(MP_QSTR_H264), MP_ROM_INT(VIDEO_MEDIA_TYPE_H264) },
    /* H265 or HEVC */
    { MP_OBJ_NEW_QSTR(MP_QSTR_HEVC), MP_ROM_INT(VIDEO_MEDIA_TYPE_HEVC) },
    /* recorder.start() */
    { MP_ROM_QSTR(MP_QSTR_start),    MP_ROM_PTR(&video_recorder_start_obj) },
    /* recorder.stop() */
    { MP_ROM_QSTR(MP_QSTR_stop),     MP_ROM_PTR(&video_recorder_stop_obj) },
    /* recorder.open(camera_idx, media_type)
     * recorder.open(url)
     */
    { MP_ROM_QSTR(MP_QSTR_open),     MP_ROM_PTR(&video_recorder_open_obj) },
    /* recorder.close() */
    { MP_ROM_QSTR(MP_QSTR_close),    MP_ROM_PTR(&video_recorder_close_obj) },
    /* recorder.pause() */
    { MP_ROM_QSTR(MP_QSTR_pause),    MP_ROM_PTR(&video_recorder_pause_obj) },
    /* recorder.resume() */
    { MP_ROM_QSTR(MP_QSTR_resume),   MP_ROM_PTR(&video_recorder_resume_obj) },
};
static MP_DEFINE_CONST_DICT(video_recorder_globals, video_recorder_globals_table);

const mp_obj_type_t video_recorder_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Recorder,
    .print = video_recorder_print,
    .make_new = video_recorder_new,
    .locals_dict = (mp_obj_dict_t *)&video_recorder_globals,
};
