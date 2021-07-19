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

extern const mp_obj_type_t video_player_type;

void video_player_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    mp_video_camera_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "modName(%s)", self->mname);
}

static mp_obj_t video_player_new(const mp_obj_type_t *type, size_t n_args,
                                 size_t n_kw, const mp_obj_t *args)
{
    mp_video_camera_obj_t* camera_obj = m_new_obj(mp_video_camera_obj_t);

    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_ENOMEM, "obj alloc fail");

    camera_obj->base.type = &video_player_type;
    camera_obj->mname = "video-player";
    camera_obj->camera_idx = -1;
    camera_obj->frame = NULL;

    return MP_OBJ_FROM_PTR(camera_obj);
}

static mp_obj_t video_player_open(size_t n_args, const mp_obj_t *args)
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
        }
    }

    VIDEO_CAMERA_OBJ_INIT_CHK(camera_obj, MP_ENOMEM, "frame alloc failed");

    do {
        sdk_run_config config;
        memset(&config, 0, sizeof(config));
        config.mem_trace_flag = SDK_RUN_NORMAL;
        ak_sdk_init(&config);
    } while (0);


    if (ak_vi_open(camera_obj->camera_idx) != 0) {
        LOG_E("ak_vi_open failed\n");
        mp_raise_OSError(MP_ENOMEM);
        return mp_const_none;
    }

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_open_obj, 2, video_player_open);

static mp_obj_t video_player_close(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    if (camera_obj->frame) {
        if (camera_obj->camera_idx >= 0)
            ak_vi_close(camera_obj->camera_idx);
        camera_obj->camera_idx = -1;
        m_del_obj(isp_frame_t, camera_obj->frame);
        camera_obj->frame = NULL;
    }
    m_del_obj(mp_video_camera_obj_t, camera_obj);

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_close_obj, 1, video_player_close);

static mp_obj_t video_player_start(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_start_obj, 1, video_player_start);

static mp_obj_t video_player_stop(size_t n_args, const mp_obj_t *args)
{
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 1, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    return MP_OBJ_FROM_PTR(camera_obj->frame);
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_stop_obj, 1, video_player_stop);

static mp_obj_t video_player_save(size_t n_args, const mp_obj_t *args)
{
    int pic_typ;
    char *save_path;
    isp_frame_t *frame;
    mp_video_camera_obj_t *camera_obj;

    VIDEO_CAMERA_PARAM_CHK(n_args, 4, "args num is illegal");

    camera_obj = (mp_video_camera_obj_t *)MP_OBJ_TO_PTR(args[0]);
    VIDEO_CAMERA_OBJ_CHK(camera_obj, MP_EINVAL, "obj not init");

    frame = (isp_frame_t *)MP_OBJ_TO_PTR(args[1]);
    pic_typ = mp_obj_get_int(args[2]);
    save_path = (char *)MP_OBJ_TO_PTR(args[3]);

    // transfer raw frame to destination picture with type of pic_type;
    // save destination picture to file of save_path;

    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_save_obj, 4, video_player_save);

static mp_obj_t video_player_config_set(size_t n_args, const mp_obj_t *args)
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
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_config_set_obj, 3, video_player_config_set);

static mp_obj_t video_player_config_get(size_t n_args, const mp_obj_t *args)
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
static MP_DEFINE_CONST_FUN_OBJ_VAR(video_player_config_get_obj, 3, video_player_config_get);

static const mp_rom_map_elem_t video_player_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_Player) },
    /* player.save(path) */
    { MP_ROM_QSTR(MP_QSTR_save),     MP_ROM_PTR(&video_player_save_obj) },
    /* player.open(camera_idx)
     * player.open(url, media_type)
     * player.open(path, media_type)
     */
    { MP_ROM_QSTR(MP_QSTR_open),     MP_ROM_PTR(&video_player_open_obj) },
    /* player.close() */
    { MP_ROM_QSTR(MP_QSTR_close),    MP_ROM_PTR(&video_player_close_obj) },
    /* player.start() */
    { MP_ROM_QSTR(MP_QSTR_start),    MP_ROM_PTR(&video_player_start_obj) },
    /* player.stop() */
    { MP_ROM_QSTR(MP_QSTR_stop),     MP_ROM_PTR(&video_player_stop_obj) },
    /* player.set(proID, val) */
    { MP_ROM_QSTR(MP_QSTR_set),      MP_ROM_PTR(&video_player_config_set_obj) },
    /* player.get(proID, val) */
    { MP_ROM_QSTR(MP_QSTR_get),      MP_ROM_PTR(&video_player_config_get_obj) },
};
static MP_DEFINE_CONST_DICT(video_player_globals, video_player_globals_table);

const mp_obj_type_t video_player_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_Player,
    .print = video_player_print,
    .make_new = video_player_new,
    .locals_dict = (mp_obj_dict_t *)&video_player_globals,
};
