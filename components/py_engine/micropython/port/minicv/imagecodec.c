#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "WrapperIHaasImageCodec.h"

extern const mp_obj_type_t minicv_imagecodec_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    CodecImageType_t mtype;
    void            *mInstance;
} mp_imagecodec_obj_t;

void imagecodec_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_imagecodec_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t imagecodec_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_imagecodec_obj_t* driver_obj = m_new_obj(mp_imagecodec_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_imagecodec_type;
    driver_obj->ModuleName = "minicv-ImageCodec";
    driver_obj->mtype = CODEC_IMAGE_SOURCE_NONE;
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    driver_obj->mtype = (CodecImageType_t)mp_obj_get_int(args[1]);
    LOG_D("%s:mtype = %d;\n", __func__, driver_obj->mtype);
    instance = ImageCodecCreateInstance(driver_obj->mtype);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("ImageCodecCreateInstance failed\n");
        return mp_const_none;
    }

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_open, 2, obj_open);

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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    ImageCodecDestoryInstance(driver_obj->mInstance);
    driver_obj->mtype = CODEC_IMAGE_SOURCE_NONE;
    driver_obj->mInstance = NULL;
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_close, 1, obj_close);

STATIC mp_obj_t obj_imgRead(size_t n_args, const mp_obj_t *args)
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    ImageBuffer_t *image = NULL;
    char *mFileName = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodecImgRead(driver_obj->mInstance, &image, mFileName);
    if (ret != 0)
    {
        LOG_E("ImageCodecImgRead failed mFileName = %s;\n", mFileName);
        return mp_const_none;
    }
    LOG_D("%s:out image->address[0] = %p;image = %p;\n", __func__, image->address[0], image);

    return MP_OBJ_FROM_PTR(image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgRead, 2, obj_imgRead);

STATIC mp_obj_t obj_imgReadMulti(size_t n_args, const mp_obj_t *args)
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    ImageBuffer_t *image = NULL;
    char *mFileName = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodecImgRead(driver_obj->mInstance, &image, mFileName);
    if (ret != 0)
    {
        LOG_E("ImageCodecImgRead failed mFileName = %s;\n", mFileName);
        return mp_const_none;
    }
    LOG_D("%s:out image->address[0] = %p;image = %p;\n", __func__, image->address[0], image);

    return MP_OBJ_FROM_PTR(image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgReadMulti, 2, obj_imgReadMulti);

