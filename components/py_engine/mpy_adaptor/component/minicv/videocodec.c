#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "WrapperIHaasVideoCodec.h"

extern const mp_obj_type_t minicv_videocodec_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    VideoCodecType_t  mType;
    void            *mInstance;
} mp_videocodec_obj_t;

void videocodec_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_videocodec_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t videocodec_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_videocodec_obj_t* driver_obj = m_new_obj(mp_videocodec_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_videocodec_type;
    driver_obj->ModuleName = "HaasCV-VideoCodec";
    driver_obj->mType = VIDEO_CODEC_NONE;
    driver_obj->mInstance = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance != NULL)
    {
        LOG_E("Module has been opened, please clode first\n");
        return mp_const_none;
    }

    driver_obj->mType = (VideoCodecType_t)mp_obj_get_int(args[1]);
    LOG_D("%s:mType = %d;\n", __func__, driver_obj->mType);
    instance = VideoCodecCreateInstance(driver_obj->mType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("MLCreateInstance failed\n");
        return mp_const_none;
    }
    ret = VideoCodecOpen(driver_obj->mInstance);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_open, 2, obj_open);

STATIC mp_obj_t obj_close(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has not been opened, not need close\n");
        return mp_const_none;
    }

    VideoCodecClose(driver_obj->mInstance);
    VideoCodecDestoryInstance(driver_obj->mInstance);
    driver_obj->mType = VIDEO_CODEC_NONE;
    driver_obj->mInstance = NULL;
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_close, 1, obj_close);

STATIC mp_obj_t obj_decodeConfig(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    DecodeConfig_t* config = (DecodeConfig_t*)MP_OBJ_TO_PTR(args[1]);
    ret = VideoCodecDecodeConfig(driver_obj->mInstance, config);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_decodeConfig, 2, obj_decodeConfig);

STATIC mp_obj_t obj_startDecode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    ret = VideoCodecStartDecode(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_startDecode, 1, obj_startDecode);

STATIC mp_obj_t obj_stopDecode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    ret = VideoCodecStopDecode(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_stopDecode, 1, obj_stopDecode);

STATIC mp_obj_t obj_getDecodeImageData(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    ImageBuffer_t* image = NULL;
    void *pkt = (void *)MP_OBJ_TO_PTR(args[1]);
    ret = VideoCodecGetDecodeImageData(driver_obj->mInstance, pkt, &image);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_OBJ_FROM_PTR(image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_getDecodeImageData, 2, obj_getDecodeImageData);

STATIC mp_obj_t obj_encodeConfig(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 2)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    EncodeConfig_t* config = (EncodeConfig_t*)MP_OBJ_TO_PTR(args[1]);
    ret = VideoCodecEncodeConfig(driver_obj->mInstance, config);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_encodeConfig, 2, obj_encodeConfig);

STATIC mp_obj_t obj_startEncode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    ret = VideoCodecStartEncode(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_startEncode, 1, obj_startEncode);

STATIC mp_obj_t obj_stopEncode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }
    ret = VideoCodecStopEncode(driver_obj->mInstance);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_stopEncode, 1, obj_stopEncode);

STATIC mp_obj_t obj_getEncodePacketData(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_videocodec_obj_t* driver_obj = (mp_videocodec_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has been closed, please open first\n");
        return mp_const_none;
    }

    VideoPacket_t *data = NULL;
    ret = VideoCodecGetEncodePacketData(driver_obj->mInstance, &data);

    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_OBJ_FROM_PTR(data);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(videocodec_obj_getEncodePacketData, 1, obj_getEncodePacketData);

STATIC const mp_rom_map_elem_t videocodec_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_VideoCodec)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_VIDEO_CODEC_NONE), MP_ROM_INT(VIDEO_CODEC_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_VIDEO_CODEC_FFMPEG), MP_ROM_INT(VIDEO_CODEC_FFMPEG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_VIDEO_CODEC_HAL), MP_ROM_INT(VIDEO_CODEC_HAL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_VIDEO_CODEC_MAX), MP_ROM_INT(VIDEO_CODEC_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&videocodec_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&videocodec_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_decodeConfig), MP_ROM_PTR(&videocodec_obj_decodeConfig)},
    {MP_ROM_QSTR(MP_QSTR_startDecode), MP_ROM_PTR(&videocodec_obj_startDecode)},
    {MP_ROM_QSTR(MP_QSTR_stopDecode), MP_ROM_PTR(&videocodec_obj_stopDecode)},
    {MP_ROM_QSTR(MP_QSTR_getDecodeImageData), MP_ROM_PTR(&videocodec_obj_getDecodeImageData)},
    {MP_ROM_QSTR(MP_QSTR_encodeConfig), MP_ROM_PTR(&videocodec_obj_encodeConfig)},
    {MP_ROM_QSTR(MP_QSTR_startEncode), MP_ROM_PTR(&videocodec_obj_startEncode)},
    {MP_ROM_QSTR(MP_QSTR_stopEncode), MP_ROM_PTR(&videocodec_obj_stopEncode)},
    {MP_ROM_QSTR(MP_QSTR_getEncodePacketData), MP_ROM_PTR(&videocodec_obj_getEncodePacketData)},
};

STATIC MP_DEFINE_CONST_DICT(videocodec_locals_dict, videocodec_locals_dict_table);

const mp_obj_type_t minicv_videocodec_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_VideoCodec,
    .print = videocodec_obj_print,
    .make_new = videocodec_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&videocodec_locals_dict,
};
