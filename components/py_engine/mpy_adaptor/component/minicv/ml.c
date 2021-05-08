#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "py/mperrno.h"
#include "py/obj.h"
#include "py/runtime.h"
#include "py/builtin.h"
#include "k_api.h"
#include "WrapperIHaasML.h"
#include "HaasLog.h"

extern const mp_obj_type_t minicv_ml_type;
// this is the actual C-structure for our new object
typedef struct
{
    // base represents some basic information, like type
    mp_obj_base_t Base;
    // a member created by us
    char *ModuleName;
    MLEngineType_t  mType;
    void            *mInstance;
} mp_ml_obj_t;

void ml_obj_print(const mp_print_t *print, mp_obj_t self_in, mp_print_kind_t kind)
{
    LOG_D("entern %s;\n", __func__);
    mp_ml_obj_t *self = MP_OBJ_TO_PTR(self_in);
    mp_printf(print, "ModuleName(%s)", self->ModuleName);
}

STATIC mp_obj_t ml_obj_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *args)
{
    LOG_D("entern  %s;\n", __func__);
    mp_ml_obj_t* driver_obj = m_new_obj(mp_ml_obj_t);
    if (!driver_obj) {
        mp_raise_OSError(ENOMEM);
    }

    driver_obj->Base.type = &minicv_ml_type;
    driver_obj->ModuleName = "minicv-ML";
    driver_obj->mType = ML_ENGINE_NONE;
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    driver_obj->mType = (MLEngineType_t)mp_obj_get_int(args[1]);
    LOG_D("%s:mType = %d;\n", __func__, driver_obj->mType);
    instance = MLCreateInstance(driver_obj->mType);
    driver_obj->mInstance = instance;
    if (instance == NULL)
    {
        LOG_E("MLCreateInstance failed\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_open, 2, obj_open);

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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    MLDestoryInstance(driver_obj->mInstance);
    driver_obj->mType = ML_ENGINE_NONE;
    driver_obj->mInstance = NULL;
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_close, 1, obj_close);

STATIC mp_obj_t obj_config(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    char *key = (char *)mp_obj_str_get_str(args[1]);
    char *secret = (char *)mp_obj_str_get_str(args[2]);
    char *region_id = (char *)mp_obj_str_get_str(args[3]);
    char *endpoint = (char *)mp_obj_str_get_str(args[4]);
    char *url = (char *)mp_obj_str_get_str(args[5]);
    LOG_D("key = %s;\n", key);
    LOG_D("secret = %s;\n", secret);
    LOG_D("region_id = %s;\n", region_id);
    LOG_D("endpoint = %s;\n", endpoint);
    LOG_D("url = %s;\n", url);
    ret = MLConfig(driver_obj->mInstance, key, secret, region_id,
            endpoint, url);
    if (ret != 0)
    {
        LOG_E("MLConfig failed\n");
        return mp_const_none;
    }

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_config, 6, obj_config);

STATIC mp_obj_t obj_setInputData(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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
    ret = MLSetInputData(driver_obj->mInstance, mFileName);
    if (ret != 0)
    {
        LOG_E("MLSetInputData failed mFileName = %s;\n", mFileName);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_setInputData, 2, obj_setInputData);

STATIC mp_obj_t obj_loadNet(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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
    ret = MLLoadNet(driver_obj->mInstance, mFileName);
    if (ret != 0)
    {
        LOG_E("MLLoadNet failed mFileName = %s;\n", mFileName);
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_loadNet, 2, obj_loadNet);

STATIC mp_obj_t obj_predict(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    ret = MLPredict(driver_obj->mInstance);
    if (ret != 0)
    {
        LOG_E("MLPredict failed\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_predict, 1, obj_predict);

STATIC mp_obj_t obj_getPredictResponses(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(args[1], &bufinfo, MP_BUFFER_WRITE);
    memset(bufinfo.buf, 0, bufinfo.len);
    LOG_D("bufinfo.buf = %p;bufinfo.len = %d;\n", bufinfo.buf, bufinfo.len);
    ret = MLGetPredictResponses(driver_obj->mInstance, bufinfo.buf, bufinfo.len);
    if (ret != NULL)
    {
        LOG_E("MLGetPredictResponses failed\n");
        //return mp_const_none;
    }
    LOG_D("bufinfo.buf = %s;bufinfo.len = %d;\n", bufinfo.buf, bufinfo.len);
    LOG_D("%s:out ret = %d;\n", __func__, ret);

    return MP_ROM_INT(ret);
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_getPredictResponses, 2, obj_getPredictResponses);

STATIC mp_obj_t obj_unLoadNet(size_t n_args, const mp_obj_t *args)
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
    mp_ml_obj_t* driver_obj = (mp_ml_obj_t *)self;
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

    ret = MLUnLoadNet(driver_obj->mInstance);
    if (ret != 0)
    {
        LOG_E("MLUnLoadNet failed\n");
        return mp_const_none;
    }
    LOG_D("%s:out\n", __func__);

    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR(ml_obj_unLoadNet, 1, obj_unLoadNet);

STATIC const mp_rom_map_elem_t ml_locals_dict_table[] = {
    {MP_OBJ_NEW_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_ML)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_NONE), MP_ROM_INT(ML_ENGINE_NONE)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_MNN), MP_ROM_INT(ML_ENGINE_MNN)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_CLOUD), MP_ROM_INT(ML_ENGINE_CLOUD)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_ODLA), MP_ROM_INT(ML_ENGINE_ODLA)},
    {MP_OBJ_NEW_QSTR(MP_QSTR_ML_ENGINE_MAX), MP_ROM_INT(ML_ENGINE_MAX)},
    {MP_ROM_QSTR(MP_QSTR_open), MP_ROM_PTR(&ml_obj_open)},
    {MP_ROM_QSTR(MP_QSTR_close), MP_ROM_PTR(&ml_obj_close)},
    {MP_ROM_QSTR(MP_QSTR_config), MP_ROM_PTR(&ml_obj_config)},
    {MP_ROM_QSTR(MP_QSTR_setInputData), MP_ROM_PTR(&ml_obj_setInputData)},
    {MP_ROM_QSTR(MP_QSTR_loadNet), MP_ROM_PTR(&ml_obj_loadNet)},
    {MP_ROM_QSTR(MP_QSTR_predict), MP_ROM_PTR(&ml_obj_predict)},
    {MP_ROM_QSTR(MP_QSTR_getPredictResponses), MP_ROM_PTR(&ml_obj_getPredictResponses)},
    {MP_ROM_QSTR(MP_QSTR_unLoadNet), MP_ROM_PTR(&ml_obj_unLoadNet)},
};

STATIC MP_DEFINE_CONST_DICT(ml_locals_dict, ml_locals_dict_table);

const mp_obj_type_t minicv_ml_type = {
    .base = {&mp_type_type},
    .name = MP_QSTR_ML,
    .print = ml_obj_print,
    .make_new = ml_obj_make_new,
    .locals_dict = (mp_obj_dict_t *)&ml_locals_dict,
};
