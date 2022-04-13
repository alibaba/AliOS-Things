#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "HaasLog.h"
#include "WrapperIHaasDataInput.h"

extern const mp_obj_type_t minicv_datainput_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t   Base;
    // a member created by us
    char            *ModuleName;
    DataInputType_t mDataInputType;
    CameraNumber_t  mCameraNum;
    char            *mFileName;
    void            *mInstance;
} mp_datainput_obj_t;

void datainput_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_datainput_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t datainput_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_datainput_obj_t* driver_obj = m_new_obj(mp_datainput_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_datainput_type;
    driver_obj->ModuleName = "minicv-DataInput";
    driver_obj->mDataInputType = DATA_INPUT_SOURCE_NONE;
    driver_obj->mCameraNum = DATA_INPUT_CAMERA_NUMBER_NONE;
    driver_obj->mFileName = NULL;
    driver_obj->mInstance = NULL;

    return MP_OBJ_FROM_PTR(driver_obj);
}

STATIC mp_obj_t obj_open(size_t n_args, const mp_obj_t *args)
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
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
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

    driver_obj->mDataInputType = (DataInputType_t)mp_obj_get_int(args[1]);
    driver_obj->mFileName = (char *)mp_obj_str_get_str(args[2]);
    LOG_D("%s:mDataInputType = %d;\n", __func__, driver_obj->mDataInputType);
    LOG_D("%s:mFileName = %s;\n", __func__, driver_obj->mFileName);
    instance = DataInputCreateInstance(driver_obj->mDataInputType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("DataInputCreateInstance failed\n");
        return mp_const_none;
    }

    ret = DataInputOpen(instance, driver_obj->mFileName);
    if(ret)
    {
        LOG_E("\r\n=====datainput open failed===\r\n");
        return mp_const_none;
    }
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_open, 3, obj_open);

STATIC mp_obj_t obj_openCamera(size_t n_args, const mp_obj_t *args)
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
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
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

    driver_obj->mDataInputType = (DataInputType_t)mp_obj_get_int(args[1]);
    driver_obj->mCameraNum = (CameraNumber_t)mp_obj_get_int(args[2]);
    LOG_D("%s:mDataInputType = %d;\n", __func__, driver_obj->mDataInputType);
    LOG_D("%s:mCameraNum = %d;\n", __func__, driver_obj->mCameraNum);
    instance = DataInputCreateInstance(driver_obj->mDataInputType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("DataInputCreateInstance failed\n");
        return mp_const_none;
    }

    ret = DataInputOpen2(instance, driver_obj->mCameraNum);
    if(ret)
    {
        LOG_E("\r\n=====datainput opencamera failed===\r\n");
        return mp_const_none;
    }

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_openCamera, 3, obj_openCamera);

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
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
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

    DataInputClose(driver_obj->mInstance);
    driver_obj->mDataInputType = DATA_INPUT_SOURCE_NONE;
    driver_obj->mCameraNum = DATA_INPUT_CAMERA_NUMBER_NONE;
    driver_obj->mFileName = NULL;
    driver_obj->mInstance = NULL;

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_close, 1, obj_close);

STATIC mp_obj_t obj_checkDataReady(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    void* instance = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has not been opened, please open first\n");
        return mp_const_none;
    }

    int state = (int)DataInputCheckDataReady(driver_obj->mInstance);
    mp_obj_t ret = mp_obj_new_bool(state);
    LOG_D("%s:out state = %d;\n", __func__, state);

    return MP_ROM_INT(&ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_checkDataReady, 1, obj_checkDataReady);

STATIC mp_obj_t obj_requestData(size_t n_args, const mp_obj_t *args)
{
    LOG_D("entern  %s; n_args = %d;\n", __func__, n_args);
    int ret = -1;
    void* instance = NULL;
    ImageBuffer_t *image = NULL;
    if (n_args < 1)
    {
        LOG_E("%s: args num is illegal :n_args = %d;\n", __func__, n_args);
        return mp_const_none;
    }
    mp_obj_base_t *self = (mp_obj_base_t*)MP_OBJ_TO_PTR(args[0]);
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }
    if (driver_obj->mInstance == NULL)
    {
        LOG_E("Module has not been opened, please open first\n");
        return mp_const_none;
    }

    ret = DataInputRequestData(driver_obj->mInstance, &image, 1000);
    if (ret != 0)
    {
        LOG_E("Request Data failed\n");
        return mp_const_none;
    }

    LOG_D("%s:out image->address[0] = %p;image = %p;\n", __func__, image->address[0], image);

    return MP_OBJ_FROM_PTR(image);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_requestData, 1, obj_requestData);

