#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "WrapperIHaasImageProc.h"

extern const mp_obj_type_t minicv_imageproc_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    ImageProcType_t  mType;
    void            *mInstance;
} mp_imageproc_obj_t;

void imageproc_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_imageproc_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t imageproc_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_imageproc_obj_t* driver_obj = m_new_obj(mp_imageproc_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_imageproc_type;
    driver_obj->ModuleName = "minicv-ImageProc";
    driver_obj->mType = IMAGE_PROC_NONE;
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
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    driver_obj->mType = (ImageProcType_t)mp_obj_get_int(args[1]);
    LOG_D("%s:mType = %d;\n", __func__, driver_obj->mType);
    instance = ImageProcCreateInstance(driver_obj->mType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("ImageProcCreateInstance failed\n");
        return mp_const_none;
    }

    ret = ImageProcOpen(driver_obj->mInstance);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_open, 2, obj_open);

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
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ret = ImageProcClose(driver_obj->mInstance);
    ImageProcDestoryInstance(driver_obj->mInstance);
    driver_obj->mType = IMAGE_PROC_NONE;
    driver_obj->mInstance = NULL;
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_close, 1, obj_close);

STATIC mp_obj_t obj_cvtColor(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    PixelFmt_t dst_format = (PixelFmt_t)mp_obj_get_int(args[2]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[3]);
    LOG_D("src = %p;dst_format = %d; dst = %p;\n", src, dst_format, dst);
    ret = ImageProcCvtColor(driver_obj->mInstance, src, dst_format, &dst);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_cvtColor, 4, obj_cvtColor);

STATIC mp_obj_t obj_resize(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    uint32_t width = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t height = (uint32_t)mp_obj_get_int(args[3]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[4]);
    LOG_D("src = %p;width = %d; height = %d; dst = %p;\n", src, width, height, dst);
    ImageSize_t imgSize;
    imgSize.width = width;
    imgSize.height = height;

    ret = ImageProcResize(driver_obj->mInstance, src, imgSize, &dst);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_resize, 5, obj_resize);

STATIC mp_obj_t obj_imgCopy(size_t n_args, const mp_obj_t *args)
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
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    LOG_D("src = %p; dst = %p;\n", src, dst);

    ret = ImageProcImgCopy(driver_obj->mInstance, src, &dst);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_imgCopy, 3, obj_imgCopy);

STATIC mp_obj_t obj_rectangle(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 6)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    int32_t left = (int32_t)mp_obj_get_int(args[2]);
    int32_t top = (int32_t)mp_obj_get_int(args[3]);
    int32_t right = (int32_t)mp_obj_get_int(args[4]);
    int32_t bottom = (int32_t)mp_obj_get_int(args[5]);
    LOG_D("src = %p;left = %d; top = %d; right = %d;bottom = %d;\n", src, left, top, right, bottom);

    ret = ImageProcRectangle(driver_obj->mInstance, src, left, top, right, bottom);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_rectangle, 6, obj_rectangle);

STATIC mp_obj_t obj_circle(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    int32_t center_x = (int32_t)mp_obj_get_int(args[2]);
    int32_t center_y = (int32_t)mp_obj_get_int(args[3]);
    int32_t radius = (int32_t)mp_obj_get_int(args[4]);
    LOG_D("src = %p;center_x = %d; center_y = %d; radius = %d;\n", src, center_x, center_y, radius);

    ret = ImageProcCircle(driver_obj->mInstance, src, center_x, center_y, radius);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_circle, 5, obj_circle);

STATIC mp_obj_t obj_drawMarker(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    uint32_t x = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[3]);
    int32_t type = (int32_t)mp_obj_get_int(args[4]);
    LOG_D("src = %p;x = %d; y = %d; type = %d;\n", src, x, y, type);
    Point_t pt;
    pt.x = x;
    pt.y = y;

    ret = ImageProcDrawMarker(driver_obj->mInstance, src, &pt, type);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_drawMarker, 5, obj_drawMarker);

STATIC mp_obj_t obj_fillPoly(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    uint32_t x = (uint32_t)mp_obj_get_int(args[2]);
    uint32_t y = (uint32_t)mp_obj_get_int(args[3]);
    int32_t color = (int32_t)mp_obj_get_int(args[4]);
    LOG_D("src = %p;x = %d; y = %d; color = %d;\n", src, x, y, color);
    Point_t pt;
    pt.x = x;
    pt.y = y;
    Point_t *ptt = &pt;

    ret = ImageProcFillPoly(driver_obj->mInstance, src, &ptt, color);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_fillPoly, 5, obj_fillPoly);

STATIC mp_obj_t obj_putText(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    char* text = (char*)mp_obj_str_get_str(args[2]);
    int32_t left = (int32_t)mp_obj_get_int(args[3]);
    int32_t top = (int32_t)mp_obj_get_int(args[4]);
    LOG_D("src = %p;text = %s; left = %d; top = %d;\n", src, text, left, top);

    ret = ImageProcPutText(driver_obj->mInstance, src, text, left, top);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_putText, 5, obj_putText);