STATIC mp_obj_t obj_imgWrite(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    ImageBuffer_t *image = (ImageBuffer_t *)MP_OBJ_TO_PTR(args[1]);
    char *mFileName = (char *)mp_obj_str_get_str(args[2]);
    LOG_D("image = %p;\n", image);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodecImgWrite(driver_obj->mInstance, image, mFileName);
    if (ret)
    {
        LOG_E("%s:ImageCodecImgWrite failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgWrite, 3, obj_imgWrite);

STATIC mp_obj_t obj_imgWriteMulti(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    ImageBuffer_t **image = (ImageBuffer_t **)MP_OBJ_TO_PTR(args[1]);
    char *mFileName = (char *)mp_obj_str_get_str(args[2]);
    LOG_D("image = %p;\n", image);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodecImgWriteMulti(driver_obj->mInstance, image, mFileName);
    if (ret)
    {
        LOG_E("%s:ImageCodecImgWriteMulti failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgWriteMulti, 3, obj_imgWriteMulti);

STATIC mp_obj_t obj_imgDecode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    void *addr = (void *)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t **image = (ImageBuffer_t **)MP_OBJ_TO_PTR(args[2]);
    LOG_D("addr = %p;\n", addr);
    LOG_D("image = %p;\n", image);
    ret = ImageCodecImgDecode(driver_obj->mInstance, addr, image);
    if (ret)
    {
        LOG_E("%s:ImageCodecImgDecode failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgDecode, 3, obj_imgDecode);

STATIC mp_obj_t obj_imgDecode2(size_t n_args, const mp_obj_t *args)
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    char *mFileName = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("mFileName = %s;\n", mFileName);
    ImageBuffer_t *image = ImageCodecImgDecode2(driver_obj->mInstance, mFileName);
    if (image == NULL)
    {
        LOG_E("%s:ImageCodecImgDecode2 failed\n", __func__);
    }
    LOG_D("%s:out image = %p;\n", __func__, image);

    return MP_OBJ_FROM_PTR(image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgDecode2, 2, obj_imgDecode2);

STATIC mp_obj_t obj_imgEncode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 3)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    void *addr = (void *)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t **image = (ImageBuffer_t **)MP_OBJ_TO_PTR(args[2]);
    LOG_D("addr = %p;\n", addr);
    LOG_D("image = %p;\n", image);
    ret = ImageCodecImgEncode(driver_obj->mInstance, addr, image);
    if (ret)
    {
        LOG_E("%s:ImageCodecImgEncode failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_imgEncode, 3, obj_imgEncode);

STATIC mp_obj_t obj_haveImageReader(size_t n_args, const mp_obj_t *args)
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    char *mFileName = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodechaveImageReader(driver_obj->mInstance, mFileName);
    if (ret)
    {
        LOG_E("%s:ImageCodechaveImageReader failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_haveImageReader, 2, obj_haveImageReader);

STATIC mp_obj_t obj_haveImageWriter(size_t n_args, const mp_obj_t *args)
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
    mp_imagecodec_obj_t* driver_obj = (mp_imagecodec_obj_t *)self;
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

    char *mFileName = (char *)mp_obj_str_get_str(args[1]);
    LOG_D("mFileName = %s;\n", mFileName);
    ret = ImageCodechaveImageWriter(driver_obj->mInstance, mFileName);
    if (ret)
    {
        LOG_E("%s:ImageCodechaveImageWriter failed\n", __func__);
    }
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imagecodec_obj_haveImageWriter, 2, obj_haveImageWriter);

STATIC const mp_rom_map_elem_t imagecodec_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ImageCodec)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAGE_SOURCE_NONE), MP_ROM_INT(CODEC_IMAGE_SOURCE_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAG_SOURCE_IMAGE_PNG), MP_ROM_INT(CODEC_IMAG_SOURCE_IMAGE_PNG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAG_SOURCE_IMAGE_JPG), MP_ROM_INT(CODEC_IMAG_SOURCE_IMAGE_JPG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAG_SOURCE_IMAGE_BMP), MP_ROM_INT(CODEC_IMAG_SOURCE_IMAGE_BMP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAG_SOURCE_IMAGE_GIF), MP_ROM_INT(CODEC_IMAG_SOURCE_IMAGE_GIF)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_CODEC_IMAG_SOURCE_MAX), MP_ROM_INT(CODEC_IMAG_SOURCE_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&imagecodec_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&imagecodec_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_imgRead), MP_ROM_PTR(&imagecodec_obj_imgRead)},
    {MP_ROM_QSTR(MP_QSTR_imgReadMulti), MP_ROM_PTR(&imagecodec_obj_imgReadMulti)},
    {MP_ROM_QSTR(MP_QSTR_imgWrite), MP_ROM_PTR(&imagecodec_obj_imgWrite)},
    {MP_ROM_QSTR(MP_QSTR_imgWriteMulti), MP_ROM_PTR(&imagecodec_obj_imgWriteMulti)},
    {MP_ROM_QSTR(MP_QSTR_imgDecode), MP_ROM_PTR(&imagecodec_obj_imgDecode)},
    {MP_ROM_QSTR(MP_QSTR_imgDecode2), MP_ROM_PTR(&imagecodec_obj_imgDecode2)},
    {MP_ROM_QSTR(MP_QSTR_imgEncode), MP_ROM_PTR(&imagecodec_obj_imgEncode)},
    {MP_ROM_QSTR(MP_QSTR_haveImageReader), MP_ROM_PTR(&imagecodec_obj_haveImageReader)},
    {MP_ROM_QSTR(MP_QSTR_haveImageWriter), MP_ROM_PTR(&imagecodec_obj_haveImageWriter)},
};

STATIC MP_DEFINE_CONST_DICT(imagecodec_locals_dict, imagecodec_locals_dict_table);

const mp_obj_type_t minicv_imagecodec_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ImageCodec,
    .print = imagecodec_obj_print,
    .make_new = imagecodec_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&imagecodec_locals_dict,
};