STATIC mp_obj_t obj_releaseData(size_t n_args, const mp_obj_t *args)
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
    mp_datainput_obj_t* driver_obj = (mp_datainput_obj_t *)self;
    if (driver_obj == NULL)
    {
        LOG_E("driver_obj is NULL\n");
        return mp_const_none;
    }

    ImageBuffer_t *image = (ImageBuffer_t *)MP_OBJ_TO_PTR(args[1]);
    if (image == NULL)
    {
        LOG_E("image is NULL\n");
        return mp_const_none;
    }

    ret = DataInputReleaseData(driver_obj->mInstance, image);
    if (ret != 0)
    {
        LOG_E("Release Data failed\n");
        return mp_const_none;
    }

    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(datainput_obj_releaseData, 2, obj_releaseData);

STATIC const mp_rom_map_elem_t datainput_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_DataInput)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_CAMERA_NUMBER_NONE), MP_ROM_INT(DATA_INPUT_CAMERA_NUMBER_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_CAMERA_NUMBER_FRONT), MP_ROM_INT(DATA_INPUT_CAMERA_NUMBER_FRONT)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_CAMERA_NUMBER_BACK), MP_ROM_INT(DATA_INPUT_CAMERA_NUMBER_BACK)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_CAMERA_NUMBER_MAX), MP_ROM_INT(DATA_INPUT_CAMERA_NUMBER_MAX)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_NONE),
		MP_ROM_INT(DATA_INPUT_SOURCE_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_IMAGE_BMP),
		MP_ROM_INT(DATA_INPUT_SOURCE_IMAGE_BMP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_IMAGE_GIF),
		MP_ROM_INT(DATA_INPUT_SOURCE_IMAGE_GIF)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_IMAGE_PNG),
		MP_ROM_INT(DATA_INPUT_SOURCE_IMAGE_PNG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_IMAGE_JPG),
		MP_ROM_INT(DATA_INPUT_SOURCE_IMAGE_JPG)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_VIDEO_FILE),
		MP_ROM_INT(DATA_INPUT_SOURCE_VIDEO_FILE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_VIDEO_RTMP),
		MP_ROM_INT(DATA_INPUT_SOURCE_VIDEO_RTMP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_VIDEO_RTSP),
		MP_ROM_INT(DATA_INPUT_SOURCE_VIDEO_RTSP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_CAMERA_USB),
		MP_ROM_INT(DATA_INPUT_SOURCE_CAMERA_USB)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_CAMERA_LOCAL),
		MP_ROM_INT(DATA_INPUT_SOURCE_CAMERA_LOCAL)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_CAMERA_IP),
		MP_ROM_INT(DATA_INPUT_SOURCE_CAMERA_IP)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_DATA_INPUT_SOURCE_MAX),
		MP_ROM_INT(DATA_INPUT_SOURCE_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&datainput_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_openCamera), MP_ROM_PTR(&datainput_obj_openCamera)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&datainput_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_checkDataReady), MP_ROM_PTR(&datainput_obj_checkDataReady)},
    {MP_ROM_QSTR(MP_QSTR_requestData), MP_ROM_PTR(&datainput_obj_requestData)},
    {MP_ROM_QSTR(MP_QSTR_releaseData), MP_ROM_PTR(&datainput_obj_releaseData)},
};

STATIC MP_DEFINE_CONST_DICT(datainput_locals_dict, datainput_locals_dict_table);

const mp_obj_type_t minicv_datainput_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_DataInput,
    .print = datainput_obj_print,
    .make_new = datainput_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&datainput_locals_dict,
};