STATIC mp_obj_t obj_dilateErode(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    int32_t iMode = (int32_t)mp_obj_get_int(args[3]);
    int32_t iThresh = (int32_t)mp_obj_get_int(args[4]);
    LOG_D("src = %p;dst = %p; iMode = %d; iThresh = %d;\n", src, dst, iMode, iThresh);

    ret = ImageProcDilateErode(driver_obj->mInstance, src, dst, iMode, iThresh);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_dilateErode, 5, obj_dilateErode);

STATIC mp_obj_t obj_imageAdd(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 5)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* added = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[3]);
    LOG_D("src = %p;added = %p; dst = %p;\n", src, added, dst);

    ImageProcImageAdd(driver_obj->mInstance, src, added, dst);
    LOG_D("%s:out \n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_imageAdd, 4, obj_imageAdd);

STATIC mp_obj_t obj_imageMinus(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* minused = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[3]);
    LOG_D("src = %p;minused = %p; dst = %p;\n", src, minused, dst);

    ImageProcImageMinus(driver_obj->mInstance, src, minused, dst);
    LOG_D("%s:out \n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_imageMinus, 4, obj_imageMinus);

STATIC mp_obj_t obj_imageElementMultiply(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* multiplied = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[3]);
    LOG_D("src = %p;multiplied = %p; dst = %p;\n", src, multiplied, dst);

    ImageProcImageElementMultiply(driver_obj->mInstance, src, multiplied, dst);
    LOG_D("%s:out \n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_imageElementMultiply, 4, obj_imageElementMultiply);

STATIC mp_obj_t obj_imageElementDivide(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    if (n_args < 4)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    ImageBuffer_t* divied = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[2]);
    ImageBuffer_t* dst = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[3]);
    LOG_D("src = %p;divied = %p; dst = %p;\n", src, divied, dst);

    ImageProcImageElementDivide(driver_obj->mInstance, src, divied, dst);
    LOG_D("%s:out \n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_imageElementDivide, 4, obj_imageElementDivide);

STATIC mp_obj_t obj_writeImageToFile(size_t n_args, const mp_obj_t *args)
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
    mp_imageproc_obj_t* driver_obj = (mp_imageproc_obj_t *)self;
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

    ImageBuffer_t* src = (ImageBuffer_t*)MP_OBJ_TO_PTR(args[1]);
    char* file_name = (char*)mp_obj_str_get_str(args[2]);
    LOG_D("src = %p;file_name = %s;\n", src, file_name);

    ret = ImageProcWriteImageToFile(driver_obj->mInstance, src, file_name);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(imageproc_obj_writeImageToFile, 3, obj_writeImageToFile);

STATIC const mp_rom_map_elem_t imageproc_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ImageProc)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IMAGE_PROC_NONE), MP_ROM_INT(IMAGE_PROC_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IMAGE_PROC_CPU), MP_ROM_INT(IMAGE_PROC_CPU)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IMAGE_PROC_OPENCV), MP_ROM_INT(IMAGE_PROC_OPENCV)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IMAGE_PROC_HAL), MP_ROM_INT(IMAGE_PROC_HAL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_IMAGE_PROC_MAX), MP_ROM_INT(IMAGE_PROC_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&imageproc_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&imageproc_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_cvtColor), MP_ROM_PTR(&imageproc_obj_cvtColor)},
    {MP_ROM_QSTR(MP_QSTR_resize), MP_ROM_PTR(&imageproc_obj_resize)},
    {MP_ROM_QSTR(MP_QSTR_imgCopy), MP_ROM_PTR(&imageproc_obj_imgCopy)},
    {MP_ROM_QSTR(MP_QSTR_rectangle), MP_ROM_PTR(&imageproc_obj_rectangle)},
    {MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&imageproc_obj_circle)},
    {MP_ROM_QSTR(MP_QSTR_drawMarker), MP_ROM_PTR(&imageproc_obj_drawMarker)},
    {MP_ROM_QSTR(MP_QSTR_fillPoly), MP_ROM_PTR(&imageproc_obj_fillPoly)},
    {MP_ROM_QSTR(MP_QSTR_putText), MP_ROM_PTR(&imageproc_obj_putText)},
    {MP_ROM_QSTR(MP_QSTR_dilateErode), MP_ROM_PTR(&imageproc_obj_dilateErode)},
    {MP_ROM_QSTR(MP_QSTR_imageAdd), MP_ROM_PTR(&imageproc_obj_imageAdd)},
    {MP_ROM_QSTR(MP_QSTR_imageMinus), MP_ROM_PTR(&imageproc_obj_imageMinus)},
    {MP_ROM_QSTR(MP_QSTR_imageElementMultiply), MP_ROM_PTR(&imageproc_obj_imageElementMultiply)},
    {MP_ROM_QSTR(MP_QSTR_imageElementDivide), MP_ROM_PTR(&imageproc_obj_imageElementDivide)},
    {MP_ROM_QSTR(MP_QSTR_writeImageToFile), MP_ROM_PTR(&imageproc_obj_writeImageToFile)},
};

STATIC MP_DEFINE_CONST_DICT(imageproc_locals_dict, imageproc_locals_dict_table);

const mp_obj_type_t minicv_imageproc_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ImageProc,
    .print = imageproc_obj_print,
    .make_new = imageproc_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&imageproc_locals_dict,
};

